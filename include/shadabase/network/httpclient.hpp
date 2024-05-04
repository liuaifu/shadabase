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

#include <string>
#include <boost/asio/awaitable.hpp>


namespace shadabase {
namespace network {
namespace http_client {
	namespace net = boost::asio;

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
}		// namespace http_client
}		// namespace network
}		// namespace shadabase

#include <shadabase/network/impl/httpclient.ipp>
