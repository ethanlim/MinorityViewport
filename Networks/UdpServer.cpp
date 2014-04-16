#include "UdpServer.h"

namespace MinorityViewport{

	UdpServer::UdpServer(const string address, const unsigned short port,MinorityViewportAlgo *viewport):
		_address(address),_port(port)
	{
		this->_viewport = viewport;
		this->_socket = SOCK_DGRAM;
	}

	UdpServer::~UdpServer(){
	}

	void UdpServer::Run(){

		SOCKET listen_socket;
		struct sockaddr_in local, from;

		int retval, fromlen=sizeof(from);

		char Buffer[65536];
		
		WSADATA wsaData;

		retval = WSAStartup(MAKEWORD(2,2), &wsaData);

		if(retval==NO_ERROR){
			listen_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

			local.sin_family		= AF_INET;
			local.sin_addr.s_addr	= inet_addr(this->_address.c_str());
			local.sin_port			= htons(this->_port);

			// Need to add a global scope or the wrong bind is used => there is one conflicting one from C++ std::bind
			retval = ::bind(listen_socket, (struct sockaddr*)&local, sizeof(local));

			while(1){
				retval = recvfrom(listen_socket,Buffer,sizeof(Buffer),0, (struct sockaddr *)&from, &fromlen);
            
				if (retval >0)
				{
					Buffer[retval] = '\0';
					String raw_recv(Buffer);
					this->_viewport->ProcessSensorData(raw_recv);
				}else if(retval == SOCKET_ERROR){
					fprintf(stderr,"Server: socket() failed with error %d\n", WSAGetLastError());
				}
			}
		}
	}

}