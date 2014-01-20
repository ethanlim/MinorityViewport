jQuery(document).ready(function () {

    var networkLib = Network.init("localhost", 1626);

    if (networkLib) {
        console.log("Libraries Initialisation Successful");
    }

    var clients = networkLib.fetchedConnectedClients();

    if (clients.length == 0) {
        $("#sensor-info-count").text("0");
    } else {

        var numOfClients = clients.length;

        $("#sensor-info-count").text(numOfClients);

        for (var client = 0; client < numOfClients; client += 1) {
            $('#sensor-info tr:last').after(
                                      "<tr>"
                                      + "<td>" + clients[client]["id"] + "</td>"
                                      + "<td>" + clients[client]["location"] + "</td>"
                                      + "<td>" + clients[client]["address"] + "</td>"
                                      + "<td>" + clients[client]["calibrated"] + "</td>"
                                      + "</tr>");
        }
    }

});