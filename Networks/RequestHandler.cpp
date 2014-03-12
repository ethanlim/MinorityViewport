#include "RequestHandler.h"

namespace http {
	namespace server {

	request_handler::request_handler(const std::string& doc_root,
									 MultipleKinectsPlatformServer::JobsQueue *cur_jobs_queue, 
									 MultipleKinectsPlatformServer::ClientsList *client_list,
									 MultipleKinectsPlatformServer::MinorityViewport *viewport)
									:_doc_root(doc_root),
									 _job_queue(cur_jobs_queue),
									 _client_list(client_list),
									 _viewport(viewport)
	{
	}

	void request_handler::handle_request(const request& req, reply& rep)
	{
	  rep.headers.resize(2);

	  // Decode url to path.
	  std::string request_path;
	 
	  if (!url_decode(req.uri, request_path))
	  {
		rep = reply::stock_reply(reply::bad_request);
		return;
	  }

	  // Bad Request
	  if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos)
	  {
		rep = reply::stock_reply(reply::bad_request);
		return;
	  }

	  // If path ends in slash (i.e. is a directory) then add "index.html".
	  if (request_path[request_path.size() - 1] == '/')
	  {
		request_path += "index.html";
	  }

	  /********************************************************/
	  /*
		 Endpoint for Sensor Data
		 @parameters
		 string SensorData_JSON
		 string time_stamp
		 @return
		 nil
	  */
	  /********************************************************/
	  if(request_path == "/api/sensors/data.json")
	  {
		// Get Sensor JSON in the header
		string sensorData_JSON = this->request_header_val(req,"SENSOR_JSON");
		string time_stamp = this->request_header_val(req,"TIME_STAMP");

		/* Insert it into the Job Queue */
		if(!sensorData_JSON.empty()&&!time_stamp.empty()){
			//_job_queue->push(sensorData_JSON,time_stamp);
			this->_viewport->ProcessSensorData(time_stamp,sensorData_JSON);
		}
	  }

	  /********************************************************/
	  /*
		 Endpoint for Client Registration
		 @parameters
		 string physical location
		 string ip address
		 @return
		 unsigned int client_id
	  */
	  /********************************************************/
	  if(request_path == "/api/clients/register.json")
	  {
		ofstream outputFile(_doc_root+request_path);

		string physical_location = this->request_header_val(req,"PHYSICAL_LOC");
		string ip_addr = this->request_header_val(req,"IP_ADDR");
		  
		unsigned int client_id = this->_client_list->AddClient(physical_location,ip_addr);

		outputFile << "{";
		outputFile << "\"client_id\":";
		outputFile <<  std::to_string(client_id);
		outputFile << "}";

		outputFile.close();
	  }

	  /********************************************************/
	  /*
		 Endpoint for Client Deregistration
		 @parameters
		 string client_id
		 @return
		 nil
	  */
	  /********************************************************/
	  if(request_path == "/api/clients/deregister.json")
	  {
		string deregisterClientId = this->request_header_val(req,"CLIENT_ID");

		this->_client_list->RemoveClient(std::stoi(deregisterClientId));
	  }

	  /********************************************************/
	  /*
		 Endpoint for Sensor Registration
		 @parameters
		 string Sensor List 
		 string clientId
	  */
	  /********************************************************/
	  if(request_path == "/api/sensors/register.json")
	  {
		string sensorsList_JSON = this->request_header_val(req,"SENSOR_LIST");
		unsigned int clientId = std::stoi(this->request_header_val(req,"CLIENT_ID"));

		MultipleKinectsPlatformServer::Client *extractedClient = this->_client_list->At(clientId);

		extractedClient->InitialSensorsList(sensorsList_JSON);
	  }

	  /********************************************************/
	  /*
		 Endpoint for Obtaining Client Listings
		 @parameters
		 nil
		 @return
		 string client listing
	  */
	  /********************************************************/
	  if(request_path == "/api/clients/listing.json")
	  {
		ofstream outputFile(_doc_root+request_path);

		outputFile << "{";
		outputFile << "\"clients\":";
		outputFile << "[";

		for(unsigned int clientIdx=0;clientIdx<this->_client_list->Size();clientIdx++){

			MultipleKinectsPlatformServer::Client *extractedClient = this->_client_list->AtIdx(clientIdx);
			  
			outputFile << extractedClient->ToJSON();
		}

		outputFile << "]";
		outputFile << "}";

		outputFile.close();
	  }


	  
	  /********************************************************/
	  /*
		 Endpoint for Browser to Get the Order the Sensors
		 @parameters
		 nil
	  */
	  /********************************************************/
	  if(request_path == "/api/visualisations/order.json")
	  {
		  ofstream outputFile(_doc_root+request_path);

		  outputFile << "{";
		  outputFile << "\"result\":";
		  outputFile << "[";
		  
		  if(this->_viewport->CalibrateSceneOrder()){
				outputFile << "true";
	      }else{
				outputFile << "false";
	      }
		 
		  outputFile << "]";
 		  outputFile << "}";

		  outputFile.close();
	  }

	   /********************************************************/
	  /*
		 Endpoint for Browser to Calibrating Scenes
		 @parameters
		 string sceneAOrder_JSON 
		 string skeletonA_JSON 
		 string sceneBOrder_JSON
		 string skeletonB_JSON 
	  */
	  /********************************************************/
	  if (request_path == "/api/visualisations/calibrate.json")
	  {
		  ofstream outputFile(_doc_root+request_path);
		  string sceneAOrder_JSON = this->request_header_val(req,"SCENE_A_ORDER");
		  string skeletonA_JSON = this->request_header_val(req,"SKELETON_A");
		  string sceneBOrder_JSON = this->request_header_val(req,"SCENE_B_ORDER");
		  string skeletonB_JSON = this->request_header_val(req,"SKELETON_B");

		  outputFile << "{";
		  outputFile << "\"result\":";
		  outputFile << "[";

		 
		  if(this->_viewport->CalibrateScenes(std::stoi(sceneAOrder_JSON),
											  skeletonA_JSON,
											  std::stoi(sceneBOrder_JSON),
											  skeletonB_JSON)){
				outputFile << "true";
	      }else{
				outputFile << "false";
	      }

		  outputFile << "]";
 		  outputFile << "}";
	  }

	  /********************************************************/
	  /*
		 Endpoint for Browser to Get Live Sensor Data
		 @parameters
		 string SensorData_JSON
		 string time_stamp
	  */
	  /********************************************************/
	  if (request_path == "/api/visualisations/data.json")
	  {
		  string type = this->request_header_val(req,"Request-Type");
		  ofstream outputFile(_doc_root+request_path);
		  MultipleKinectsPlatformServer::Scene *scene;
		  string Scene_JSON;

		  if(type=="global"){
			  scene = this->_viewport->GetGlobalScene();
		  }else if(type == "single"){
			  string sensorId = this->request_header_val(req,"Sensor-Id");
			  scene = this->_viewport->GetLocalSceneBySensorId(sensorId);
		  }

		  if(scene!=NULL){
			Scene_JSON = scene->ToJSON();
		  }

		  outputFile << Scene_JSON;
	  }

	  // Determine the file extension.
	  std::size_t last_slash_pos = request_path.find_last_of("/");
	  std::size_t last_dot_pos = request_path.find_last_of(".");
	  std::string extension;
	  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
	  {
		extension = request_path.substr(last_dot_pos + 1);
	  }
	  
	  // Open the file to send back.
	  std::string full_path = _doc_root + request_path;
	  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
	  if (!is)
	  {
		rep = reply::stock_reply(reply::not_found);
		return;
	  }

	  // Fill out the reply to be sent to the client.
	  rep.status = reply::ok;
	  char buf[512];
	  while (is.read(buf, sizeof(buf)).gcount() > 0)
	  rep.content.append(buf, is.gcount());
	  rep.headers[0].name = "Content-Length";
	  rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	  rep.headers[1].name = "Content-Type";
	  rep.headers[1].value = mime_types::extension_to_type(extension);

	  //Remove content of request path file after return
	  if(extension=="json"){
		  ofstream erasingFile(full_path);
		  erasingFile.close();
	  }
	}

	std::string request_handler::request_header_val(const request& req, std::string request_header){
		
		unsigned int numOfHeaders = (req.headers)._Mylast - (req.headers)._Myfirst;
		std::string header_val = "";

		for(unsigned int headerIdx=0; headerIdx<numOfHeaders;headerIdx++){
			http::server::header header = ((req.headers)._Myfirst)[headerIdx];

			if(header.name==request_header){
				header_val = header.value;
			}
		}

		return header_val;
	}

	bool request_handler::url_decode(const std::string& in, std::string& out)
	{
	  out.clear();
	  out.reserve(in.size());
	  for (std::size_t i = 0; i < in.size(); ++i)
	  {
		if (in[i] == '%')
		{
		  if (i + 3 <= in.size())
		  {
			int value = 0;
			std::istringstream is(in.substr(i + 1, 2));
			if (is >> std::hex >> value)
			{
			  out += static_cast<char>(value);
			  i += 2;
			}
			else
			{
			  return false;
			}
		  }
		  else
		  {
			return false;
		  }
		}
		else if (in[i] == '+')
		{
		  out += ' ';
		}
		else
		{
		  out += in[i];
		}
	  }
	  return true;
	}

	} // namespace server
} // namespace http