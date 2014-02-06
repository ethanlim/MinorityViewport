jQuery(document).ready(function () {

    CalibrationPage.Init();

});

var CalibrationPage = {

    clients: null,
    networkClient: null,
    visualisationCanvas0:null,
    visualisationCanvas1: null,
    commWorkersId: [],

    Init:function(){
        
        this.networkClient = Network.initClient("localhost", 1626);
        this.visualisationCanvas0 = new Visualisation();
        this.visualisationCanvas1 = new Visualisation();

        var scenes = new Array(2)
        scenes[0] = { sensorId: null, scene: null };
        scenes[1] = { sensorId: null, scene: null };
        jQuery.data(document.body,"scenes",scenes);

        this.Diagnostic();

        this.clients = this.networkClient.fetchedConnectedClients();

        this.visualisationCanvas0.init({ id: "canvas-container-0", height: "600px", width: "550px" }
                                       ,{ wireFrameColor: 0x3300FF, backgroundColor: 0xB6B6B4 }
                                       ,this.ReconstructFn);
        this.visualisationCanvas1.init({ id: "canvas-container-1", height: "600px", width: "550px" }
                                       ,{ wireFrameColor: 0x006600, backgroundColor: 0xB6B6B4 }
                                       ,this.ReconstructFn);
        this.visualisationCanvas0.render("canvas-container-0");
        this.visualisationCanvas1.render("canvas-container-1");

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
        jQuery("#status-msg").text(statusText).removeClass("label-*").addClass("label-"+type);
    },

    /* Networking */
    StartCommunicationWithScene: function(sensorId){
        return this.networkClient.startCommWorker({ type: "single", sensorId: sensorId }, this.NetworkHandler);        
    },

    /* Event Handlers */

    SetupEventHandlers: function () {
        jQuery(".scene-selection > div.btn-group > ul > li").on('click', { callingObj: this }, this.SensorSelectionHandler);
        jQuery(".scene-selection > div > button.dropdown-toggle").on('click', { callingObj: this }, this.RemoveCommWorker);
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
        var arr = sceneSelectionId.split("-");

        var scenes = jQuery.data(document.body, "scenes");
        sceneId = arr[2];
        scenes[sceneId].sensorId = text;
        jQuery.data(document.body,"scenes",scenes);

        callingObj.commWorkersId[sceneSelectionId] = workerId;
    },

    NetworkHandler: function(event){

        var responseType = event.data.type;
        var responseData = event.data.msg;

        if (responseType == "data") {
            
            var sceneFromServer = JSON.parse(responseData);

            var sensorId = sceneFromServer["sensorId"];
            var scenes = jQuery.data(document.body,"scenes");

            for (var sceneNo = 0;sceneNo<scenes.length; sceneNo += 1) {
                if (scenes[sceneNo].sensorId == sensorId) {
                    scenes[sceneNo].scene = sceneFromServer;
                    jQuery.data(document.body, "scenes", scenes);
                }
            }
        }
    },
    
    /* Scene Reconstruction */

    ReconstructFn : function(scene){

    }
};