

var Visualisation = {
    
    glContext:null,

    /**
    *  Begin constructing key elements for graphics
    *
    */ 
    init: function () {
        this.glContext = this.getGLContext();

        return this;
    },

    /**
    *  Return null if no GL context
    *  @param null
    *  @return glContext
    */
    getGLContext: function(){
        
        var html5_canvas = document.getElementById("canvas-stage-1");

        if (html5_canvas == null) {
            alert("there is no canvas on this page");
            return;
        }

        var names =
        ["webgl",
        "experimental-webgl",
        "webkit-3d",
        "moz-webgl"];

        for (var i = 0; i < names.length; ++i) {
            try {
                gl = html5_canvas.getContext(names[i]);
            }
            catch (e) {

                console.log("Gl Init Error : " + e);
            }

            if (gl) break;
        }
        
        return gl;
    },

}