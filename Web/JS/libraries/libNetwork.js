
var Network = {

    serverEndpt: null,
    port: 0,
    clientListing_URL: "/api/clients/listing.json",
    clientCalibration_URL: "/api/visualisations/calibrate.json",
    commWorker:null,

    /**
    * Construct all network components
    * @param server_endpoint
    * @param port
    * @return bool
    */
    initClient: function (server_endpoint,port) {
        this.serverEndpt = "http://"+server_endpoint;
        this.port = port;

        if (this.testConnection()) {
            return false;
        }

        return this;
    },

    /**
    * Test Connection with Server
    * @return bool
    */
    testConnection: function () {
        jQuery.ajax(this.serverEndpt+":"+this.port)
        .done(function () {
            return true;
        })
        .fail(function () {
            return false;
        })
    },

    /**
    *  Fetch Connected Client Listing
    *  @return client_listing_JSON
    */
    fetchedConnectedClients : function(){
        var raw_json = jQuery.ajax({
                                    url: this.serverEndpt + ":" + this.port + this.clientListing_URL,
                                    async:false
        }).responseText;

        var clientObj = JSON.parse(raw_json);

        return clientObj['clients']
    },

    /**
    *  Initial the calibration of clients
    *  @return clients
    */
    calibrateClients:function(){
        var raw_json = jQuery.ajax({
            url: this.serverEndpt + ":" + this.port + this.clientCalibration_URL,
            async: false
        }).responseText;

        var resultObj = JSON.parse(raw_json);

        return resultObj["result"][0];
    },
    
    commError:function(error){
        throw new Error(error.message + " (" + error.filename + ":" + error.lineno + ")");
    },

    startCommWorker: function (cmd,handler) {

        var workerDir = "/js/libraries/workers/comm-worker.js";
        var commPath = "/api/visualisations/data.json";

        this.commWorker = new Worker(workerDir);

        this.commWorker.onerror = this.commError;

        this.commWorker.addEventListener(/*type*/"message",/*handler*/handler,/*useCapture*/true);
        
        this.commWorker.postMessage({
            'cmd'           : 'start',
            'serverEndPt'   : this.serverEndpt,
            'port'          : this.port,
            'fps'           : 1,
            'path'          : commPath,
            'type'          : cmd.type,
            'sensorId'      : cmd.sensorId
        });
    },

    stopCommWorker: function () {
            this.commWorker.postMessage({ 'cmd': 'stop' });
    }
}