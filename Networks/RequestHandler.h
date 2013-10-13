//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_REQUEST_HANDLER_HPP
#define HTTP_SERVER_REQUEST_HANDLER_HPP

#include <string>
#include <boost/noncopyable.hpp>
#include "../Data/JobsQueue.h"

namespace http {
	namespace server {

	struct reply;
	struct request;

	/// The common handler for all incoming requests.
	class request_handler : private boost::noncopyable
	{
	public:
	  /// Construct with a directory containing files to be served.
	  explicit request_handler(const std::string& doc_root, MultipleKinectsPlatformServer::JobsQueue* cur_jobs_queue);

	  /// Handle a request and produce a reply.
	  void handle_request(const request& req, reply& rep);

	private:
	  /// The directory containing the files to be served.
	  std::string doc_root_;

	  /// Pointer to current job queue
	  MultipleKinectsPlatformServer::JobsQueue* job_queue_;

	  /// Perform URL-decoding on a string. Returns false if the encoding was invalid.
	  static bool url_decode(const std::string& in, std::string& out);

	  /// Extract Value of Request Header
	  std::string request_header_val(const request& req, std::string request_header);
	};

	} // namespace server
} // namespace http

#endif // HTTP_SERVER_REQUEST_HANDLER_HPP