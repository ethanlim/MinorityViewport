
jQuery(document).ready(function () {

    var networkLib = Network.init("localhost",1626),
        visualLib = Visualisation.init("canvas-stage-1");

    if (networkLib || visualLib) {
        console.log("Libraries Initialisation Successful");
    }

    var clients = networkLib.fetchedConnectedClients();

    if (clients.length==0) {
        $("#sensor-info-count").text("0");
    }

});