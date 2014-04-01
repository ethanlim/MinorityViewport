
jQuery(document).ready(function (){
    IndexPage.Init();
});

/* Timer */
var d = new Date;
var timeSent = 0;
var timeTaken_s;
var timeTaken_ms;

function onInitFs(fs) {
    fs.root.getFile("streaming.txt", { create: true }, function (DataFile) {
        DataFile.createWriter(function (DataContent) {

            DataContent.seek(DataContent.length);

            var blob = new Blob([timeTaken_ms +"\n"], { type: "text/plain" });
            DataContent.write(blob);
        });
    });
}

function getByteLen(normal_val) {
    // Force string type
    normal_val = String(normal_val);

    var byteLen = 0;
    for (var i = 0; i < normal_val.length; i++) {
        var c = normal_val.charCodeAt(i);
        byteLen += c < (1 << 7) ? 1 :
                   c < (1 << 11) ? 2 :
                   c < (1 << 16) ? 3 :
                   c < (1 << 21) ? 4 :
                   c < (1 << 26) ? 5 :
                   c < (1 << 31) ? 6 : Number.NaN;
    }
    return byteLen;
}

var IndexPage = {

    networkClient: null,
    visualisationCanvas: null,

    Init: function () {
        this.networkClient = Network.initClient(GlobalVar.hostURL,GlobalVar.port),
        this.visualisationCanvas = new Visualisation();
        localStorage.setItem("globalScene", "");

        this.Diagnostic();

        var clients = this.networkClient.fetchedConnectedClients();

        this.visualisationCanvas.init({ id: "canvas-container", height: "600px", width: "100%" }
                                       , { wireFrameColor: 0x3300FF, backgroundColor: 0xB6B6B4,float:'none'}
                                       , this.ReconstructFn);
        this.visualisationCanvas.render("canvas-container");

        this.UpdateSensorTable(clients);

        this.StartCommunicationWithScene();
    },

    Diagnostic: function () {
        if (this.networkClient
            && this.visualisationCanvas) {
            console.log("Libraries Initialisation Successful");
            return true;
        } else {
            if (!this.networkClient) {

                this.networkClient = Network.initClient("localhost", GlobalVar.port);

                console.log("Network Connection Failed : Default to Localhost");
            } else {
                console.log("Network Connection Failed : Fail Safe failed");
            }

            return false;
        }
    },

    StartCommunicationWithScene: function () {
        timeSent = d.getTime()
        return this.networkClient.startCommWorker({ type: "global"}, this.NetworkHandler);
    },

    NetworkHandler: function (event) {
        var responseType = event.data.type;
        var responseData = event.data.msg;

        if (responseType == "data") {
            if (responseData != "") {
                var sceneFromServer = JSON.parse(responseData);
                localStorage.setItem("globalScene", JSON.stringify(sceneFromServer));
            } else {
                localStorage.setItem("globalScene", "");
            }
        }
    },

    UpdateSensorTable: function (clients) {
        if (clients.length == 0) {
            $("#sensor-info-count").text("0");
        } else {
            var numOfClients = clients.length;

            $("#sensor-info-count").text(numOfClients);

            for (var client = 0; client < numOfClients; client += 1) {

                var newClientRow = document.createElement("tr");

                var clientIdCell = document.createElement("td");
                clientIdCell.innerHTML = clients[client]["id"];

                var clientLocCell = document.createElement("td");
                clientLocCell.innerHTML = clients[client]["location"];

                var clientAddressCell = document.createElement("td");
                clientAddressCell.innerHTML = clients[client]["address"];

                newClientRow.appendChild(clientIdCell);
                newClientRow.appendChild(clientLocCell);
                newClientRow.appendChild(clientAddressCell);

                var sensors = clients[client]["sensors"];
                var numOfSensors = sensors.length;
                var sensorIdList = [];

                for (var sensor = 0; sensor < numOfSensors; sensor += 1) {

                    // Add to the sensor list    
                    sensorIdList.push(sensors[sensor]["id"]);
                    jQuery.data(document.body, "sensorIdList", sensorIdList);

                    var sensorId = document.createElement("td");
                    sensorId.innerHTML = sensors[sensor]["id"];

                    var sensorOrdering = document.createElement("td");
                    sensorOrdering.innerHTML = sensors[sensor]["ordering"];

                    var sensorCalibrated = document.createElement("td");
                    sensorCalibrated.innerHTML = sensors[sensor]["calibrated"];

                    if (sensor == 0) {

                        newClientRow.id = sensors[sensor]["id"];

                        newClientRow.appendChild(sensorId);
                        newClientRow.appendChild(sensorCalibrated);
                        newClientRow.appendChild(sensorOrdering);

                        $("#sensor-info > tbody").append(newClientRow.outerHTML);
                    } else {
                        var additionalClientRow = document.createElement("tr");
                        additionalClientRow.id = sensors[sensor]["id"];

                        var blankCell = document.createElement("td");
                        blankCell.innerHTML = "";
                        var blankCell1 = document.createElement("td");
                        blankCell.innerHTML = "";
                        var blankCell2 = document.createElement("td");
                        blankCell.innerHTML = "";

                        additionalClientRow.appendChild(blankCell);
                        additionalClientRow.appendChild(blankCell1);
                        additionalClientRow.appendChild(blankCell2);

                        additionalClientRow.appendChild(sensorId);
                        additionalClientRow.appendChild(sensorCalibrated);
                        additionalClientRow.appendChild(sensorOrdering);

                        $("#sensor-info > tbody").append(additionalClientRow.outerHTML);
                    }
                }

            }
        }
    },

    ReconstructFn: function (glScene, canvasId) {
        
        // Remove all skeletons previously inserted into scene
        for (var child = 7; child < glScene.children.length; child++) {
            glScene.remove(glScene.children[child]);
        }

        /* Choose the global scene to draw*/
        var globalSceneJSON = localStorage.getItem("globalScene");
        var globalScene;
        if(globalSceneJSON!=""){
            globalScene = JSON.parse(globalSceneJSON);
        }

        if (globalScene != null) {
            for (var newSkeleton in globalScene["skeletons"]) {

                var skeletonObj = new Skeleton(globalScene["skeletons"][newSkeleton]);
                var skeletonGeometry;

                if (globalScene["skeletons"][newSkeleton]["shared"] == "true") {
                    //purple
                    skeletonGeometry = skeletonObj.getGeometry(0xA30000);
                } else {
                    //orange
                    skeletonGeometry = skeletonObj.getGeometry(0xFF9933);
                }

                glScene.add(skeletonGeometry);
            }

            var d = new Date;
            timeTaken_ms = Math.round((d.getTime() - timeSent) / 10);
            timeTaken_s = timeTaken_ms / 1000;
            timeSent += timeTaken_ms;

            window.webkitRequestFileSystem(window.TEMPORARY, 1024 * 1024, onInitFs)

            return glScene;
        } else {
            return null;
        }
    }
};