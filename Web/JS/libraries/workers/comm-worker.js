/* Global Variables */
var runningStatus = true,
    xmlHTTPObj = new XMLHttpRequest(),
    sensorId = null,
    serverEndpt = "",
    port = 0,
    path = "",
    fps = 1;

/* External Script */

/* Main */
self.addEventListener('message', function (event) {

    var data = event.data;
     
    switch (data.cmd) {
        case 'start':

            //Configuration
            runningStatus = true;
            serverEndpt = data.serverEndPt;
            port = data.port;
            fps = data.fps;
            path = data.path;
            type = data.type;

            if (type == "single") {
                sensorId = data.sensorId;
            }

            getSceneFromServer();
            postMessage({ type: "status", msg: "Worker Started" });
            break;
        case 'stop':
            runningStatus = false;
            postMessage({ type: "status", msg: "Worker Stopping" });
            self.close();
            break;
        default:
            postMessage("Error in Web Worker : " + data.msg);
            break;
    }

});

/* Functions */
function getSceneFromServer() {

    var connections = 0,
        updateDelay = 1000/fps;

    if (serverEndpt != "") {
        xmlHTTPObj.onreadystatechange = sceneCallback;
        xmlHTTPObj.open("POST",serverEndpt + ":" + port + path,true);
        xmlHTTPObj.setRequestHeader('Content-Type', 'application/javascript');
        xmlHTTPObj.setRequestHeader('Request-Type', type);
        
        if (type == "single") {
            xmlHTTPObj.setRequestHeader('Sensor-Id', sensorId);
        }

        xmlHTTPObj.send();
    }

    if (runningStatus) {
        setTimeout(getSceneFromServer, updateDelay);
    }
}

/* Callbacks */

function sceneCallback() {
    if (xmlHTTPObj.readyState === 4) {
        if (xmlHTTPObj.status === 200) {
            postMessage({ type: "status", msg: "Response Received" });
            postMessage({ type: "data", msg: xmlHTTPObj.responseText});
        } else if (xmlHTTPObj.status == 404) {
            postMessage({ type: "error", msg: "Server Endpt not found" });
        } else {
            postMessage({ type: "error", msg: "Connection Failed" });
        }
    }
}