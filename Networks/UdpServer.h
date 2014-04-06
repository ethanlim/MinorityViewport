#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <string>
#include <sys/types.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
 
#pragma comment(lib, "Ws2_32.lib")

#include "../Algorithms/MinorityViewport.h"

using namespace std;

namespace MultipleDepthSensorsPlatformServer{
	class UdpServer{
		private:
			int				_socket;
			int				_port;
			string			_address;
			struct addrinfo	_addressInfo;

			MinorityViewport *_viewport;
		public:
			UdpServer(const string address, const unsigned short port,MinorityViewport* viewport);
			~UdpServer();
			void Run();

			int	get_socket() const;
			int	get_port() const;
			string	get_addr() const;
	};
}

#endif
