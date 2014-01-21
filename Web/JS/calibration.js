jQuery(document).ready(function () {

    CalibrationPage.Init();

});

var CalibrationPage = {

    Init:function(){
        var networkLib = Network.init("localhost", 1626);

        if (networkLib) {
            console.log("Libraries Initialisation Successful");
        }

        var clients = networkLib.fetchedConnectedClients();

        this.UpdateSensorTable(clients);

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
                newClientRow.id = client;

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

                    var sensorOrdering = document.createElement("td");
                    sensorOrdering.innerHTML = sensors[sensor]["ordering"];

                    var sensorCalibrated = document.createElement("td");
                    sensorCalibrated.innerHTML = sensors[sensor]["calibrated"];

                    if (sensor==0) {
                        newClientRow.appendChild(sensorOrdering);
                        newClientRow.appendChild(sensorCalibrated);

                        $("#sensor-info").appendChild(newClientRow);
                    } else {
                        var additionalClientRow = document.createElement("tr");

                        var blankCell = document.createElement("td");

                        additionalClientRow.appendChild(blank).appendChild(blank).appendChild(blank);

                        additionalClientRow.appendChild(sensorOrdering);
                        additionalClientRow.appendChild(sensorCalibrated);
                    }
                }

            }
        }
    },

    SetupEventHandlers: function () {
        $("#calibrate-btn").click(this.CalibrateBtnOnClick);
    },

    CalibrateBtnOnClick : function(){
        $("#calibration-progressbar").css("display", "block");
        $("#calibration-progressbar > div.progress-bar").attr("aria-valuenow", 0);
        $("#calibration-progressbar > div.progress-bar").css("width", 0+"%");
        $("#calibration-status").css("display", "block");
        $("#calibration-status").text("Calibrating...");


    }
};