var Visualisation = {
    
    canvasContainer: null,

    scene: null,
    backgroundColor:0xB6B6B4,
    axes:null,
    camera: null,
    renderer: null,

    controls:null,

    /**
    *  Begin constructing key elements for graphics
    *
    */ 
    init: function (canvas_container_dom_id) {

        this.canvasContainer = document.getElementById(canvas_container_dom_id);

        // Div hack to set the dimension
        this.canvasContainer.style.width = '100%';
        this.canvasContainer.style.height = "600px";
        this.canvasContainer.width = this.canvasContainer.offsetWidth;
        this.canvasContainer.height = this.canvasContainer.offsetHeight;

        this.initCanvasObj(this.canvasContainer);

        return this;
    },

    /**
    *  Initialise the 3d Canvas
    *
    */
    initCanvasObj: function (canvas_container) {

        //Scene
        this.scene = new THREE.Scene();

        //Axes
        this.axes = new THREE.AxisHelper(100);
        this.scene.add(this.axes);

        //Camera
        var SCREEN_WIDTH = canvas_container.width, SCREEN_HEIGHT = canvas_container.height;
        var VIEW_ANGLE = 45, ASPECT = SCREEN_WIDTH/SCREEN_HEIGHT, NEAR = 0.1, FAR = 20000;
        this.camera = new THREE.PerspectiveCamera(VIEW_ANGLE, ASPECT, NEAR, FAR);
        this.camera.position.set(0, 150, 300);
        this.camera.lookAt(this.scene.position);

        //Renderer
        this.renderer = new THREE.WebGLRenderer();
        this.renderer.setSize(canvas_container.width, canvas_container.height);

        //Controls
        this.controls = new THREE.OrbitControls(this.camera,this.renderer.domElement);

        //Lighting
        var light = new THREE.PointLight(0xffffff);
        light.position.set(100, 250, 100);
        this.scene.add(light);

        var skyBoxGeometry = new THREE.CubeGeometry(10000, 10000, 10000);
        var skyBoxMaterial = new THREE.MeshBasicMaterial({ color: this.backgroundColor, side: THREE.BackSide });
        var skyBox = new THREE.Mesh(skyBoxGeometry, skyBoxMaterial);
        this.scene.add(skyBox);

        /* X Plane */
        var gridXZ = new THREE.GridHelper(500, 10);
        gridXZ.setColors(new THREE.Color(0x006600), new THREE.Color(0x006600));
        gridXZ.position.set(0, 0, 0);
        this.scene.add(gridXZ);
        
        //Arrow
        var origin = new THREE.Vector3(0, 0, 0);
        var length = 100;

        var dirX = new THREE.Vector3(1, 0, 0),
            colorX = 0xFF0000;
        var dirY = new THREE.Vector3(0, 1, 0),
            colorY = 0x00FF00;
        var dirZ = new THREE.Vector3(0, 0, 1),
            colorZ = 0x0000FF;

        var arrowX = new THREE.ArrowHelper(dirX, origin, length, colorX);
        this.scene.add(arrowX);
        var arrowY = new THREE.ArrowHelper(dirY, origin, length, colorY);
        this.scene.add(arrowY);
        var arrowZ = new THREE.ArrowHelper(dirZ, origin, length, colorZ);
        this.scene.add(arrowZ);

        canvas_container.appendChild(this.renderer.domElement);
        jQuery.data(document.body, "visualisation", this);
    },

    render: function () {
        var visualisation = jQuery.data(document.body, "visualisation");
        
        requestAnimationFrame(visualisation.render);
        visualisation.renderer.render(visualisation.scene, visualisation.camera);
    }
}