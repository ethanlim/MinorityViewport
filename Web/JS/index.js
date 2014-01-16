
$(document).ready(function () {

    var networkLib = Network.init("localhost",1626),
        visualLib = Visualisation.init("canvas-stage-1");

    if (networkLib || visualLib) {
        console.log("Libraries Initialisation Successful");
    }


});