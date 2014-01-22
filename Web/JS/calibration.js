jQuery(document).ready(function () {

    CalibrationPage.Init();

});

var CalibrationPage = {

    clients: null,
    networkClient:null,

    Init:function(){
        
        this.networkClient = Network.initClient("localhost", 1626);

        if (this.networkClient) {
            console.log("Libraries Initialisation Successful");
        }

        this.clients = this.networkClient.fetchedConnectedClients();

        this.UpdateSensorTable(this.clients);

        /* Establish the Event Handlers */

        this.SetupEventHandlers();
    },

    UpdateSensorTable: function (clients) {
        if (clients.length == 0) {
            $("#sensor-info-count").text("0");
        } else {
            var numOfClients = clients.length;

            $("#sensor-info-count").text(numOfClients);

            for (var client = 0; client < numOfClients; client += 1) {

                var newClientRow = document.createElement("tr");
                newClientRow.id = clients[client]["id"];

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

                for (var sensor = 0; sensor < numOfSensors; sensor += 1) {

                    var sensorId = document.createElement("td");
                    sensorId.innerHTML = sensors[sensor]["id"];

                    var sensorOrdering = document.createElement("td");
                    sensorOrdering.innerHTML = sensors[sensor]["ordering"];

                    var sensorCalibrated = document.createElement("td");
                    sensorCalibrated.innerHTML = sensors[sensor]["calibrated"];

                    if (sensor == 0) {
                        newClientRow.appendChild(sensorId);
                        newClientRow.appendChild(sensorCalibrated);
                        newClientRow.appendChild(sensorOrdering);
                        
                        $("#sensor-info tr:last").after(newClientRow.outerHTML);
                    } else {
                        var additionalClientRow = document.createElement("tr");

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

                        $("#sensor-info tr:last").after(additionalClientRow.outerHTML);
                    }
                }

            }
        }
    },

    SetupEventHandlers: function () {
        $("#calibrate-btn").on('click', {callingObj:this},this.CalibrateBtnOnClick);
    },

    CalibrateBtnOnClick: function (event) {

        var callingObj = event.data.callingObj;

        if (callingObj.clients.length!=0) {

            $("#calibration-progressbar").css("display", "block");
            $("#calibration-progressbar > div.progress-bar").attr("aria-valuenow", 0);
            $("#calibration-progressbar > div.progress-bar").css("width", 0 + "%");
            $("#calibration-status").css("display", "block");
            $("#calibration-status").text("Calibrating...");

            var amtOfDelay_s = callingObj.clients.length * 5;

            callingObj.networkClient.calibrateClients({cmd:"new"});

            setTimeout(callingObj.networkClient.calibrateClients, amtOfDelay_s, { cmd: "check" });

        } else {
            $("#calibration-progressbar").css("display", "none");
            $("#calibration-status").css("display", "block");
            $("#calibration-status").text("No Client Detected - Please check client connection to server");
        }

    }
};