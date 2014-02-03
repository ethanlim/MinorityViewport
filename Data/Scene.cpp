#include "Scene.h"

namespace MultipleKinectsPlatformServer{

	Scene::Scene(Timer *time)
		:_curTime(time),_refreshRate_ms(1000),_firstSkeletonObservedTime_ms(0),_ordering(0),_calibrated(false)
	{
		this->refreshThread = new thread(&MultipleKinectsPlatformServer::Scene::Clear,this);
	}

	Scene::~Scene(){
	}

	void Scene::Update(Skeleton newPerson){
		
		unsigned short skeletonId = newPerson.GetSkeletonId();

		if(this->_firstSkeletonObservedTime_ms==0&&this->_skeletons.size()==0){
			this->_firstSkeletonObservedTime_ms = this->_curTime->GetTicks_ms();
		}

		this->_sceneMutex.lock();

		this->_skeletons.erase(skeletonId);

		this->_skeletons.insert(std::pair<unsigned short,Skeleton>(skeletonId,newPerson));
	
		this->_sceneMutex.unlock();
	}

	void Scene::Clear(){

		long nextLapse = this->_curTime->GetTicks_ms()+this->_refreshRate_ms;

		while(1){
			if(this->_curTime->GetTicks_ms()>nextLapse){

				this->_sceneMutex.lock();

				this->_skeletons.clear();

				this->_sceneMutex.unlock();

				nextLapse = this->_curTime->GetTicks_ms()+this->_refreshRate_ms;
			}
		}
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

		json+="\"ordering\":";
		json+=to_string(this->_ordering);
		json+=",";

		json="\"calibrated\":";
		json+=to_string(this->_calibrated);
		json+=",";
		
		map<unsigned short,Skeleton> skeletons = this->GetSkeletons();
		json+="\"skeleton\":";
		json+="[";
		map<unsigned short,Skeleton>::iterator lastSkeleton = skeletons.end();
		std::advance(lastSkeleton,-1);
		for(map<unsigned short,Skeleton>::iterator itr = skeletons.begin();itr!=skeletons.end();itr++)
		{
			json += "{";
			json += itr->second.ToJSON();
			json += "}";

			if(itr!=lastSkeleton)
			{
				json += ",";
			}
		}
		json+="]";

		json += "}";
		
		return json;
	}
}