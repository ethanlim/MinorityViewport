
jQuery(document).ready(function () {

    var networkLib = Network.init("localhost",1626),
        visualLib = Visualisation.init();

    if (networkLib || visualLib) {
        console.log("Libraries Initialisation Successful");
    }

});