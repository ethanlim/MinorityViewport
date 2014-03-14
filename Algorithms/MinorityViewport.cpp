#include "MinorityViewport.h"

using namespace std;

namespace MultipleKinectsPlatformServer{

	MinorityViewport::MinorityViewport(Timer *curTime,ClientsList *clients)
		:_curTime(curTime),_clients(clients)
	{
		
		mergingLogFile = new ofstream();

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
										   string skeletonsA_json,
										   unsigned int sceneBOrder,
										   string skeletonsB_json)
	{
		bool calibrateSuccess = false;
		
		Json::Value skeletonsARoot; 
		Json::Value skeletonsBRoot; 
		Json::Reader reader;
		ofstream testOutputSensorAFile("A_calibration_data.txt");
		ofstream testOutputSensorBFile("B_calibration_data.txt");
		ofstream centroidAFile("A_centroid.txt");
		ofstream centroidBFile("B_centroid.txt");
		ofstream translationData("T_calibration_data.txt");
		ofstream rotationData("R_calibration_data.txt");

		if (this->_orderedScenes.size()>0&&reader.parse(skeletonsA_json,skeletonsARoot)&&reader.parse(skeletonsB_json,skeletonsBRoot))
		{
			Json::Value skeletonsA_JSON = skeletonsARoot["skeletons"];
			Json::Value skeletonsB_JSON = skeletonsBRoot["skeletons"];
			list<Skeleton> skeletonsA;
			list<Skeleton> skeletonsB;

			for(unsigned short skeleton=0;skeleton<skeletonsA_JSON.size();skeleton+=1){

				Skeleton skeletonFromSceneA(skeletonsA_JSON[skeleton],0);
				Skeleton skeletonFromSceneB(skeletonsB_JSON[skeleton],0);

				/* Eliminate vectors which are not full sets of joints */
				if(skeletonFromSceneA.checkFullSetOfJoints()&&skeletonFromSceneB.checkFullSetOfJoints()){
					skeletonsA.push_front(skeletonFromSceneA);
					skeletonsB.push_front(skeletonFromSceneB);
				}
			}

			/* Regardless of Scene A or Scene B, Matrix A must be the reference frame which is the lower order */
			Mat A,B;					//nx3
			Mat centroidA,centroidB;	//1x3 (Verified with Matlab)
			unsigned int bodyFrameOrder=0,refFrameOrder=0;

			//scene A is the reference frame
			if(sceneAOrder<sceneBOrder){
				for(list<Skeleton>::iterator skeletonFromA=skeletonsA.begin();skeletonFromA!=skeletonsA.end();++skeletonFromA){
					//nx3 vector matrix
					A.push_back((*skeletonFromA).GetCompletePointsVectorMatrix(NULL,false)); //A
					B.push_back((*skeletonFromA).GetCompletePointsVectorMatrix(NULL,false)); //B
				}

				//1x3
				reduce(A,centroidA,0,1);
				reduce(B,centroidB,0,1);

				/*
				centroidA = skeletonFromSceneA.ComputeCentroid(&centroidAFile,true);
				centroidB = skeletonFromSceneB.ComputeCentroid(&centroidBFile,true);
				*/

				refFrameOrder = sceneAOrder;
				bodyFrameOrder = sceneBOrder;
			}else{
			//scene B is the reference frame
				for(list<Skeleton>::iterator skeletonFromA=skeletonsA.begin();skeletonFromA!=skeletonsA.end();++skeletonFromA){
					//nx3 vector matrix
					A.push_back((*skeletonFromA).GetCompletePointsVectorMatrix(NULL,false)); //A
					B.push_back((*skeletonFromA).GetCompletePointsVectorMatrix(NULL,false)); //B
				}

				//1x3
				reduce(B,centroidA,0,1);
				reduce(A,centroidB,0,1);

				/*
				centroidA = skeletonFromSceneB.ComputeCentroid(&centroidAFile,true);
				centroidB = skeletonFromSceneA.ComputeCentroid(&centroidBFile,true);
				*/

				refFrameOrder = sceneBOrder;
				bodyFrameOrder = sceneAOrder;
			}

			/* Debugging A,B and centroids */

			A.convertTo(A, CV_64F);
			B.convertTo(B, CV_64F);
			for(unsigned int row=0;row<A.rows;row+=1){
				testOutputSensorAFile<<  A.at<double>(row,0) << ",";
				testOutputSensorAFile << A.at<double>(row,1) << ",";
				testOutputSensorAFile << A.at<double>(row,2) ;
				testOutputSensorAFile << endl;

				testOutputSensorBFile<<  B.at<double>(row,0) << ",";
				testOutputSensorBFile << B.at<double>(row,1) << ",";
				testOutputSensorBFile << B.at<double>(row,2) ;
				testOutputSensorBFile << endl;
			}

			testOutputSensorAFile.close();
			testOutputSensorBFile.close();

			centroidA.convertTo(centroidA, CV_64F);
			centroidB.convertTo(centroidB, CV_64F);

			centroidAFile<<	 centroidA.at<double>(0,0) << ",";
			centroidAFile << centroidA.at<double>(0,1) << ",";
			centroidAFile << centroidA.at<double>(0,2) ;

			centroidBFile<<	 centroidB.at<double>(0,0) << ",";
			centroidBFile << centroidB.at<double>(0,1) << ",";
			centroidBFile << centroidB.at<double>(0,2) ;

			centroidAFile.close();
			centroidBFile.close();

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

	void MinorityViewport::ProcessSensorData(string sensorData){
		Json::Value root;   
		Json::Reader reader;

		if (!sensorData.empty()&&reader.parse(sensorData,root))
		{
			long timeStamp = root.get("TIME_STAMP","0").asDouble();
			Json::Value skeletons_JSON = root["SENSOR_JSON"];

			unsigned int numOfSkeletons = skeletons_JSON.size();
			for(unsigned short skeletons=0;skeletons<numOfSkeletons;skeletons++){
				MultipleKinectsPlatformServer::Skeleton newSkeleton(skeletons_JSON[skeletons],timeStamp);
				this->LoadSkeleton(newSkeleton);
			}
		}
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
				this->_globalScene->ManualClear();
				
				this->_orderedSceneMutex.lock();
				vector<Scene*> orderedScenes = this->_orderedScenes;
				this->_orderedSceneMutex.unlock();

				/* Do the comparison with reference frame skeletons and discard skeletons as necessary */
				double threshold = 0.04;
				unsigned int randomSkeletonId;

				if(orderedScenes.size()>0){
					for(vector<Scene*>::reverse_iterator orderedSceneItr = orderedScenes.rbegin();orderedSceneItr!=orderedScenes.rend();orderedSceneItr++){
						if((*orderedSceneItr)->GetCalibration()){

							Scene *leftScenePtr = (*orderedSceneItr)->GetLeftFrame();
							map<unsigned short,Skeleton> currentSceneSkeletons = (*orderedSceneItr)->GetSkeletons();

							if(leftScenePtr==NULL||leftScenePtr->GetSkeletons().size()==0){
								for(map<unsigned short,Skeleton>::iterator	bodyFrameSkeleton=currentSceneSkeletons.begin(); 
																			bodyFrameSkeleton!=currentSceneSkeletons.end();
																			bodyFrameSkeleton++){
									bodyFrameSkeleton->second.UnsetShared();

									randomSkeletonId  = rand()%100;

									this->_globalScene->Update(randomSkeletonId,bodyFrameSkeleton->second);
								}
							}else{
								for(map<unsigned short,Skeleton>::iterator  bodyFrameSkeleton=currentSceneSkeletons.begin();
																			bodyFrameSkeleton!=currentSceneSkeletons.end();
																			bodyFrameSkeleton++){

									/* Transform all the skeletons within body frame to the left frame coordinates*/
									Mat R = (*orderedSceneItr)->GetRMatrix(NULL,false); //3x3
									Mat T = (*orderedSceneItr)->GetTMatrix(NULL,false); //3x1
									Mat translatedSkeletonMatrix = this->TransformSkeletonMatrix(bodyFrameSkeleton->second.GetCompletePointsVectorMatrix(NULL,false),R,T);

									bodyFrameSkeleton->second.ConvertVectorMatrixtoSkeletonPoints(translatedSkeletonMatrix);
									

									map<unsigned short,Skeleton> leftFrameSkeletons = leftScenePtr->GetSkeletons();

									/* Do comparison with the left scene skeletons */
									for(map<unsigned short,Skeleton>::iterator  refFrameSkeleton=leftFrameSkeletons.begin();
																				refFrameSkeleton!=leftFrameSkeletons.end();
																				refFrameSkeleton++){
											/* Comparison */
											if( abs(refFrameSkeleton->second.pos_x-bodyFrameSkeleton->second.pos_x)>threshold||
												abs(refFrameSkeleton->second.pos_y-bodyFrameSkeleton->second.pos_y)>threshold||
												abs(refFrameSkeleton->second.pos_z-bodyFrameSkeleton->second.pos_z)>threshold){
													//Deemed the same human appear in both left and right scene
											   
													//Eliminate the skeleton on the left scene that has the same coordinates as the cur scene skeleton's translated coordinates
													leftScenePtr->Remove(refFrameSkeleton->first);

													randomSkeletonId  = rand()%100;

													bodyFrameSkeleton->second.SetShared();
													this->_globalScene->Update(randomSkeletonId,bodyFrameSkeleton->second);
													break;
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