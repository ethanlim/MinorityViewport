
var Network = {

    serverEndpt: null,
    port:0,

    /**
    * Construct all network components
    * @param server_endpoint
    * @param port
    * @return bool
    */
    init: function (server_endpoint,port) {
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
    *
    */
    testConnection: function () {
        jQuery.ajax(this.serverEndpt+":"+this.port)
        .done(function () {
            return true;
        })
        .fail(function () {
            return false;
        })
    }

}