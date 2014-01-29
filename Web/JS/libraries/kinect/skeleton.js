
function Skeleton(id){
    this._id = id;

    this._tracked = {
        POSITIONONLY: { value: 0, name: "positionOnly" },
        WITHJOINTS: { value: 1, name: "position&joints" }
    };

    this._X = 0;
    this._Y = 0;
    this._Z = 0;

    this._jointCount = 20;
    this._joints = new Array(this._jointCount);

    this._joints["hand_R"]=null;
    this._joints["hand_L"]=null;
    this._joints["head"] = null;
    this._joints["shoulder_C"] = null;
    this._joints["wrist_R"] = null;
    this._joints["wrist_L"] = null;
    this._joints["elbow_R"] = null;
    this._joints["elbow_L"] = null;
    this._joints["shoulder_R"] = null;
    this._joints["shoulder_L"] = null;
    this._joints["spine"] = null;
    this._joints["hip_R"] = null;
    this._joints["hip_C"] = null;
    this._joints["hip_L"] = null;
    this._joints["knee_R"] = null;
    this._joints["knee_L"] = null;
    this._joints["ankle_R"] = null;
    this._joints["ankle_L"] = null;
    this._joints["foot_R"] = null;
    this._joints["foot_L"] = null;
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
};