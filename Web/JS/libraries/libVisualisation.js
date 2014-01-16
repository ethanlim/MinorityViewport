

var Visualisation = {
    
    html5CanvasObj: null,
    context2d:null,
    glContext:null,

    /**
    *  Begin constructing key elements for graphics
    *
    */ 
    init: function (canvas_dom_id) {

        this.html5CanvasObj = document.getElementById(canvas_dom_id);

        // Make it visually fill the positioned parent
        this.html5CanvasObj.style.width = '100%';
        this.html5CanvasObj.style.height = "600px";
        // ...then set the internal size to match
        this.html5CanvasObj.width = this.html5CanvasObj.offsetWidth;
        this.html5CanvasObj.height = this.html5CanvasObj.offsetHeight;

        if (this.html5CanvasObj == null) {
            alert("there is no canvas on this page");
            return;
        }

        this.initCanvasObj(this.html5CanvasObj);

        this.glContext = this.getGLContext(this.html5CanvasObj);

        return this;
    },

    /**
    * Draw the canvas object first
    *
    */
    initCanvasObj:function(canvas_obj){

        this.context2d = canvas_obj.getContext('2d');

        this.context2d.fillStyle = "rgb(84,84,84)";

        this.context2d.fillRect(0, 0, this.html5CanvasObj.width, this.html5CanvasObj.height);

    },


    /**
    *  Return null if no GL context
    *  @param null
    *  @return glContext
    */
    getGLContext: function(canvas_obj){
        
        var names =
        ["webgl",
        "experimental-webgl",
        "webkit-3d",
        "moz-webgl"];

        for (var i = 0; i < names.length; ++i) {
            try {
                gl = canvas_obj.getContext(names[i]);
            }
            catch (e) {

                console.log("Gl Init Error : " + e);
            }

            if (gl) break;
        }
        
        return gl;
    },

}