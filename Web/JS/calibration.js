jQuery(document).ready(function () {

    CalibrationPage.Init();

});

var CalibrationPage = {

    clients: null,
    networkClient: null,
    visualisationCanvas0:null,
    visualisationCanvas1:null,

    Init:function(){
        
        this.networkClient = Network.initClient("localhost", 1626);
        this.visualisationCanvas0 = new Visualisation();
        this.visualisationCanvas1 = new Visualisation();

        if (    this.networkClient
            && this.visualisationCanvas0
            && this.visualisationCanvas1
            ) {
            console.log("Libraries Initialisation Successful");
        }

        this.clients = this.networkClient.fetchedConnectedClients();

        this.visualisationCanvas0.init({ id: "canvas-container-0", height: "600px", width: "550px" }
                                       ,{ wireFrameColor: 0x3300FF, backgroundColor: 0xB6B6B4 }
                                       ,this.ReconstructFn);
        this.visualisationCanvas1.init({ id: "canvas-container-1", height: "600px", width: "550px" }
                                       ,{ wireFrameColor: 0x006600, backgroundColor: 0xB6B6B4 }
                                       ,this.ReconstructFn);
        this.visualisationCanvas0.render("canvas-container-0");
        this.visualisationCanvas1.render("canvas-container-1");

        this.StartCommunicationWithScenes();

        /* Establish the Event Handlers */

        this.SetupEventHandlers();
    },

    /* Networking */
    StartCommunicationWithScenes: function(){
        if (this.clients != null) {
            for (var client = 0; client < this.clients.length; client += 1) {
                var sensors = this.clients[client]["sensors"];
                for (var sensor = 0; sensor < sensors.length; sensor += 1) {
                    this.networkClient.startCommWorker({ type: "single", sensorId: sensors[sensor]["id"] }, this.NetworkHandler);
                }
            }
        }
    },

    /* Event Handlers */

    SetupEventHandlers: function () {
    },

    NetworkHandler: function(event){
        var responseType = event.data.type;
        var responseData = event.data.msg;

        if (responseType == "data") {

            /* Obtain scene information ("Dimensions","Skeletons") */

            console.log(responseData);
        }
    },

    /* Scene Reconstruction */

    ReconstructFn : function(scene){

    }
};