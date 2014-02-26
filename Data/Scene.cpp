#include "Scene.h"

namespace MultipleKinectsPlatformServer{

	Scene::Scene(string sensorId,Timer *time)
		:_sensorId(sensorId),
		_dimensionX(0),
		_dimensionY(0),
		_dimensionZ(0),
		_curTime(time),
		_refreshRate_ms(30000),
		_refreshThread(new thread(&MultipleKinectsPlatformServer::Scene::Clear,this)),
		_firstSkeletonObservedTime_ms(0)
		,_ordering(0),
		_calibrated(false)
	{
	}

	Scene::Scene(unsigned int dim_x, unsigned int dim_y,unsigned int dim_z,Timer *time)
		:_sensorId(""),
		_dimensionX(dim_x),
		_dimensionY(dim_y),
		_dimensionZ(dim_z),
		_curTime(time),
		_refreshRate_ms(30000),
		_refreshThread(new thread(&MultipleKinectsPlatformServer::Scene::Clear,this)),
		_firstSkeletonObservedTime_ms(0)
		,_ordering(0),
		_calibrated(false)
	{
	}

	Scene::~Scene(){
	}

	void Scene::Update(unsigned short serverSkeletonId, Skeleton newPerson){
		
		// Tracked the 1st time sensor observe a skeleton, used for ordering sensors
		if(this->_firstSkeletonObservedTime_ms==0&&this->_skeletons.size()==0){
			this->_firstSkeletonObservedTime_ms = this->_curTime->GetTicks_ms();
		}

		this->_sceneMutex.lock();

		this->_skeletons.erase(serverSkeletonId);
		this->_skeletons.insert(std::pair<unsigned short,Skeleton>(serverSkeletonId,newPerson));
		
		this->_sceneMutex.unlock();
	}

	void Scene::Clear(){

		long curTime;
		long nextLapse;

		while(1){
			curTime = this->_curTime->GetTicks_ms();
			nextLapse = curTime+this->_refreshRate_ms;

			if(this->_curTime->GetTicks_ms()>nextLapse){

				this->_sceneMutex.lock();

				this->_skeletons.clear();

				this->_sceneMutex.unlock();

				nextLapse = this->_curTime->GetTicks_ms()+this->_refreshRate_ms;
			}
		}
	}

	/**
	 *  GetFirstSkeletonObservedTime_ms
	 *  Get the time when the first skeleton was observed in the scene
	 */ 
	long Scene::GetFirstSkeletonObservedTime_ms(){
		return this->_firstSkeletonObservedTime_ms;
	}

	unsigned int Scene::GetOrdering(){
		return this->_ordering;
	}

	void Scene::SetOrdering(unsigned int order){
		this->_ordering = order;
	}

	bool Scene::GetCalibration(){
		return this->_calibrated;
	}

	void Scene::SetCalibration(bool calibrated){
		this->_calibrated = calibrated;
	}

	void Scene::SetRotationTranslationMatrix(Mat R,Mat T){
		this->RotationMatrix = R;
		this->TranslationMatrix = T;
	}

	void Scene::SetDimensions(unsigned int x, unsigned int y, unsigned int z){
		this->_dimensionX = x;
		this->_dimensionY = y;
		this->_dimensionZ = z;
	}

	void Scene::SetLeftRightScene(Scene *left, Scene *right){
		this->left = left;
		this->right = right;
	}

	void Scene::SetCalibrationSkeleton(Skeleton *calibrationSkeleton){
		this->calibrationSkeleton = calibrationSkeleton;
	}

	Mat Scene::GetRMatrix(){
		return this->RotationMatrix;
	}
	
	Mat Scene::GetTMatrix(){
		return this->TranslationMatrix;
	}

	map<unsigned short,Skeleton> Scene::GetSkeletons(){

		map<unsigned short,Skeleton> tempSkeletonsHolder;

		this->_sceneMutex.lock();

		tempSkeletonsHolder = this->_skeletons;

		this->_sceneMutex.unlock();

		return tempSkeletonsHolder;
	}

	string Scene::ToJSON(){
		string json;

		json+="{";

		json+="\"sensorId\":";
		string sensorId = this->_sensorId;
		size_t i = sensorId.find('\\');
		while (i != string::npos)
		{
			string part1 = sensorId.substr(0, i);
			string part2 = sensorId.substr(i + 1);
			sensorId = part1 + "\\\\" + part2;
			i = sensorId.find('\\', i + 4);
		}
		json+="\"" + sensorId + "\"";
		json+=",";

		json+="\"dimensionX\":";
		json+=to_string(this->_dimensionX);
		json+=",";

		json+="\"dimensionY\":";
		json+=to_string(this->_dimensionY);
		json+=",";

		json+="\"dimensionZ\":";
		json+=to_string(this->_dimensionZ);
		json+=",";

		json+="\"ordering\":";
		json+=to_string(this->_ordering);
		json+=",";

		json+="\"calibrated\":";
		json+=to_string(this->_calibrated);
		json+=",";
		
		map<unsigned short,Skeleton> skeletons = this->GetSkeletons();
		json+="\"skeletons\":";
		json+="[";
		if(skeletons.size()>0){
			map<unsigned short,Skeleton>::iterator lastSkeleton = skeletons.end();
			std::advance(lastSkeleton,-1);
			for(map<unsigned short,Skeleton>::iterator itr = skeletons.begin();itr!=skeletons.end();itr++)
			{
				json += itr->second.ToJSON();

				if(itr!=lastSkeleton)
				{
					json += ",";
				}
			}
		}
		json+="]";

		json += "}";
		
		return json;
	}
}