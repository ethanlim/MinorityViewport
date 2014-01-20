#include "ClientsList.h"

namespace MultipleKinectsPlatformServer{

	ClientsList::ClientsList(){
		this->raw_client_list.clear();
		this->min_id_num = 1;
		this->max_id_num = 1000;
		
	}

	ClientsList::~ClientsList(){

	}

	unsigned int ClientsList::Size(){
		return this->raw_client_list.size();
	}

	unsigned int ClientsList::AddClient(string physical_loc, string ip_addr){

		unsigned int newClientId = this->GenerateNextClientId();

		Client newClient(newClientId,physical_loc,ip_addr);

		this->raw_client_list.push_back(newClient);

		return newClientId;
	}

	void ClientsList::RemoveClient(unsigned int id){

		this->avaliableIDs.push(id);

		for(std::list<Client>::iterator it = this->raw_client_list.begin(); it != this->raw_client_list.end(); it++){
			
			if(it->GetId()==id){
				this->raw_client_list.erase(it);
				return;
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

				assigned_id = rand()%this->max_id_num+this->min_id_num;

			}while(this->ClientIdIsPrevAssigned(assigned_id));

		}

		return assigned_id;
	}

	bool ClientsList::ClientIdIsPrevAssigned(unsigned int id){

		for(std::list<Client>::iterator it = this->raw_client_list.begin(); it != this->raw_client_list.end(); it++){
			
			if(it->GetId()==id){
				return true;
			}
		}

		return false;
	}

	Client ClientsList::At(unsigned int id){
		
		for(list<Client>::iterator itr = this->raw_client_list.begin();itr!=this->raw_client_list.end();itr++){
			if(itr->GetId()==id){
				return *itr;
			}
		}

	}
}