jQuery(document).ready(function () {

    OrderingPage.Init();

});

var OrderingPage = {

    clients: null,
    networkClient: null,
    calibrationProgress: 0,

    Init: function () {

        this.networkClient = Network.initClient(GlobalVar.hostURL, GlobalVar.port);

        this.Diagnostic();

        this.clients = this.networkClient.fetchedConnectedClients();

        this.UpdateSensorTable(this.clients);

        /* Establish the Event Handlers */

        this.SetupEventHandlers();
    },

    Diagnostic: function () {
        if (this.networkClient) {
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

    UpdateSensorTable: function (clients) {
        if (clients.length == 0) {
            $("#sensor-info-count").text("0");
            $("#calibrate-btn").css("display", "none");
            $("#calibration-progressbar").css("display", "none");
            $("#calibration-status").css("display", "block");
            $("#calibration-status").text("No Client Detected - Please check client connection to server");
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

    /* Event Handlers */

    SetupEventHandlers: function () {
        $("#calibrate-btn").on('click', { callingObj: this }, this.CalibrateBtnOnClick);
    },

    CalibrateBtnOnClick: function (event) {

        var callingObj = event.data.callingObj;

        if (callingObj.clients.length != 0) {
            this.calibrationProgress = 0;
            $("#calibration-progressbar").css("display", "block");
            $("#calibration-progressbar > div.progress-bar").attr("aria-valuenow", 0);
            $("#calibration-progressbar > div.progress-bar").css("width", 0 + "%");
            $("#calibration-status").css("display", "block");
            $("#calibration-status").text("Calibrating...");

            if (!callingObj.networkClient.calibrateOrder()) {

                $("#calibration-status").text("Calibration Failed - Skeletons not detected");

                return;
            }

            callingObj.calibrationProgress = 10;

            var timerId = setInterval(function () {
                if (callingObj.calibrationProgress > 100) {
                    $("#calibration-status").text("Calibration Completed");
                    clearInterval(timerId);
                    window.location.reload();
                }

                $("#calibration-progressbar > div.progress-bar").attr("aria-valuenow", callingObj.calibrationProgress);
                $("#calibration-progressbar > div.progress-bar").css("width", callingObj.calibrationProgress + "%");

                callingObj.calibrationProgress += 20;
            }, 200);

        } else {
            $("#calibration-progressbar").css("display", "none");
            $("#calibration-status").css("display", "block");
            $("#calibration-status").text("No Client Detected - Please check client connection to server");
        }
    },
};