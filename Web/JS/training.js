jQuery(document).ready(function () {

    TrainingPage.Init();

});

var TrainingPage = {

    clients: null,
    networkClient: null,
    visualisationCanvas0: null,
    commWorkersId: [],

    automaticSkeletonTimeoutVar: null,
    trainingBtnToggle: false,

    Init: function () {

        this.networkClient = Network.initClient(GlobalVar.hostURL, GlobalVar.port);
        this.visualisationCanvas0 = new Visualisation();

        var scenes = new Array(1)
        scenes[0] = { sensorId: null, scene: null };
        jQuery.data(document.body, "scenes", scenes);

        this.Diagnostic();

        this.clients = this.networkClient.fetchedConnectedClients();

        this.visualisationCanvas0.init({ id: "canvas-container-0", height: "500px", width: "1140px" }
                                       , { wireFrameColor: 0x3300FF, backgroundColor: 0xB6B6B4, float: 'left' }
                                       , this.ReconstructFn);
        this.visualisationCanvas0.render("canvas-container-0");

        localStorage.setItem("trainingMode", "false");

        this.UpdateSensorTable(this.clients);
        this.UpdateSensorDDL(this.clients);

        if (this.clients.length == 0) {
            this.UpdateCalibrationMenuStatus("No Client Detected", "danger");
        } else {
            this.UpdateCalibrationMenuStatus("Awaiting Selections", "primary");
        }

        /* Establish the Event Handlers */

        this.SetupEventHandlers();
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

    UpdateSensorDDL: function (clients) {

        var buttonGrp = ".scene-selection > div.btn-group";

        jQuery(buttonGrp).remove("ul");

        var ul = document.createElement("ul");
        ul.setAttribute("role", "menu");
        ul.setAttribute("class", "dropdown-menu");


        for (var client = 0; client < clients.length; client += 1) {
            var sensors = clients[client]["sensors"];
            for (var sensor = 0; sensor < sensors.length; sensor += 1) {
                var li = document.createElement("li");
                var a = document.createElement("a");
                a.textContent = sensors[sensor]["id"];

                li.appendChild(a);
                ul.appendChild(li);
            }
        }

        jQuery(buttonGrp).append(ul);
    },

    UpdateCalibrationMenuStatus: function (statusText, type) {
        jQuery("#status-msg").text(statusText).removeClass().addClass("label-" + type);
    },

    /* Networking */
    StartCommunicationWithScene: function (sensorId) {
        return this.networkClient.startCommWorker({ type: "single", sensorId: sensorId }, this.NetworkHandler);
    },

    /* Event Handlers */

    SetupEventHandlers: function () {
        jQuery(".scene-selection > div.btn-group > ul > li").on('click', { callingObj: this }, this.SensorSelectionHandler);
        jQuery(".scene-selection > div > button.dropdown-toggle").on('click', { callingObj: this }, this.RemoveCommWorker);
        jQuery("#training-btn").on('click', { callingObj: this }, this.TrainingBtnHandler);
        jQuery("#send-btn").on('click', { callingObj: this }, this.SendBtnHandler);
    },

    RemoveCommWorker: function (event) {
        var callingObj = event.data.callingObj;
        var span = event.currentTarget;
        var sceneSelectionId = jQuery(span).closest(".scene-selection").attr("id");
        var workerId = callingObj.commWorkersId[sceneSelectionId];

        if (workerId != null) {
            callingObj.networkClient.stopCommWorker(workerId);
            var btn = jQuery(span).closest(".btn-group").children()[0]
            jQuery(btn).removeClass("btn-success");
            jQuery(btn).addClass("btn-danger");
        }
    },

    SensorSelectionHandler: function (event) {
        var callingObj = event.data.callingObj;
        var li = event.currentTarget;
        var text = jQuery(li).text();

        var btn = jQuery(li).closest(".btn-group").children()[0]

        jQuery(btn).text(text);
        jQuery(btn).removeClass("btn-default");
        jQuery(btn).removeClass("btn-danger");
        jQuery(btn).addClass("btn-success");

        // Remember the web worker so that we can shut it down later
        var workerId = callingObj.StartCommunicationWithScene(text);
        var sceneSelectionId = jQuery(li).closest(".scene-selection").attr("id");
        var sceneId = sceneSelectionId.split("-")[2];

        var scenes = jQuery.data(document.body, "scenes");
        scenes[sceneId].sensorId = text;
        jQuery.data(document.body, "scenes", scenes);

        //Set the canvas sensor id status
        jQuery("#canvas-container-" + sceneId + "-scene-status").text(text);

        callingObj.commWorkersId[sceneSelectionId] = workerId;
    },

    NetworkHandler: function (event) {

        var responseType = event.data.type;
        var responseData = event.data.msg;

        if (responseType == "data") {

            var scenes = jQuery.data(document.body, "scenes");

            if (responseData != "") {
                var sceneFromServer = JSON.parse(responseData);

                var sensorId = sceneFromServer["sensorId"];

                for (var sceneNo = 0; sceneNo < scenes.length; sceneNo += 1) {
                    if (scenes[sceneNo].sensorId == sensorId) {
                        scenes[sceneNo].scene = sceneFromServer;
                        jQuery.data(document.body, "scenes", scenes);
                    }
                }
            } else {
                for (var sceneNo = 0; sceneNo < scenes.length; sceneNo += 1) {
                    scenes[sceneNo].scene = null;
                    jQuery.data(document.body, "scenes", scenes);
                }
            }
        }
    },

    TrainingBtnHandler: function (event) {
        var callingObj = event.data.callingObj;
        var trainingBtn = event.currentTarget;

        if (!callingObj.trainingBtnToggle) {
            jQuery(trainingBtn).removeClass("btn-default");
            jQuery(trainingBtn).addClass("btn-warning");
            callingObj.trainingBtnToggle = true;
            callingObj.UpdateCalibrationMenuStatus("Training In Process", "warning");
            localStorage.setItem("trainingMode", "true");

            clearTimeout(callingObj.automaticSkeletonTimeoutVar);

            var lockedSkeletons = new Array();
            localStorage.setItem("lockedSkeletons", JSON.stringify(lockedSkeletons));

            callingObj.automaticSkeletonTimeoutVar = window.setInterval(function () {
                callingObj.AutomaticSceneTrainingHandler(callingObj, trainingBtn);
            }, 1);

        } else {
            jQuery(trainingBtn).addClass("btn-default");
            jQuery(trainingBtn).removeClass("btn-warning");
            callingObj.trainingBtnToggle = false;
            callingObj.UpdateCalibrationMenuStatus("Scene Locking Stopped", "default");
            localStorage.setItem("lockingMode", "false");

            /* Stop the skeletons collection */
            window.clearInterval(callingObj.automaticSkeletonTimeoutVar);
        }
    },

    AutomaticSceneTrainingHandler: function (callingObj, trainingBtn) {

        /* Copy a set of locked scenes from active scenes which are constantly updated */
        var activeScenes = jQuery.data(document.body, "scenes");

        if (activeScenes[0]["scene"] != null) {

            var activeSkeletons = activeScenes[0]["scene"]["skeletons"];

            var lockedSkeletons = JSON.parse(localStorage.getItem("lockedSkeletons"));

            if (activeSkeletons[0].length > 0) {
                if (activeSkeletons[0]["joints"].length >= 20) {

                    lockedSkeletons.push(lockedSkeletons[0]);

                    localStorage.setItem("lockedSkeletons", JSON.stringify(lockedSkeletons));
                }
            }
        }
    },

    SendBtnHandler: function (event) {
        var callingObj = event.data.callingObj;

        callingObj.UpdateCalibrationMenuStatus("Calibration Started", "warning");

        /* Get all the captured skeletons */
        var lockedSkeletons = JSON.parse(localStorage.getItem("lockedSkeletons"));
        var name = jQuery("#subject-name").val();

        if (lockedSkeletons!=null && lockedSkeletons.length > 0 && name != "") {
            if (callingObj.networkClient.calibrateScene(name,lockedSkeletons)) {
                callingObj.UpdateCalibrationMenuStatus("Calibration Succeeded", "success");

                /* Update sensor calibration status on site*/
                sensorId = lockedSkeletons[0]["sensor_id"];

                jQuery("#sensor-info > tbody > tr").each(function () {
                    var id = jQuery(this).attr('id');
                    if (id == sensorId) {
                        jQuery(this.cells[4]).text("true");
                    }
                });

            } else {
                callingObj.UpdateCalibrationMenuStatus("Calibration Failed", "warning");
            }
        }
    },

    /* Scene Reconstruction */

    ReconstructFn: function (glScene, canvasId) {

        // Remove all skeletons previously inserted into scene
        for (var child = 7; child < glScene.children.length; child++) {
            glScene.remove(glScene.children[child]);
        }

        /* Choose the right scene from server to draw on the right canvas */
        var scenesFromServer = jQuery.data(document.body, "scenes");
        var sceneIdFromCanvasId = canvasId.split("-")[2];
        var sceneFromServerToDraw = scenesFromServer[sceneIdFromCanvasId].scene;

        if (sceneFromServerToDraw != null) {
            for (var newSkeleton in sceneFromServerToDraw["skeletons"]) {

                var skeletonObj = new Skeleton(sceneFromServerToDraw["skeletons"][newSkeleton]);
                var skeletonGeometry;

                if (localStorage.getItem("lockingMode") == "true") {
                    skeletonGeometry = skeletonObj.getGeometry(0xFF0000);
                } else {
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