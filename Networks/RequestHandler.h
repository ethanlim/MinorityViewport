#ifndef HTTP_SERVER_REQUEST_HANDLER_HPP
#define HTTP_SERVER_REQUEST_HANDLER_HPP

#include <string>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "MimeTypes.h"
#include "Reply.h"
#include "Request.h"
#include "../Data/Skeleton.h"
#include "../Data/Client.h"
#include "../Data/ClientsList.h"
#include "../Data/JobsQueue.h"
#include "../Algorithms/MinorityViewport.h"

/* Do not move the ordering of boost library*/
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace http {
	namespace server {

		struct reply;
		struct request;

		class request_handler : private boost::noncopyable
		{
			private:
			  std::string _doc_root;
	  
			  MultipleDepthSensorsServer::MinorityViewport *_viewport;

			  /// Perform URL-decoding on a string. Returns false if the encoding was invalid.
			  static bool url_decode(const std::string& in, std::string& out);

			  /// Extract Value of Request Header
			  std::string request_header_val(const request& req, std::string request_header);
			public:
			  /// Construct with a directory containing files to be served.
			  explicit request_handler(const std::string& doc_root,
									   MultipleDepthSensorsServer::MinorityViewport *viewport);

			  /// Handle a request and produce a reply.
			  void handle_request(const request& req, reply& rep);
		};
	} 
}

#endif