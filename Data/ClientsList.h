#ifndef CLIENTS_LIST_H
#define CLIENTS_LIST_H

#include "Client.h"
#include <cstdlib> 
#include <ctime>

#include <list>
#include <queue>

using namespace std;

namespace MultipleKinectsPlatformServer{

	class ClientsList{
		private:
			queue<unsigned int> avaliableIDs;

			vector<Client> _raw_client_list;
			unsigned int GenerateNextClientId();
			bool ClientIdIsPrevAssigned(unsigned int id);

			unsigned int _min_id_num;
			unsigned int _max_id_num;
		public:
			ClientsList();
			~ClientsList();
			unsigned int AddClient(string physical_loc, string ip_addr);
			void RemoveClient(unsigned int id);
			unsigned int Size();
			Client* At(unsigned int id);
			Client* AtIdx(unsigned int idx); 
	};
}

#endif