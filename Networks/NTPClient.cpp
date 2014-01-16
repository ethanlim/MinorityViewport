#include "NTPClient.h"

namespace MultipleKinectsPlatformServer{
	
	NTPClient::NTPClient(string i_hostname)
		:_host_name(i_hostname),_port(123)
	{


	}

	long NTPClient::RequestDatetime_UNIX(){

		time_t timeRecv;

		boost::asio::io_service io_service;

		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(
													 boost::asio::ip::udp::v4(),
													 "0.nettime.pool.ntp.org",
													 "ntp");

		boost::asio::ip::udp::endpoint receiver_endpoint = *resolver.resolve(query);

		boost::asio::ip::udp::socket socket(io_service);
		socket.open(boost::asio::ip::udp::v4());
		
		boost::array<unsigned char, 48> sendBuf  = {010,0,0,0,0,0,0,0,0};
    
		socket.send_to(boost::asio::buffer(sendBuf), receiver_endpoint);

		boost::array<unsigned long, 1024> recvBuf;
		boost::asio::ip::udp::endpoint sender_endpoint;
		
		try{

			size_t len = socket.receive_from(
												boost::asio::buffer(recvBuf), 
												sender_endpoint
											);

			timeRecv = ntohl((time_t)recvBuf[4]);

			timeRecv-= 2208988800U; 

		}catch (std::exception& e){

			std::cerr << e.what() << std::endl;

		}

		return (long)timeRecv;
	}

}