#include "MinorityViewport.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	MinorityViewport::MinorityViewport(Timer *curTime,ClientsList *clients)
		:_curTime(curTime),_clients(clients)
	{
		//Create the global scene
		this->_globalScene = new Scene(10,10,10,curTime);
		this->_mergethread = new thread(&MultipleKinectsPlatformServer::MinorityViewport::MergeScenes,this);
	}

	MinorityViewport::~MinorityViewport(){
		this->_mergethread->join();
	}

	/**
	 *   CalibrateSceneOrder
	 *   Determine the order of scene based on their last skeleton observed time
	 *   @return - true if scenes have been assigned with order
	 */
	bool MinorityViewport::CalibrateSceneOrder(){
		
		this->RefreshScenesSet();

		this->_orderedSceneMutex.lock();
		this->_orderedScenes.clear();
		this->_orderedSceneMutex.unlock();

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

		//Reset all scenes within the set
		for(set<Scene*>::const_iterator itr = this->_scenesSet.begin();itr!=this->_scenesSet.end();itr++){
					
			Scene *scenePtr = *itr;

			scenePtr->SetOrdering(0);

			sortedTimeStamp.push_back(scenePtr->GetFirstSkeletonObservedTime_ms());
		}

		sort(sortedTimeStamp.begin(),sortedTimeStamp.end());

		unsigned int order = 1;
		for(vector<long>::const_iterator sortedTimeStampItr = sortedTimeStamp.begin();sortedTimeStampItr!=sortedTimeStamp.end();sortedTimeStampItr++){
			for(set<Scene*>::const_iterator sceneItr = this->_scenesSet.begin();sceneItr!=this->_scenesSet.end();sceneItr++){

				Scene *scenePtr = *sceneItr;

				if(scenePtr->GetFirstSkeletonObservedTime_ms()==*sortedTimeStampItr){

					scenePtr->SetOrdering(order);
					order+=1;
					
					this->_orderedSceneMutex.lock();
					this->_orderedScenes.push_back(scenePtr);
					this->_orderedSceneMutex.unlock();
				}
			}
		}

		for(vector<Scene*>::iterator orderedSceneItr = this->_orderedScenes.begin();orderedSceneItr!=this->_orderedScenes.end();orderedSceneItr++)
		{
			Scene *scenePtr = *orderedSceneItr;
			vector<Scene*>::iterator next, prev;

			if(this->_orderedScenes.size()>1){
				if(orderedSceneItr==this->_orderedScenes.begin()){
					next = orderedSceneItr+1;
					scenePtr->SetLeftRightScene(NULL,*next);
				}else if(orderedSceneItr==this->_orderedScenes.end()-1)
				{
					prev = orderedSceneItr-1;
					scenePtr->SetLeftRightScene(*prev,NULL);
				}else
				{
					next = orderedSceneItr+1;
					prev = orderedSceneItr-1;
					scenePtr->SetLeftRightScene(*prev,*next);
				}
			}
		}
		
		return true;
	}

	/** 
	 *   Calibrate the R & T from the two scenes 
	 *   At = RBt+T
	 *   A is the reference frame
	 *   B is the body frame
	 *   R and T is the rotation and translation from B to A
	 *   @return true if computation of R & T matrix is successful
	 */
	bool MinorityViewport::CalibrateScenes(unsigned int sceneAOrder,
										   string skeletonA_json,
										   unsigned int sceneBOrder,
										   string skeletonB_json)
	{
		bool calibrateSuccess = false;
		
		Json::Value skeletonARoot; 
		Json::Value skeletonBRoot; 
		Json::Reader reader;

		if (this->_orderedScenes.size()>0&&reader.parse(skeletonA_json,skeletonARoot)&&reader.parse(skeletonB_json,skeletonBRoot))
		{
			Skeleton skeletonFromSceneA(skeletonARoot.get("skeleton",NULL),0);
			Skeleton skeletonFromSceneB(skeletonBRoot.get("skeleton",NULL),0);

			/* Regardless of Scene A or Scene B, Matrix A must be the reference frame which is the lower order */
			Mat A,B,centroidA,centroidB;
			unsigned int bodyFrameOrder=0,refFrameOrder=0;

			//scene A is the reference frame
			if(sceneAOrder<sceneBOrder){
				//nx3 vector matrix
				A = skeletonFromSceneA.GetCompletePointsVectorMatrix(); //A
				B = skeletonFromSceneB.GetCompletePointsVectorMatrix(); //B

				//3x1 centroid matrix
				centroidA = skeletonFromSceneA.ComputeCentroid();
				centroidB = skeletonFromSceneB.ComputeCentroid();

				refFrameOrder = sceneAOrder;
				bodyFrameOrder = sceneBOrder;
			}else{
			//scene B is the reference frame
				//nx3 vector matrix
				A = skeletonFromSceneB.GetCompletePointsVectorMatrix(); //A
				B = skeletonFromSceneA.GetCompletePointsVectorMatrix(); //B

				//3x1 centroid matrix
				centroidA = skeletonFromSceneB.ComputeCentroid();
				centroidB = skeletonFromSceneA.ComputeCentroid();

				refFrameOrder = sceneBOrder;
				bodyFrameOrder = sceneAOrder;
			}

			//3xn vector matrix
			Mat At;
			transpose(A,At); //A transpose
			Mat Bt;
			transpose(B,Bt);

			/* Construct the H matrix */
			Mat H(3, 3, CV_32F, Scalar(0));
			for(int col=0;col<At.cols;col+=1){
				Mat matrixACol = At.col(col);	//PAi
				Mat matrixBCol = Bt.col(col);	//PBi

				Mat subTotalB;
				subtract(matrixBCol,centroidB,subTotalB);
				Mat subTotalA;
				subtract(matrixACol,centroidA,subTotalA);
				Mat subTotalA_transpose;
				transpose(subTotalA,subTotalA_transpose);

				Mat subTotal = subTotalB*subTotalA_transpose;

				add(H,subTotal,H,noArray(),CV_32F);
			}

			/* Perform SVD on H */
			Mat W,U,Vt,Ut,V;
			SVD::compute(H,W,U,Vt);

			transpose(Vt,V);
			transpose(U,Ut);

			/* Compute the Rotation Matrix (3x3) */
			Mat rotationMatrix=V*Ut;

			/* Compute the Translation Matrix (3x1) */
			Mat translationMatrix;
			Mat negativeRMatrix = -1*rotationMatrix;

			negativeRMatrix.convertTo(negativeRMatrix, CV_32F);
			centroidB.convertTo(centroidB,CV_32F);

			Mat temp = negativeRMatrix * centroidB;
 			add(temp,centroidA,translationMatrix,noArray(),CV_32F);

			/* Assign to the R and T to the B scene */
			Scene *bodyFrameScene = this->_orderedScenes.at(bodyFrameOrder-1);
			bodyFrameScene->SetRotationTranslationMatrix(rotationMatrix,translationMatrix);

			Scene *refFrameScene = this->_orderedScenes.at(refFrameOrder-1);

			bodyFrameScene->SetCalibration(true);
			refFrameScene->SetCalibration(true);
			calibrateSuccess = true;
		}else{
			calibrateSuccess = false;
		}

		return calibrateSuccess;
	}

	void MinorityViewport::LoadSkeleton(Skeleton newSkeleton){

		unsigned int clientId = newSkeleton.GetClientId();
		string sensorId = newSkeleton.GetSensorId();

		Client *client = this->_clients->At(clientId);

		if(client!=NULL){
			Sensor *sensor = client->ExtractSensor(sensorId);

			sensor->UpdateScene(newSkeleton);
		}
	}

	void MinorityViewport::MergeScenes(){
		while(true){
			this->_orderedSceneMutex.lock();
			vector<Scene*> orderedScenes = this->_orderedScenes;
			this->_orderedSceneMutex.unlock();

			if(orderedScenes.size()>0){
				for(vector<Scene*>::iterator orderedSceneItr = orderedScenes.begin();orderedSceneItr!=orderedScenes.end();orderedSceneItr++){
					
					Scene *scenePtr = *orderedSceneItr;

					if(scenePtr->GetCalibration()){

						map<unsigned short,Skeleton> originalSkeletons = scenePtr->GetSkeletons();

						if(orderedSceneItr==orderedScenes.begin()){
							/* Reference frame skeletons need not do any transformation */
							for(map<unsigned short,Skeleton>::iterator firstSceneSkeleton=originalSkeletons.begin(); firstSceneSkeleton!= originalSkeletons.end();firstSceneSkeleton++) {
								this->_globalScene->Update(firstSceneSkeleton->first,firstSceneSkeleton->second);
							}
						}else{
							/* All body frames skeletons need to be transform to reference frame */
							Mat R = scenePtr->GetRMatrix(); //3x3
							Mat T = scenePtr->GetTMatrix(); //3x1
					
							/* Transform all the skeletons within body frame to the reference frame (global scene)*/
							for(map<unsigned short,Skeleton>::iterator bodyFrameSkeleton=originalSkeletons.begin(); bodyFrameSkeleton!= originalSkeletons.end();bodyFrameSkeleton++){
						
								Mat translatedSkeletonMatrix = this->TransformSkeletonMatrix(bodyFrameSkeleton->second.GetCompletePointsVectorMatrix(),R,T);
								
								Mat translatedSkeletonMatrix_transpose;
								transpose(translatedSkeletonMatrix,translatedSkeletonMatrix_transpose); //21x3

								bodyFrameSkeleton->second.ConvertVectorMatrixtoSkeletonPoints(translatedSkeletonMatrix_transpose);

								/* Do the comparison with reference frame skeletons and discard skeletons as necessary */
								map<unsigned short,Skeleton> globalSkeletons = this->_globalScene->GetSkeletons();
								bool samePerson = false;
								for(map<unsigned short,Skeleton>::iterator refFrameSkeleton=globalSkeletons.begin(); refFrameSkeleton!=globalSkeletons.end(); refFrameSkeleton++){

									Mat Areft;
									transpose(refFrameSkeleton->second.GetCompletePointsVectorMatrix(),Areft); //3x21

									SVD svd(translatedSkeletonMatrix);
									Mat pInverse = svd.vt.t()*Mat::diag(1./svd.w)*svd.u.t();	//3x21
									
									Mat leastSquare = pInverse*Areft;

									transpose(leastSquare,leastSquare);

									Mat disparity;
									subtract(Areft,translatedSkeletonMatrix,disparity,noArray(),CV_64F);
								}

								if(samePerson){

								}else{
									//check that skeleton id do not clash
									unsigned short currentSkeletonId = bodyFrameSkeleton->first;
									while(globalSkeletons.find(currentSkeletonId)!=globalSkeletons.end()){
										currentSkeletonId = rand()%100;
									}

									// Add into the global scene
									//this->_globalScene->Update(currentSkeletonId,bodyFrameSkeleton->second);
								}
							}
						}
					}
				}
			}
		}
	}

	Mat MinorityViewport::TransformSkeletonMatrix(Mat bodyFramesSkeleton,Mat R, Mat T){
		Mat B = bodyFramesSkeleton;
		Mat Bt;		
		transpose(B,Bt);	//3x21

		Mat Tt;				
		transpose(T,Tt);
		Mat TResized(Tt);	//1x3
						
		for(int col=0;col<Bt.cols-1;col+=1){
			TResized.push_back(Tt);			//21x3
		}

		transpose(TResized,TResized);	//3x21

		Mat translatedMatrix;	//3x21
		Mat RmulBt;
		RmulBt = R*Bt;		//3x21
		add(RmulBt,TResized,translatedMatrix,noArray(),CV_32F);

		return translatedMatrix;	//3x21
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

	Scene* MinorityViewport::GetGlobalScene(){

		if(this->_orderedScenes.size()!=0){
			return this->_globalScene;
		}else{
			return NULL;
		}
	}

	Scene* MinorityViewport::GetLocalSceneBySensorId(string sensorId){

		if(this->_clients->Size()>0){
		
			Client *clientPtr;

			for(unsigned int client=0;client<this->_clients->Size();client+=1){
			
				clientPtr = this->_clients->AtIdx(client);
				map<string,Sensor*> sensors = clientPtr->GetSensorsList();

				for(map<string,Sensor*>::iterator itr = sensors.begin();itr!=sensors.end();itr++)
				{
					if(itr->first==sensorId){
						return itr->second->GetScene();
					}
				}
			}

		}

		return NULL;
	}

}