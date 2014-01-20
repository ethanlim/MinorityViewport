
var Network = {

    serverEndpt: null,
    port: 0,
    clientListing_URL:"/api/clients/listing.json",

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
    }

}