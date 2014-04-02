jQuery(document).ready(function () {

    CalibrationPage.Init();

});

var CalibrationPage = {

    clients: null,
    networkClient: null,
    visualisationCanvas0:null,
    visualisationCanvas1: null,
    commWorkersId: [],

    calibrationMode: false,
    numOfSkeletonsScanned:0,
    numOfSkeletonsRequired: 0,
    numOfJointPoints:0,
    automaticSkeletonTimeoutVar:null,
    lockBtnToggle:false,

    Init:function(){
        
        this.networkClient = Network.initClient(GlobalVar.hostURL,GlobalVar.port);
        this.visualisationCanvas0 = new Visualisation();
        this.visualisationCanvas1 = new Visualisation();

        this.Diagnostic();

        var scenes = new Array(2)
        scenes[0] = { sensorId: null, scene: null};
        scenes[1] = { sensorId: null, scene: null };
        localStorage.setItem("scenes", JSON.stringify(scenes));

        var sceneSkeletons = new Array(2);

        this.numOfSkeletonsRequired = 35;
        this.numOfJointPoints = this.numOfSkeletonsRequired * 21;
        jQuery("#numOfSkeletonsRequired").val(this.numOfSkeletonsRequired);
        jQuery("#numOfJointPts").text(this.numOfJointPoints);
        jQuery("#numOfSkeletonsScanned").text(this.numOfSkeletonsScanned);

        this.clients = this.networkClient.fetchedConnectedClients();

        this.visualisationCanvas0.init({ id: "canvas-container-0", height: "600px", width: "550px" }
                                       ,{ wireFrameColor: 0x3300FF, backgroundColor: 0xB6B6B4, float:'left'}
                                       ,this.ReconstructFn);
        this.visualisationCanvas1.init({ id: "canvas-container-1", height: "600px", width: "550px" }
                                       , { wireFrameColor: 0x006600, backgroundColor: 0xB6B6B4, float: 'left' }
                                       ,this.ReconstructFn);
        this.visualisationCanvas0.render("canvas-container-0");
        this.visualisationCanvas1.render("canvas-container-1");

        localStorage.setItem("lockingMode", "false");

        this.UpdateSensorTable(this.clients);
        this.UpdateSensorDDL(this.clients);
        
        if (this.clients.length == 0) {
            this.UpdateCalibrationMenuStatus("No Client Detected", "danger");
        }else{
            this.UpdateCalibrationMenuStatus("Awaiting Selections", "primary");
        }

        /* Establish the Event Handlers */

        this.SetupEventHandlers();
    },

    Diagnostic:function(){
        if (this.networkClient
           && this.visualisationCanvas0
           && this.visualisationCanvas1
           ) {
            console.log("Libraries Initialisation Successful");
            return true;
        } else {

            if (!this.networkClient) {

                this.networkClient = Network.initClient("localhost", GlobalVar.port);

                console.log("Network Connection Failed : Default to Localhost");
            } else {
                console.log("Visualisation Lib Failed");
            }

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

    UpdateCalibrationMenuStatus: function(statusText,type){
        jQuery("#status-msg").text(statusText).removeClass().addClass("label-"+type);
    },

    /* Networking */
    StartCommunicationWithScene: function(sensorId){
        return this.networkClient.startCommWorker({ type: "single", sensorId: sensorId }, this.NetworkHandler);        
    },

    /* Event Handlers */

    SetupEventHandlers: function () {
        jQuery(".scene-selection > div.btn-group > ul > li").on('click', { callingObj: this }, this.SensorSelectionHandler);
        jQuery(".scene-selection > div > button.dropdown-toggle").on('click', { callingObj: this }, this.RemoveCommWorker);
        jQuery("#lock-btn").on('click', { callingObj: this }, this.LockBtnHandler);
        jQuery("#calibrate-btn").on('click', { callingObj: this }, this.CalibrateBtnHandler);
        jQuery("#numOfSkeletonsRequired").on('change', { callingObj: this },this.SkeletonRequiredHandler);
    },

    RemoveCommWorker:function(event){
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

        var scenes = JSON.parse(localStorage.getItem("scenes"));
        scenes[sceneId].sensorId = text;
        localStorage.setItem("scenes", JSON.stringify(scenes));

        //Set the canvas sensor id status
        jQuery("#canvas-container-" + sceneId + "-scene-status").text(text);

        callingObj.commWorkersId[sceneSelectionId] = workerId;
    },

    SkeletonRequiredHandler: function (event) {
        var callingObj = event.data.callingObj;
        var skeletonsRequiredInput = event.currentTarget;

        if (!isNaN(parseInt(skeletonsRequiredInput.value))) {
            callingObj.numOfSkeletonsRequired = skeletonsRequiredInput.value;
            callingObj.numOfJointPoints = callingObj.numOfSkeletonsRequired * 21;
            jQuery("#numOfJointPts").text(callingObj.numOfJointPoints);
        } else {
            callingObj.UpdateCalibrationMenuStatus("Please enter a number into the Skeleton Required Input", "warning");
        }
    },

    NetworkHandler: function(event){

        var responseType = event.data.type;
        var responseData = event.data.msg;

        if (responseType == "data") {

            var scenes = JSON.parse(localStorage.getItem("scenes"));

            if (responseData != "") {
                var sceneFromServer = JSON.parse(responseData);

                var sensorId = sceneFromServer["sensorId"];
              
                for (var sceneNo = 0; sceneNo < scenes.length; sceneNo += 1) {
                    if (scenes[sceneNo].sensorId == sensorId) {
                        scenes[sceneNo].scene = sceneFromServer;
                        localStorage.setItem("scenes", JSON.stringify(scenes));
                    }
                }

                if (localStorage.getItem("lockingMode") == "true") {
                    if (scenes[0]["scene"] != null && scenes[1]["scene"] != null) {

                        var lockedSkeletonsA = scenes[0]["scene"]["skeletons"];
                        var lockedSkeletonsB = scenes[1]["scene"]["skeletons"];

                        var lockedSkeletons = JSON.parse(localStorage.getItem("lockedSkeletons"));

                        if (lockedSkeletonsA.length > 0 && lockedSkeletonsB.length > 0) {
                            if (lockedSkeletonsA[0]["joints"].length >= 20 && lockedSkeletonsB[0]["joints"].length >= 20) {
                                lockedSkeletons[0].push(lockedSkeletonsA[0]);
                                lockedSkeletons[1].push(lockedSkeletonsB[0]);

                                localStorage.setItem("lockedSkeletons", JSON.stringify(lockedSkeletons));
                            }
                        }
                    }
                }
            } else {
                for (var sceneNo = 0; sceneNo < scenes.length; sceneNo += 1) {
                        scenes[sceneNo].scene = null;
                        localStorage.setItem("scenes", JSON.stringify(scenes));
                }
            }
        }
    },

    LockBtnHandler: function(event){
        var callingObj = event.data.callingObj;
        var lockBtn = event.currentTarget;
        
        if (!callingObj.lockBtnToggle) {
            jQuery(lockBtn).removeClass("btn-default");
            jQuery(lockBtn).addClass("btn-warning");
            callingObj.lockBtnToggle = true;
            callingObj.UpdateCalibrationMenuStatus("Scene Locking In Process","warning");
            localStorage.setItem("lockingMode", "true");

            clearTimeout(callingObj.automaticSkeletonTimeoutVar);

            var lockedSkeletons = new Array(2)
            lockedSkeletons[0] = new Array();
            lockedSkeletons[1] = new Array();
            localStorage.setItem("lockedSkeletons", JSON.stringify(lockedSkeletons));
            callingObj.numOfSkeletonsScanned = 0;
            jQuery("#numOfSkeletonsScanned").text(this.numOfSkeletonsScanned);

            callingObj.automaticSkeletonTimeoutVar = window.setInterval(function () {
                callingObj.AutomaticSceneLockingHandler(callingObj,lockBtn);
            }, 1);  
        } else {
            jQuery(lockBtn).addClass("btn-default");
            jQuery(lockBtn).removeClass("btn-warning");
            callingObj.lockBtnToggle = false;
            callingObj.UpdateCalibrationMenuStatus("Scene Locking Stopped", "default");
            localStorage.setItem("lockingMode", "false");

            /* Stop the skeletons collection */
            window.clearInterval(callingObj.automaticSkeletonTimeoutVar);
        }
    },

    AutomaticSceneLockingHandler: function (callingObj, lockBtn) {

        var lockedSkeletons = JSON.parse(localStorage.getItem("lockedSkeletons"));

        if (lockedSkeletons != null) {
            callingObj.numOfSkeletonsScanned = lockedSkeletons[0].length;
            jQuery("#numOfSkeletonsScanned").text(callingObj.numOfSkeletonsScanned);

            if (callingObj.numOfSkeletonsScanned == callingObj.numOfSkeletonsRequired) {
                callingObj.UpdateCalibrationMenuStatus("Joint Points Collection Completed", "default");
                localStorage.setItem("lockingMode","false")
            }
        }
    },

    CalibrateBtnHandler: function(event){
        var callingObj = event.data.callingObj;
        
        callingObj.UpdateCalibrationMenuStatus("Calibration Started", "warning");

        /* Get all the captured skeletons */
        var lockedSkeletons = JSON.parse(localStorage.getItem("lockedSkeletons"));
        var activeScenes = JSON.parse(localStorage.getItem("scenes"));

        var sceneAOrder = activeScenes[0]["scene"]["ordering"];
        var sceneBOrder = activeScenes[1]["scene"]["ordering"];

        var lockedSkeletonsA = lockedSkeletons[0];
        var lockedSkeletonsB = lockedSkeletons[1];

        if (lockedSkeletonsA.length > 0 && lockedSkeletonsB.length > 0) {

            /* Only send required number of skeletons */
            lockedSkeletonsA.splice(callingObj.numOfSkeletonsRequired,lockedSkeletonsA.length-callingObj.numOfSkeletonsRequired);
            lockedSkeletonsB.splice(callingObj.numOfSkeletonsRequired,lockedSkeletonsB.length-callingObj.numOfSkeletonsRequired);

            if (callingObj.networkClient.calibrateScene(sceneAOrder, lockedSkeletonsA, sceneBOrder, lockedSkeletonsB)) {
                callingObj.UpdateCalibrationMenuStatus("Calibration Succeeded", "success");
                
                /* Update sensor calibration status on site*/
                sensorAId = lockedSkeletonsA[0]["sensor_id"];
                sensorBId = lockedSkeletonsB[0]["sensor_id"];
                jQuery("#sensor-info > tbody > tr").each(function () {
                    var id = jQuery(this).attr('id');
                    if (id == sensorAId) {
                        jQuery(this.cells[4]).text("true");
                    } else if (id == sensorBId) {
                        jQuery(this.cells[4]).text("true");
                    }
                });

            } else {
                callingObj.UpdateCalibrationMenuStatus("Calibration Failed", "warning");
            }
        }
    },
    
    /* Scene Reconstruction */

    ReconstructFn : function(glScene,canvasId){

        // Remove all skeletons previously inserted into scene
        var allGlSceneChildren = glScene.children.length;

        for (var child = 7; child < allGlSceneChildren; child++) {
            glScene.remove(glScene.children[child]);
        }

        /* Choose the right scene from server to draw on the right canvas */
        var scenesFromServer = JSON.parse(localStorage.getItem("scenes"));
        var sceneIdFromCanvasId =  canvasId.split("-")[2];
        var sceneFromServerToDraw = scenesFromServer[sceneIdFromCanvasId].scene;

        if (sceneFromServerToDraw != null) {
            for (var newSkeleton in sceneFromServerToDraw["skeletons"]) {

                var skeletonObj = new Skeleton(sceneFromServerToDraw["skeletons"][newSkeleton]);
                var skeletonGeometry;

                if (localStorage.getItem("lockingMode") == "true") {
                    skeletonGeometry = skeletonObj.getGeometry(0xFF0000);
                } else {
                    skeletonGeometry = skeletonObj.getGeometry(0xFF9933);
                }                

                glScene.add(skeletonGeometry);
            }

            return glScene;
        } else {
            return null;
        }
    }
};