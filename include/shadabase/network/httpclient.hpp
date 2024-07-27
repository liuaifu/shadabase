// 
// Copyright (c) 2024 laf163@gmail.com
// This file is part of the shadabase library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//

#pragma once

#include <map>
#include <string>
#include <boost/asio/awaitable.hpp>


namespace shadabase {
namespace network {
namespace http_client {
	namespace net = boost::asio;

	typedef std::map<std::string, std::string> http_headers;

	class http_response {
	public:
		// Gets the status code of the HTTP response.
		int status_int() const {
			return status_;
		}

		// Sets the status code of the HTTP response.
		void set_status_int(int status) {
			status_ = status;
		}

		// Gets the reason of the HTTP response.
		const std::string& reason() const {
			return reason_;
		}

		// Sets the reason of the HTTP response.
		void set_reason(const std::string& reason) {
			reason_ = reason;
		}

		// Gets the headers of the HTTP response.
		const http_headers& headers() const {
			return headers_;
		}

		// Sets the headers of the HTTP response.
		void set_headers(const http_headers& headers) {
			headers_ = headers;
		}

		// Gets the response body as a string.
		const std::string_view body() const {
			return body_;
		}

		// Sets the response body.
		void set_body(std::string&& body) {
			body_ = std::move(body);
		}

	private:
		int status_ = 0;
		std::string reason_;
		http_headers headers_;
		std::string body_;
	};

	/**
	* @brief Downloads a file from the provided URL.
	*
	* This function retrieves the content from the specified URL and saves it to the
	* specified local file path. It is designed to handle HTTP and HTTPS URLs,
	* providing a simple interface for file downloading tasks.
	*
	* @param[in] url The URL from which to download the file. The URL must be valid
	*                and the scheme must be HTTP or HTTPS.
	* @param[in] save_to The local file path where the downloaded content will be saved.
	*                    This can be a full absolute path or a path relative to the
	*                    directory containing the current executable.
	* @return None
	*
	* @note If the download fails for any reason (e.g., network error, invalid URL),
	*       the function may throw an exception.
	*
	* @throws std::exception If the URL is not reachable or another error occurs
	*                            during the download process.
	*/
	void download(std::string_view str_url, std::string_view save_to);

	/**
	* @brief Downloads a file from the provided URL.
	*
	* This function retrieves the content from the specified URL and saves it to the
	* specified local file path. It is designed to handle HTTP and HTTPS URLs,
	* providing a simple interface for file downloading tasks.
	*
	* @param[in] url The URL from which to download the file. The URL must be valid
	*                and the scheme must be HTTP or HTTPS.
	* @param[in] save_to The local file path where the downloaded content will be saved.
	*                    This can be a full absolute path or a path relative to the
	*                    directory containing the current executable.
	* @return None
	*
	* @note If the download fails for any reason (e.g., network error, invalid URL),
	*       the function may throw an exception.
	*
	* @throws std::exception If the URL is not reachable or another error occurs
	*                            during the download process.
	*/
	net::awaitable<void> async_download(std::string_view str_url, std::string_view save_to);

	/**
	* @brief Asynchronously sends an HTTP POST request to a specified URL.
	*
	* @param[in] url The target URL to which the data will be posted.
	* @param[in] headers HTTP headers to be sent with the request.
	* @param[in] body The body of the request.
	* @return http_response
	*
	* @note If the download fails for any reason (e.g., network error, invalid URL),
	*       the function may throw an exception.
	*
	* @throws std::exception If the URL is not reachable or another error occurs
	*                            during the post process.
	*/
	net::awaitable<http_response> async_post(
		std::string_view url,
		const std::map<std::string, std::string>& headers,
		std::string_view body);
}		// namespace http_client
}		// namespace network
}		// namespace shadabase

#include <shadabase/network/impl/httpclient.ipp>
