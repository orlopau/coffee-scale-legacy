const UPDATE_INTERVAL = 1000

const IDs = {
  weight: "#val-weight",
  time: "#val-time",
  estimatedTime: "#val-time-est",
  diagram: "#diagram",
  mode: "#title"
}

let options = {
  chart: {
    type: "line",
    toolbar: {
      show: false
    },
    zoom: {
      enabled: false
    },
    animations: {
      enabled: false,
      dynamicAnimation: {
        speed: UPDATE_INTERVAL
      }
    }
  },
  series: [
    {
      data: [],
    },
  ],
  xaxis: {
    type: "numeric",
    decimalsInFloat: 1,
    title: {
      text: "Time (s)"
    },
    labels: {
      formatter: (val) => {
        return (val / 1000).toFixed(0)
      }
    }
  },
  yaxis: {
    decimalsInFloat: 0,
    title: {
      text: "Weight (g)"
    },
  },
  dataLabels: {
    enabled: false
  },
  stroke: {
    curve: ["smooth", "straight"],
    width: [3, 1],
    dashArray: [0, 8]
  },
  markers: {
    size: 0
  },
  legend: {
    show: false
  },
}

// eslint-disable-next-line no-undef
let chart = new ApexCharts(document.querySelector(IDs.diagram), options)
chart.render()

function formatTime(time) {
  return `${(time / 1000).toFixed(1)}s`
}

function formatWeight(weight) {
  return `${weight.toFixed(1)}g`
}

const updateView = (data) => {
  let seriesData = []

  // merge two elements into one pair
  for (let i = 0; i < data.history.length - 1; i += 2) {
    seriesData.push([data.history[i], data.history[i + 1] / 100])
  }

  chart.updateOptions({
    series: [{ data: seriesData }],
    // xaxis: {
    //   min: seriesData[0][0],
    //   max: Math.max(seriesData[seriesData.length - 1][0], data.mode.estimatedTime)
    // },
    annotations: {
      xaxis: [{
        id: "estimatedTime",
        x: data.mode.estimatedTime,
        label: {
          text: "ETA"
        }
      }],
      yaxis: [{
        id: "targetWeight",
        y: data.mode.targetWeight,
        label: {
          text: "Target"
        }
      }]
    }
  })

  const stoppedTime = data.mode.stopwatchStart !== 0 ? data.currentTime - data.mode.stopwatchStart : 0
  document.querySelector(IDs.mode).innerHTML = data.currentMode.toUpperCase()
  document.querySelector(IDs.weight).innerHTML = formatWeight(data.currentWeight)
  document.querySelector(IDs.estimatedTime).innerHTML = formatTime(data.mode.estimatedTime - data.mode.stopwatchStart)
  document.querySelector(IDs.time).innerHTML = formatTime(stoppedTime)
}

let inInterval = false

setInterval(async () => {
  if (inInterval) {
    return
  }
  inInterval = true

  try {
    // const response = await fetch(`http://${window.location.hostname}:1122/data/info`)
    const response = await fetch("http://coffee-scale.local:1122/api/info")
    const data = await response.json()
    updateView(data)
  } catch (e) {
    console.info(e)
  }

  inInterval = false
}, UPDATE_INTERVAL)
