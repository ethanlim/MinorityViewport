#include "ClientsList.h"

namespace MultipleKinectsPlatformServer{

	ClientsList::ClientsList(){
		this->_raw_client_list.clear();
		this->_min_id_num = 1;
		this->_max_id_num = 1000;
	}

	ClientsList::~ClientsList(){

	}

	unsigned int ClientsList::Size(){
		return this->_raw_client_list.size();
	}

	unsigned int ClientsList::AddClient(string physical_loc, string ip_addr){

		unsigned int newClientId = this->GenerateNextClientId();

		Client newClient(newClientId,physical_loc,ip_addr);

		this->_raw_client_list.push_back(newClient);

		return newClientId;
	}

	void ClientsList::RemoveClient(unsigned int id){

		this->avaliableIDs.push(id);

		for(unsigned int client=0; client<this->_raw_client_list.size(); client+=1){
			if(this->_raw_client_list.at(client).GetId()==id){
				this->_raw_client_list.erase(this->_raw_client_list.begin()+client);
			}
		}
	}

	unsigned int ClientsList::GenerateNextClientId(){

		unsigned int assigned_id = 0;
		srand((unsigned int) time(NULL));

		if(this->avaliableIDs.size()>0){
			assigned_id = this->avaliableIDs.front();
			this->avaliableIDs.pop();
		}else{
			do{
				assigned_id = rand()%this->_max_id_num+this->_min_id_num;
			}while(this->ClientIdIsPrevAssigned(assigned_id));
		}
		return assigned_id;
	}

	bool ClientsList::ClientIdIsPrevAssigned(unsigned int id){
		for(unsigned int client=0;client<this->_raw_client_list.size();client+=1){
			if(this->_raw_client_list.at(client).GetId()==id){
				return true;
			}
		}
		return false;
	}

	Client* ClientsList::At(unsigned int id){
		for(unsigned int client=0;client<this->_raw_client_list.size();client+=1){
			if(this->_raw_client_list.at(client).GetId()==id){
				return &this->_raw_client_list.at(client);
			}
		}
	}

	Client* ClientsList::AtIdx(unsigned int idx){
		return &this->_raw_client_list[idx];
	}
}