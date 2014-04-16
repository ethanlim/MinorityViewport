#include "Scene.h"

namespace MinorityViewport{

	Scene::Scene(string sensorId,Timer *time)
		:_sensorId(sensorId),
		_dimensionX(0),
		_dimensionY(0),
		_dimensionZ(0),
		_timer(time),
		_refreshThread(new thread(&MinorityViewport::Scene::Clear,this)),
		_firstSkeletonObservedTime_ms(0)
		,_ordering(0),
		_calibrated(false),
		_refreshThreadDie(false)
	{
		this->_refreshRate_ms=3000;
	}

	Scene::Scene(unsigned int dim_x, unsigned int dim_y,unsigned int dim_z,Timer *time)
		:_sensorId(""),
		_dimensionX(dim_x),
		_dimensionY(dim_y),
		_dimensionZ(dim_z),
		_timer(time),
		_refreshThread(NULL),
		_firstSkeletonObservedTime_ms(0)
		,_ordering(0),
		_calibrated(false),
		_refreshThreadDie(false)
	{
		this->_refreshRate_ms=3000;
	}

	Scene::~Scene(){
		this->_refreshThreadDie=true;
		this->_refreshThread->join();
	}

	void Scene::Update(unsigned short serverSkeletonId, Skeleton newPerson){
		
		// Tracked the 1st time sensor observe a skeleton, used for ordering sensors
		if(this->_firstSkeletonObservedTime_ms==0&&this->_skeletons.size()==0){
			this->_firstSkeletonObservedTime_ms = this->_timer->GetTicks_ms();
		}

		this->_sceneMutex.lock();

		this->_skeletons.erase(serverSkeletonId);

		if(this->_skeletons.size()<3){
			this->_skeletons.insert(std::pair<unsigned short,Skeleton>(serverSkeletonId,newPerson));
		}
		
		this->_sceneMutex.unlock();
	}

	void Scene::Remove(unsigned short serverSkeletonId){

		this->_sceneMutex.lock();
		this->_skeletons.erase(serverSkeletonId);
		this->_sceneMutex.unlock();
	}

	void Scene::Clear(){

		long curTime=0;
		long nextLapse=0;

		while(1){
			if(this->_refreshThreadDie){
				break;
			}

			curTime = this->_timer->GetTicks_ms();

			if(this->_timer->GetTicks_ms()>nextLapse){
				
				this->_sceneMutex.lock();

				nextLapse = curTime+this->_refreshRate_ms;

				this->_skeletons.clear();

				this->_sceneMutex.unlock();
			}
		}
	}

	void Scene::ManualClear(){
		this->_sceneMutex.lock();
		this->_skeletons.clear();
		this->_sceneMutex.unlock();
	}

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

	void Scene::SetRAndT(Mat R,Mat T){
		this->RMatrix = R;
		this->TMatrix = T;
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

	Mat Scene::GetRMatrix(ofstream *fileObj,bool writeToFile){

		Mat RotationMatrix = this->RMatrix;

		if(writeToFile){
			RotationMatrix.convertTo(RotationMatrix, CV_64F);
			for(unsigned int row=0;row<RotationMatrix.rows;row+=1){
				*fileObj << RotationMatrix.at<double>(row,0) << ",";
				*fileObj << RotationMatrix.at<double>(row,1) << ",";
				*fileObj << RotationMatrix.at<double>(row,2) ;
				*fileObj << endl;
			}

			fileObj->close();
		}

		return this->RMatrix;
	}
	
	Mat Scene::GetTMatrix(ofstream *fileObj,bool writeToFile){

		Mat TranslationMatrix = this->TMatrix;

		if(writeToFile){
			TranslationMatrix.convertTo(TranslationMatrix, CV_64F);

			*fileObj << TranslationMatrix.at<double>(0,0) << endl;
			*fileObj << TranslationMatrix.at<double>(1,0) << endl;
			*fileObj << TranslationMatrix.at<double>(2,0) << endl;
			*fileObj << endl;

			fileObj->close();
		}

		return this->TMatrix;
	}

	Scene* Scene::GetLeftFrame(){
		return this->left;
	}

	Scene* Scene::GetRightFrame(){
		return this->right;
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