#include "MinorityViewport.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	MinorityViewport::MinorityViewport(Timer *curTime,ClientsList *clients)
		:_curTime(curTime),_clients(clients)
	{
		
	}

	MinorityViewport::~MinorityViewport(){

	}

	bool MinorityViewport::Calibrate(){
		
		this->RefreshScenesSet();

		//Probe each scene
		unsigned int numOfCalibratedSceneRequired = this->_scenesSet.size(); 
		unsigned int numOfCalibratedScene = 0;

		for(set<Scene*>::const_iterator itr = this->_scenesSet.begin();itr!=this->_scenesSet.end();itr++){
					
			Scene *scenePtr = *itr;

			long lastSkeletonObserved = scenePtr->GetFirstSkeletonObservedTime_ms();
			
			if(lastSkeletonObserved>0){
				numOfCalibratedScene+=1;
			}
		}

		if(numOfCalibratedScene<numOfCalibratedSceneRequired){
			return false;
		}

		vector<long> sortedTimeStamp;

		for(set<Scene*>::const_iterator itr = this->_scenesSet.begin();itr!=this->_scenesSet.end();itr++){
					
			Scene *scenePtr = *itr;

			scenePtr->SetCalibration(false);
			scenePtr->SetOrdering(0);

			sortedTimeStamp.push_back(scenePtr->GetFirstSkeletonObservedTime_ms());
		}

		sort(sortedTimeStamp.begin(),sortedTimeStamp.end());

		unsigned int order = 0;
		for(vector<long>::const_iterator sortedTimeStampItr = sortedTimeStamp.begin();sortedTimeStampItr!=sortedTimeStamp.end();sortedTimeStampItr++){
			for(set<Scene*>::const_iterator sceneItr = this->_scenesSet.begin();sceneItr!=this->_scenesSet.end();sceneItr++){
				
				Scene *scenePtr = *sceneItr;

				if(scenePtr->GetFirstSkeletonObservedTime_ms()==*sortedTimeStampItr){

					scenePtr->SetCalibration(true);
					scenePtr->SetOrdering(order);
					order+=1;

					this->_orderedScenes.push_back(scenePtr);
				}
			}
		}
		
		return true;
	}

	void MinorityViewport::LoadSkeleton(Skeleton newSkeleton,long timeStamp){

		unsigned int clientId = newSkeleton.GetClientId();
		string sensorId = newSkeleton.GetSensorId();
		
		Client *client = this->_clients->At(clientId);

		Sensor *sensor = client->ExtractSensor(sensorId);

		sensor->UpdateScene(newSkeleton,timeStamp);
	}

	void MinorityViewport::MergeScenes(){

		
	}

	void MinorityViewport::RefreshScenesSet(){

		this->_scenesSet.clear();

		Client *clientPtr;
		
		for(unsigned int client=0;client<this->_clients->Size();client+=1){

			clientPtr = this->_clients->AtIdx(client);
			map<string,Sensor*> sensors = clientPtr->GetSensorsList();

			for(map<string,Sensor*>::iterator itr = sensors.begin();itr!=sensors.end();itr++)
			{
				Scene *scene = itr->second->GetScene();

				this->_scenesSet.insert(scene);
			}
		}
	}

	set<Scene*> MinorityViewport::GetScenesSet(){
		return this->_scenesSet;
	}

}