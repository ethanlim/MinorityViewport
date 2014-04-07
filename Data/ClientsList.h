#ifndef CLIENTS_LIST_H
#define CLIENTS_LIST_H

#include <cstdlib> 
#include <ctime>
#include <list>
#include <queue>

#include "Client.h"
#include "../Misc/Timer.h"

using namespace std;

namespace MultipleDepthSensorsServer{

	class ClientsList{
		private:
			queue<unsigned int> avaliableIDs;

			vector<Client*> _raw_client_list;
			unsigned int GenerateNextClientId();
			bool ClientIdIsPrevAssigned(unsigned int id);
			Timer *_curTime;

			unsigned int _min_id_num;
			unsigned int _max_id_num;
		public:
			ClientsList(Timer *time);
			~ClientsList();
			unsigned int AddClient(string physical_loc, string ip_addr);
			void RemoveClient(unsigned int id);
			unsigned int Size();
			Client* At(unsigned int id);
			Client* AtIdx(unsigned int idx); 
	};
}

#endif