#include "Scene.h"

namespace MultipleKinectsPlatformServer{

	Scene::Scene(){

	}

	Scene::~Scene(){

	}

	void Scene::Update(Skeleton newPerson,long timeStamp){

		unsigned short skeletonId = newPerson.GetSkeletonId();

		this->_skeletons.erase(skeletonId);

		this->_skeletons.insert(std::pair<unsigned short,Skeleton>(skeletonId,newPerson));

		this->_timeStamps.insert(std::pair<unsigned short,long>(skeletonId,timeStamp));
	}
}