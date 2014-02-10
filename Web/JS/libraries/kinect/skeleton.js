


function Skeleton(skeleton_JSON){
    this._id = "";

    this._tracked = {
        POSITIONONLY: { value: 0, name: "positionOnly" },
        WITHJOINTS: { value: 1, name: "position&joints" }
    };

    this._X = skeleton_JSON.pos_x;
    this._Y = skeleton_JSON.pos_y;
    this._Z = skeleton_JSON.pos_z;

    this._jointCount = 20;
    this._joints = new Array(this._jointCount);

    this._joints["ankle_L"] = skeleton_JSON.joints[0];
    this._joints["ankle_R"] = skeleton_JSON.joints[1];
    this._joints["elbow_L"] = skeleton_JSON.joints[2];
    this._joints["elbow_R"] = skeleton_JSON.joints[3];
    this._joints["foot_L"] = skeleton_JSON.joints[4];
    this._joints["foot_R"] = skeleton_JSON.joints[5];
    this._joints["hand_L"] = skeleton_JSON.joints[6];
    this._joints["hand_R"] = skeleton_JSON.joints[7];
    this._joints["head"] = skeleton_JSON.joints[8];
    this._joints["hip_C"] = skeleton_JSON.joints[9];
    this._joints["hip_L"] = skeleton_JSON.joints[10];
    this._joints["hip_R"] = skeleton_JSON.joints[11];
    this._joints["knee_L"] = skeleton_JSON.joints[12];
    this._joints["knee_R"] = skeleton_JSON.joints[13];
    this._joints["shoulder_C"] = skeleton_JSON.joints[14];
    this._joints["shoulder_L"] = skeleton_JSON.joints[15];
    this._joints["shoulder_R"] = skeleton_JSON.joints[16];
    this._joints["spine"] = skeleton_JSON.joints[17];
    this._joints["wrist_L"] = skeleton_JSON.joints[18];
    this._joints["wrist_R"] = skeleton_JSON.joints[19];
}

Skeleton.prototype = {
    
    getId: function(){
        return this._id;
    },

    /**
    * Get the array of joints
    */
    getJoints: function () {
        return this._joints;
    },

    getJointTypes:function(){
        return Object.keys(this._joints);
    },

    checkJointTypeExist:function(typeName){

        var jointTypes = this.getJointTypes();

        for (var i = 0; i < this._jointCount; i += 1) {
            if (jointTypes[i] == typeName) {
                return true;
            }
        }

        return false;
    },

    setJoints: function (jointType,jointObj) {

        if (this.checkJointTypeExist(jointType)) {

            this._joints[jointType].X = jointObj.X;
            this._joints[jointType].Y = jointObj.Y;
            this._joints[jointType].Z = jointObj.Z;

            return true;
        } else {
            return false;
        }

    },

    getGeometry: function () {

        /* Requires THREE.js library */

        for (var joint = 0; joint < skeletonObj.joints.length; joint += 1) {

            var jointSphere = new THREE.SphereGeometry(10, 100, 100);
            var jointMaterial = new THREE.MeshPhongMaterial({ color: 0xffff00 });

            var jointGeometry = new THREE.Mesh(jointSphere, jointMaterial);

            jointGeometry.position.x = skeletonObj.joints[joint].X;
            jointGeometry.position.x = skeletonObj.joints[joint].Y;
            jointGeometry.position.x = skeletonObj.joints[joint].Z;
        }
    }
};