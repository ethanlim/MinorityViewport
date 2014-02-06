
var Scene = function Scene(sceneDOMId) {
    this._sceneDOMId = sceneDOMId;
    this._scene = null;
};

Scene.prototype = {
    UpdateScene: function (scene) {
        delete this._scene;

        this._scene = scene;
    }
};
