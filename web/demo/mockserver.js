import http from "http"
import url from "url"
import CBuffer from "CBuffer"

const hostname = "0.0.0.0"
const port = 1122

// max time in ms
const maxTime = 32000
// max weight
const maxWeight = 39.5
// update interval
const updateInterval = 100
const estimatedTime = 32000

const buffer = new CBuffer(200)

const demoData = {
    currentMode: "regression",
    currentTime: 15000,
    mode: {
        targetWeight: 39.5,
        stopwatchStart: 10000,
        estimatedTime: estimatedTime,
    },
    history: []
}

const intervals = maxTime / updateInterval
let increment = maxWeight / intervals
setInterval(() => {
    const lastElement = buffer.last()
    if (lastElement == undefined) {
        // set initial element
        return buffer.push([0, 0])
    }

    const newElement = []

    // if weight is above max, reverse
    if (lastElement[1] >= maxWeight || lastElement[1] < 0) {
        increment *= -1
    }

    newElement[0] = lastElement[0] + updateInterval
    newElement[1] = lastElement[1] + increment

    buffer.push(newElement)

    demoData.history = buffer.toArray().map(e => [e[0], e[1] * 100]).flat()
}, updateInterval)

const server = http.createServer((req, res) => {
    const reqUrl = url.parse(req.url)
    res.setHeader("Content-Type", "application/json")
    res.setHeader("Access-Control-Allow-Origin", "*")
    res.statusCode = 200

    if (reqUrl.pathname == "/data/info" && req.method == "GET") {
        return res.end(JSON.stringify(demoData))
    }

    res.statusCode = 404
    return res.end()
})

server.listen(port, hostname, () => {
    console.log(`Server running at http://${hostname}:${port}/`)
})
