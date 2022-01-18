# Info

Used to retrieve info about the current state of the scale.

**URL** : `/api/info`

**Method** : `GET`

### Query Params

**Optional:**

* `history=[boolean]` True if response should include history.

## Success Response

**Code** : `200 OK`

**Content example**

Currently, the only available mode info is about the regression mode.

```json
{
    "currentMode": "regression",
    "currentTime" : 12345, // current millis()
    "currentWeight": 30, // current weight in g
    "mode": {
        "targetWeight": 39.5, // target weight in g
        "stopwatchStart": 10000, // time when the stopwatch was started
        "estimatedTime": 45000 // estimated time (in arduino-millis) at which the target weight is reached
    },
    "history": [200, 3950,  300, 4000, 400, 4100]
    // contains tuples of ( timestamp (ms), weight (g) ) in an array
}
```