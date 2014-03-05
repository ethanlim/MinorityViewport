#include "MinorityViewport.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	MinorityViewport::MinorityViewport(Timer *curTime,ClientsList *clients)
		:_curTime(curTime),_clients(clients)
	{
		
		mergingLogFile = new ofstream();

		//Create the global scene
		this->_globalScene = new Scene(10,10,10,curTime);
		this->_mergethread = NULL; /*new thread(&MultipleKinectsPlatformServer::MinorityViewport::MergeScenes,this);*/

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
					//First Scene
					next = orderedSceneItr+1;
					scenePtr->SetLeftRightScene(NULL,*next);
				}else if(orderedSceneItr==this->_orderedScenes.end()-1)
				{
					//Last Scene
					prev = orderedSceneItr-1;
					scenePtr->SetLeftRightScene(*prev,NULL);
				}else
				{
					//i scene i!=0 && i!=n
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
		ofstream testOutputSensorAFile("A_calibration_data.txt");
		ofstream testOutputSensorBFile("B_calibration_data.txt");
		ofstream centroidAFile("A_centroid.txt");
		ofstream centroidBFile("B_centroid.txt");
		ofstream translationData("T_calibration_data.txt");
		ofstream rotationData("R_calibration_data.txt");

		if (this->_orderedScenes.size()>0&&reader.parse(skeletonA_json,skeletonARoot)&&reader.parse(skeletonB_json,skeletonBRoot))
		{
			Skeleton skeletonFromSceneA(skeletonARoot.get("skeleton",NULL),0);
			Skeleton skeletonFromSceneB(skeletonBRoot.get("skeleton",NULL),0);

			if(!skeletonFromSceneA.checkFullSetOfJoints()||!skeletonFromSceneB.checkFullSetOfJoints()){
				return false;
			}

			/* Regardless of Scene A or Scene B, Matrix A must be the reference frame which is the lower order */
			Mat A,B;					//nx3
			Mat centroidA,centroidB;	//1x3 (Verified with Matlab)
			unsigned int bodyFrameOrder=0,refFrameOrder=0;

			//scene A is the reference frame
			if(sceneAOrder<sceneBOrder){
				//nx3 vector matrix
				A = skeletonFromSceneA.GetCompletePointsVectorMatrix(&testOutputSensorAFile,true); //A
				B = skeletonFromSceneB.GetCompletePointsVectorMatrix(&testOutputSensorBFile,true); //B

				//1x3
				centroidA = skeletonFromSceneA.ComputeCentroid(&centroidAFile,true);
				centroidB = skeletonFromSceneB.ComputeCentroid(&centroidBFile,true);

				refFrameOrder = sceneAOrder;
				bodyFrameOrder = sceneBOrder;
			}else{
			//scene B is the reference frame
				//nx3 vector matrix
				A = skeletonFromSceneB.GetCompletePointsVectorMatrix(&testOutputSensorAFile,true); //A
				B = skeletonFromSceneA.GetCompletePointsVectorMatrix(&testOutputSensorBFile,true); //B

				//1x3
				centroidA = skeletonFromSceneB.ComputeCentroid(&centroidAFile,true);
				centroidB = skeletonFromSceneA.ComputeCentroid(&centroidBFile,true);

				refFrameOrder = sceneBOrder;
				bodyFrameOrder = sceneAOrder;
			}

			/* Construct the H matrix */
			Mat centroidA_row = centroidA;
			Mat centroidB_row = centroidB;

			for(int row=0;row<A.rows-1;row++){
				centroidA.push_back(centroidA_row);
				centroidB.push_back(centroidB_row);
			}

			// H = (B - repmat(centroid_B, N, 1))'*(A - repmat(centroid_A, N, 1));
			Mat firstOperand;
			subtract(B,centroidB,firstOperand,noArray(),CV_32F);
			transpose(firstOperand,firstOperand);

			Mat secondOperand;
			subtract(A,centroidA,secondOperand,noArray(),CV_32F);

			Mat H = firstOperand*secondOperand;

			/* Perform SVD on H */
			Mat U,S,Vt,Ut,V;
			SVD::compute(H,S,U,Vt);

			transpose(U,Ut);
			transpose(Vt,V);

			/* Compute the Rotation Matrix (3x3) (Verified with Matlab) */
			Mat rotationMatrix=V*Ut;

			/* Compute the Translation Matrix (3x1) */
			Mat translationMatrix;
			centroidB_row.convertTo(centroidB_row,CV_32F);
			rotationMatrix.convertTo(rotationMatrix,CV_32F);
			Mat centroidB_transpose;
			transpose(centroidB_row,centroidB_transpose);
			Mat temp = (-1*rotationMatrix)*centroidB_transpose;

			Mat centroidA_transpose;
			transpose(centroidA_row,centroidA_transpose);

 			add(temp,centroidA_transpose,translationMatrix,noArray(),CV_32F);

			/* Assign to the R and T to the B scene */
			Scene *bodyFrameScene = this->_orderedScenes.at(bodyFrameOrder-1);
			bodyFrameScene->SetRAndT(rotationMatrix,translationMatrix);

			/* Output to file for testing */
			bodyFrameScene->GetRMatrix(&rotationData,true);
			bodyFrameScene->GetTMatrix(&translationData,true);

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
			/* Do the comparison with reference frame skeletons and discard skeletons as necessary */
			double threshold = 0.04;
			this->_globalScene->ManualClear();
			map<unsigned short,Skeleton> globalSkeletons; 

			this->_orderedSceneMutex.lock();
			vector<Scene*> orderedScenes = this->_orderedScenes;
			this->_orderedSceneMutex.unlock();

			if(orderedScenes.size()>0){ //As we move down the ordered scene
				
				for(vector<Scene*>::iterator orderedSceneItr = orderedScenes.begin();orderedSceneItr!=orderedScenes.end();orderedSceneItr++){
					
					globalSkeletons = this->_globalScene->GetSkeletons();
					Scene *scenePtr = *orderedSceneItr;

					if(scenePtr->GetCalibration()){

						map<unsigned short,Skeleton> originalSkeletons = scenePtr->GetSkeletons();

						if(orderedSceneItr==orderedScenes.begin()){

							map<unsigned short,Skeleton> rightFrameSkeletons = scenePtr->GetRightFrame()->GetSkeletons();

							if(rightFrameSkeletons.size()==0){
								//Right Frame can no longer see the skeletons

								for(map<unsigned short,Skeleton>::iterator firstFrameSkeleton=originalSkeletons.begin(); firstFrameSkeleton!= originalSkeletons.end();firstFrameSkeleton++){
									firstFrameSkeleton->second.UnsetShared();
									this->_globalScene->Update(firstFrameSkeleton->first,firstFrameSkeleton->second);
								}
							}
						}else{
								map<unsigned short,Skeleton> leftFrameSkeletons = scenePtr->GetLeftFrame()->GetSkeletons();
							
								if(leftFrameSkeletons.size()==0){
									//Left frame can no longer see the skeleton

									for(map<unsigned short,Skeleton>::iterator bodyFrameSkeleton=originalSkeletons.begin(); bodyFrameSkeleton!= originalSkeletons.end();bodyFrameSkeleton++){
										bodyFrameSkeleton->second.UnsetShared();

										//check that skeleton id do not clash
										unsigned short currentSkeletonId = bodyFrameSkeleton->first;
										while(globalSkeletons.find(currentSkeletonId)!=globalSkeletons.end()){
											currentSkeletonId = rand()%100;
										}
										this->_globalScene->Update(currentSkeletonId,bodyFrameSkeleton->second);
									}
								}else{
									/* In between two sensors */
									for(map<unsigned short,Skeleton>::iterator bodyFrameSkeleton=originalSkeletons.begin(); bodyFrameSkeleton!= originalSkeletons.end();bodyFrameSkeleton++){

										/* All body frames skeletons need to be transform to left frame coordinates */
										Mat R = scenePtr->GetRMatrix(NULL,false); //3x3
										Mat T = scenePtr->GetTMatrix(NULL,false); //3x1

										/* Transform all the skeletons within body frame to the left frame coordinates*/
										Mat translatedSkeletonMatrix = this->TransformSkeletonMatrix(bodyFrameSkeleton->second.GetCompletePointsVectorMatrix(NULL,false),R,T);

										bodyFrameSkeleton->second.ConvertVectorMatrixtoSkeletonPoints(translatedSkeletonMatrix);
								
										for(map<unsigned short,Skeleton>::iterator refFrameSkeleton=leftFrameSkeletons.begin();
											refFrameSkeleton!=leftFrameSkeletons.end();
											refFrameSkeleton++){

												/* Comparison */
												if(abs(refFrameSkeleton->second.pos_x-bodyFrameSkeleton->second.pos_x)>threshold||
													abs(refFrameSkeleton->second.pos_y-bodyFrameSkeleton->second.pos_y)>threshold||
													abs(refFrameSkeleton->second.pos_z-bodyFrameSkeleton->second.pos_z)>threshold){
														//human appear on left scene as well
											   
														bodyFrameSkeleton->second.SetShared();

														this->_globalScene->Update(refFrameSkeleton->second.GetSkeletonId(),bodyFrameSkeleton->second);
												}
										}
									}
								}
						}
					}
				}
			}
	}


	/**
	 *	Implement the formulat 
	 *	A (3xn) = R(3x3)*B(3xn)+T(3xn)
	 */
	Mat MinorityViewport::TransformSkeletonMatrix(Mat bodyFramesSkeleton,Mat R, Mat T)
	{
		Mat Bt;		
		transpose(bodyFramesSkeleton,Bt);							//3x21

		Mat Tt;				
		transpose(T,Tt);

		Mat TResized(Tt);											//1x3
		for(int col=0;col<Bt.cols-1;col+=1){
			TResized.push_back(Tt);									//21x3
		}
		transpose(TResized,TResized);								//3x21
					
		Mat RmulBt;
		RmulBt = R*Bt;												//3x21
		Mat transformedMatrix;										//3x21
		add(RmulBt,TResized,transformedMatrix,noArray(),CV_32F);
		transpose(transformedMatrix,transformedMatrix);

		return transformedMatrix;									//21x3
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

			this->MergeScenes();
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