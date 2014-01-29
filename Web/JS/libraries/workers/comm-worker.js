/* Global Variables */
var runningStatus = true,
    xmlHTTPObj = new XMLHttpRequest(),
    serverEndpt = "",
    port = 0,
    path = "/api/visualisations/data.json",
    fps = 1;

/* External Script */

/* Main */
self.addEventListener('message', function (event) {

    var data = event.data;
     
    switch (data.cmd) {
        case 'start':
            runningStatus = true;
            serverEndpt = data.serverEndPt;
            port = data.port;
            fps = data.fps;
            getGlobalSceneFromServer();
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
function getGlobalSceneFromServer() {

    var connections = 0,
        updateDelay = 1000/fps;

    if (serverEndpt != "") {
        xmlHTTPObj.onreadystatechange = globalSceneCallback;
        xmlHTTPObj.open("POST",serverEndpt + ":" + port + path,true);
        xmlHTTPObj.setRequestHeader("DATAREQ", "GLOBALSCENE");
        xmlHTTPObj.setRequestHeader('Content-Type', 'application/javascript');
        xmlHTTPObj.send();
       
    }

    if (runningStatus) {
        setTimeout(getGlobalSceneFromServer, updateDelay);
    }
}

/* Callbacks */

function globalSceneCallback() {
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