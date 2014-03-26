
jQuery(document).ready(function (){
    IndexPage.Init();
});

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

    StartCommunicationWithScene: function () {
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

    Diagnostic: function () {
        if (this.networkClient
           && this.visualisationCanvas0
           && this.visualisationCanvas1
           ) {
            console.log("Libraries Initialisation Successful");
            return true;
        } else {
            return false;
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
                    //yellow
                    skeletonGeometry = skeletonObj.getGeometry(0xffff00);
                }

                glScene.add(skeletonGeometry);
            }

            return glScene;
        } else {
            return null;
        }
    }
};