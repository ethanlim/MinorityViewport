#include "MinorityViewportAlgo.h"

using namespace std;

namespace MinorityViewport{

	MinorityViewportAlgo::MinorityViewportAlgo(Timer *curTime,ClientsList *clients)
		:_curTime(curTime),_clients(clients)
	{
		
		mergingLogFile = new ofstream("mergingLogFile.txt");

		//Create the global scene
		this->_globalScene = new Scene(10,10,10,curTime);
		//this->_mergethread = new thread(&MinorityViewportAlgo::MinorityViewportAlgo::MergeScenes,this);
	}

	MinorityViewportAlgo::~MinorityViewportAlgo(){
		this->_mergethread->join();
		this->mergingLogFile->close();
	}

	unsigned int MinorityViewportAlgo::RegisterClient(string phyLocation, string ipAddr){
		
		unsigned int clientId = this->_clients->AddClient(phyLocation,ipAddr);

		this->ReportStatus("client id assigned - " + std::to_string(clientId));

		return clientId;
	}

	string MinorityViewportAlgo::GetClientListing(){

		string clientListing="";

		clientListing += "{";
		clientListing += "\"clients\":";
		clientListing += "[";

		for(unsigned int clientIdx=0;clientIdx<this->_clients->Size();clientIdx++){
			MinorityViewport::Client *extractedClient = this->_clients->AtIdx(clientIdx);
			  
			clientListing += extractedClient->ToJSON();

			if(clientIdx!=this->_clients->Size()-1){
				clientListing+=",";
			}
		}
		

		clientListing += "]";
		clientListing += "}";

		return clientListing;
	}

	void MinorityViewportAlgo::DeregisterClient(unsigned int clientId){
		this->ReportStatus("client id deregistered - " + std::to_string(clientId));

		Client *clientToBeRm = this->_clients->At(clientId);

		map<string,Sensor*> allSensorsForClient = clientToBeRm->GetSensorsList();

		this->_orderedSceneMutex.lock();

		for(map<string,Sensor*>::iterator	sensorToBeRm=allSensorsForClient.begin(); 
											sensorToBeRm!=allSensorsForClient.end();
											sensorToBeRm++){
				
				Scene *scenePtr = sensorToBeRm->second->GetScene();

				for(vector<Scene*>::iterator orderedSceneItr = this->_orderedScenes.begin();orderedSceneItr!=this->_orderedScenes.end();orderedSceneItr++){
					if(*orderedSceneItr==scenePtr){
						this->_orderedScenes.erase(orderedSceneItr);
					}
				}
				for(set<Scene*>::iterator sceneItr = this->_scenesSet.begin();sceneItr!=this->_scenesSet.end();sceneItr++){
					if(*sceneItr==scenePtr){
						this->_scenesSet.erase(sceneItr);
					}
				}
		}
		
		this->_orderedSceneMutex.unlock();

		this->_clients->RemoveClient(clientId);
	}

	void MinorityViewportAlgo::RegisterSensors(unsigned int clientId,string rawSensorsList){
		MinorityViewport::Client *extractedClient = this->_clients->At(clientId);

		extractedClient->InitialSensorsList(rawSensorsList);
	}

	/**
	 *   CalibrateSceneOrder
	 *   Determine the order of scene based on their last skeleton observed time
	 *   @return - true if scenes have been assigned with order
	 */
	bool MinorityViewportAlgo::CalibrateSceneOrder(){
		
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
		
		this->_scenesSet.clear();

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
	bool MinorityViewportAlgo::CalibrateScenes(unsigned int sceneAOrder,
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
		ofstream translationDataFile("T_calibration_data.txt");
		ofstream rotationDataFile("R_calibration_data.txt");

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

			/* Regardless of Scene A or Scene B, Matrix A must be the reference frame here*/
			Mat A,B;					//nx3
			Mat centroidA,centroidB;	//1x3 (Verified with Matlab)
			unsigned int bodyFrameOrder=0,refFrameOrder=0;

			// A must always be the reference frame and B the body frame after this if condition
			if(sceneAOrder<sceneBOrder){
			//scene A is the reference frame
				for(list<Skeleton>::iterator skeletonFromA=skeletonsA.begin();skeletonFromA!=skeletonsA.end();++skeletonFromA){
					//nx3 vector matrix
					A.push_back((*skeletonFromA).GetCompletePointsVectorMatrix(NULL,false)); //A
				}

				for(list<Skeleton>::iterator skeletonFromB=skeletonsB.begin();skeletonFromB!=skeletonsB.end();++skeletonFromB){
					//nx3 vector matrix
					B.push_back((*skeletonFromB).GetCompletePointsVectorMatrix(NULL,false)); //B
				}

				refFrameOrder	= sceneAOrder;
				bodyFrameOrder	= sceneBOrder;
			}else{
			//scene B is the reference frame
				for(list<Skeleton>::iterator skeletonFromB=skeletonsB.begin();skeletonFromB!=skeletonsB.end();++skeletonFromB){
					//nx3 vector matrix
					A.push_back((*skeletonFromB).GetCompletePointsVectorMatrix(NULL,false)); //A
				}

				for(list<Skeleton>::iterator skeletonFromA=skeletonsA.begin();skeletonFromA!=skeletonsA.end();++skeletonFromA){
					//nx3 vector matrix
					B.push_back((*skeletonFromA).GetCompletePointsVectorMatrix(NULL,false)); //B
				}

				refFrameOrder	= sceneBOrder;
				bodyFrameOrder	= sceneAOrder;
			}

			//1x3
			reduce(A,centroidA,0,1);
			reduce(B,centroidB,0,1);

			/* Log A,B and centroids */

			A.convertTo(A, CV_64F);
			B.convertTo(B, CV_64F);
			for(unsigned int row=0;row<A.rows;row+=1){
				testOutputSensorAFile << A.at<double>(row,0) << ",";
				testOutputSensorAFile << A.at<double>(row,1) << ",";
				testOutputSensorAFile << A.at<double>(row,2) ;
				testOutputSensorAFile << endl;

				testOutputSensorBFile << B.at<double>(row,0) << ",";
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

			/* End of Logging */

			/* Construct the H matrix */
			Mat centroidA_row = centroidA;
			Mat centroidB_row = centroidB;

			for(int row=0;row<A.rows-1;row++){
				centroidA.push_back(centroidA_row);
				centroidB.push_back(centroidB_row);
			}

			// A is the reference B is the body
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
			bodyFrameScene->GetRMatrix(&rotationDataFile,true);
			bodyFrameScene->GetTMatrix(&translationDataFile,true);

			Scene *refFrameScene = this->_orderedScenes.at(refFrameOrder-1);

			bodyFrameScene->SetCalibration(true);
			refFrameScene->SetCalibration(true);

			calibrateSuccess = true;
		}else{
			calibrateSuccess = false;
		}

		return calibrateSuccess;
	}

	void MinorityViewportAlgo::ProcessSensorData(string sensorData){
		Json::Value root;   
		Json::Reader reader;

		if (!sensorData.empty()&&reader.parse(sensorData,root))
		{
			long timeStamp = root.get("TIME_STAMP","0").asDouble();
			Json::Value skeletons_JSON = root["SENSOR_JSON"];

			unsigned int numOfSkeletons = skeletons_JSON.size();
			for(unsigned short skeletons=0;skeletons<numOfSkeletons;skeletons++){
				MinorityViewport::Skeleton newSkeleton(skeletons_JSON[skeletons],timeStamp);
				this->LoadSkeleton(newSkeleton);
			}
		}
	}

	void MinorityViewportAlgo::LoadSkeleton(Skeleton newSkeleton){

		unsigned int clientId = newSkeleton.GetClientId();
		string sensorId = newSkeleton.GetSensorId();

		Client *client = this->_clients->At(clientId);

		if(client!=NULL){
			Sensor *sensor = client->ExtractSensor(sensorId);

			sensor->UpdateScene(newSkeleton);
		}
	}

	void MinorityViewportAlgo::MergeScenes(){

		Mat R,T,Combi_R,Combi_T,transformedSkeletonMatrix;
		long start=0,end=0;
		double threshold = 0.04,diff_x=0,diff_y=0,diff_z=0;
		unsigned int randomSkeletonId;

		this->_globalScene->ManualClear();
		
		//* Optimisation - remove mutex locks since only read and would affect real-time performance */
		vector<Scene*> orderedScenes = this->_orderedScenes;

		/* Do the comparison with reference frame skeletons and discard skeletons as necessary */
		if(orderedScenes.size()>0){
			for(vector<Scene*>::reverse_iterator orderedSceneItr = orderedScenes.rbegin();orderedSceneItr!=orderedScenes.rend();orderedSceneItr++){
			
				if((*orderedSceneItr)!=NULL&&(*orderedSceneItr)->GetCalibration()){

					Combi_R = Mat::eye(3,3, CV_32F);
					Combi_T = Mat(3,1, CV_32F,double(0));

					Scene *leftScenePtr = (*orderedSceneItr)->GetLeftFrame();
					map<unsigned short,Skeleton> currentSceneSkeletons = (*orderedSceneItr)->GetSkeletons();
					randomSkeletonId  = rand()%100;

					for(vector<Scene*>::reverse_iterator prevSceneItr = orderedSceneItr;prevSceneItr!=orderedScenes.rend();prevSceneItr++){
						if((*prevSceneItr)->GetLeftFrame()!=NULL){
								R = (*prevSceneItr)->GetRMatrix(NULL,false); //3x3
								T = (*prevSceneItr)->GetTMatrix(NULL,false); //3x1

								Combi_R = Combi_R * R;
								add(Combi_T,T,Combi_T,noArray(),CV_32F);
							}
					}

					start=this->_curTime->GetTicks_ms();

					if(leftScenePtr==NULL){
						//This is the first scene => no transformation required
						for(map<unsigned short,Skeleton>::iterator	bodyFrameSkeleton=currentSceneSkeletons.begin(); 
																	bodyFrameSkeleton!=currentSceneSkeletons.end();
																	bodyFrameSkeleton++){
							bodyFrameSkeleton->second.UnsetShared();

							this->_globalScene->Update(randomSkeletonId,bodyFrameSkeleton->second);
						}
					}else {
						map<unsigned short,Skeleton> leftFrameSkeletons = leftScenePtr->GetSkeletons();

						if(leftFrameSkeletons.size()==0){
							for(map<unsigned short,Skeleton>::iterator	bodyFrameSkeleton=currentSceneSkeletons.begin(); 
																		bodyFrameSkeleton!=currentSceneSkeletons.end();
																		bodyFrameSkeleton++){
								bodyFrameSkeleton->second.UnsetShared();

								transformedSkeletonMatrix = this->TransformSkeletonMatrix(bodyFrameSkeleton->second.GetCompletePointsVectorMatrix(NULL,false),Combi_R,Combi_T);
								bodyFrameSkeleton->second.ConvertVectorMatrixtoSkeletonPoints(transformedSkeletonMatrix);

								this->_globalScene->Update(randomSkeletonId,bodyFrameSkeleton->second);
							}
						}else{
							R = (*orderedSceneItr)->GetRMatrix(NULL,false); //3x3
							T = (*orderedSceneItr)->GetTMatrix(NULL,false); //3x1
							for(map<unsigned short,Skeleton>::iterator  bodyFrameSkeleton=currentSceneSkeletons.begin();
																		bodyFrameSkeleton!=currentSceneSkeletons.end();
																		bodyFrameSkeleton++){

								/* Transform all the skeletons within body frame to the left frame coordinates*/
								Skeleton matchingSkeleton(bodyFrameSkeleton->second);
								transformedSkeletonMatrix = this->TransformSkeletonMatrix(matchingSkeleton.GetCompletePointsVectorMatrix(NULL,false),R,T);
								matchingSkeleton.ConvertVectorMatrixtoSkeletonPoints(transformedSkeletonMatrix);
									
								/* Do comparison with the left scene skeletons */
								for(map<unsigned short,Skeleton>::iterator  refFrameSkeleton=leftFrameSkeletons.begin();
																			refFrameSkeleton!=leftFrameSkeletons.end();
																			refFrameSkeleton++){
										/* Comparison */
										diff_x = abs(refFrameSkeleton->second.pos_x-matchingSkeleton.pos_x);
										diff_y = abs(refFrameSkeleton->second.pos_y-matchingSkeleton.pos_y);
										diff_z = abs(refFrameSkeleton->second.pos_z-matchingSkeleton.pos_z);

										if(diff_x<threshold||diff_y<threshold||diff_z<threshold){
												//Deemed the same human appear in both left and right scene
											   
												//Eliminate the skeleton on the left scene that has the same coordinates as the cur scene skeleton's translated coordinates
												leftScenePtr->Remove(refFrameSkeleton->first);
												
												bodyFrameSkeleton->second.SetShared();
												
												transformedSkeletonMatrix = this->TransformSkeletonMatrix(bodyFrameSkeleton->second.GetCompletePointsVectorMatrix(NULL,false),Combi_R,Combi_T);
												bodyFrameSkeleton->second.ConvertVectorMatrixtoSkeletonPoints(transformedSkeletonMatrix);

												this->_globalScene->Update(randomSkeletonId,bodyFrameSkeleton->second);
												break;
										}
								}
							}
						}
					}

					end=this->_curTime->GetTicks_ms();
					*this->mergingLogFile << to_string(end-start);
					*this->mergingLogFile << endl;
				}
			}
		}
	}

	/**
	 *	Implement the formulat 
	 *	A (3xn) = R(3x3)*B(3xn)+T(3xn)
	 */
	Mat MinorityViewportAlgo::TransformSkeletonMatrix(Mat bodyFramesSkeleton,Mat R, Mat T)
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

	void MinorityViewportAlgo::RefreshScenesSet(){

		this->_scenesSet.clear();

		Client *clientPtr;
		
		/* Load all sensors from clients into the unordered sceneset */

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

	Scene* MinorityViewportAlgo::GetGlobalScene(){
		if(this->_orderedScenes.size()!=0){
			this->MergeScenes();
			return this->_globalScene;
		}else{
			return NULL;
		}
	}

	Scene* MinorityViewportAlgo::GetLocalSceneBySensorId(string sensorId){

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

	void MinorityViewportAlgo::ReportStatus(string msg){
		cout << "Viewport : " << msg << endl;
	}
}