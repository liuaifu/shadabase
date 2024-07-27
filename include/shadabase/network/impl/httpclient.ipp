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
#include <boost/url.hpp>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/redirect_error.hpp>
#include <shadabase/network/httpclient.hpp>
#include <shadabase/network/impl/root_certificates.hpp>


namespace shadabase {
namespace network {
namespace http_client {
	namespace beast = boost::beast; // from <boost/beast.hpp>
	namespace http = beast::http;   // from <boost/beast/http.hpp>
	namespace net = boost::asio;    // from <boost/asio.hpp>
	namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
	using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

	inline void download_http(const boost::urls::url& url, std::string_view save_to) {
		net::io_context ioc;
		auto host = url.host();
		uint16_t port = url.has_port() ? url.port_number() : 80;

		tcp::socket          socket(ioc);
		tcp::resolver        resolver(ioc);
		tcp::resolver::query query(host, std::to_string(port));
		net::connect(socket, resolver.resolve(query));

		// Http request send
		http::request<http::string_body> req(http::verb::get, url.encoded_target(), 11);
		req.set(http::field::host, host);
		http::write(socket, req);

		// Http response get
		boost::beast::flat_buffer              buffer;
		http::response_parser<http::file_body> res;
		boost::beast::error_code               ec;
		res.get().body().open(save_to.data(), boost::beast::file_mode::write, ec);
		res.body_limit(std::numeric_limits<uint64_t>::max());
		http::read(socket, buffer, res);
		boost::system::error_code sys_ec;
		socket.shutdown(net::socket_base::shutdown_both, sys_ec);
	}

	inline net::awaitable<void> async_download_http(const boost::urls::url& url, std::string_view save_to) {
		net::io_context ioc;
		auto host = url.host();
		uint16_t port = url.has_port() ? url.port_number() : 80;

		tcp::socket socket(co_await net::this_coro::executor);
		tcp::resolver resolver(co_await net::this_coro::executor);
		auto endpoint = co_await resolver.async_resolve(host, std::to_string(port), net::use_awaitable);
		co_await net::async_connect(socket, endpoint, net::use_awaitable);

		// Http request send
		http::request<http::string_body> req(http::verb::get, url.encoded_target(), 11);
		req.set(http::field::host, host);
		co_await http::async_write(socket, req, net::use_awaitable);

		// Http response get
		boost::beast::flat_buffer buffer;
		http::response_parser<http::file_body> res;
		boost::beast::error_code ec;
		res.get().body().open(save_to.data(), boost::beast::file_mode::write, ec);
		res.body_limit(std::numeric_limits<uint64_t>::max());
		co_await http::async_read(socket, buffer, res, net::use_awaitable);
		boost::system::error_code sys_ec;
		socket.shutdown(net::socket_base::shutdown_both, sys_ec);
	}

	inline void download_https(const boost::urls::url& url, std::string_view save_to) {
		net::io_context ioc;
		auto host = url.host();
		uint16_t port = url.has_port() ? url.port_number() : 443;

		ssl::context                ctx{ ssl::context::sslv23_client };
		ssl::stream<tcp::socket>    stream{ ioc, ctx };
		tcp::resolver               resolver{ ioc };
		// TODO should have a timer in case of a hang
		load_root_certificates(ctx);

		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
			boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
			throw boost::system::system_error{ ec };
		}
		//TODO resolve is depreciated, use endpoint
		auto const results = resolver.resolve(host, std::to_string(port));

		boost::asio::connect(stream.next_layer(), results.begin(), results.end());
		stream.handshake(ssl::stream_base::client);

		// Http request send
		http::request<http::string_body> req(http::verb::get, url.encoded_target(), 11);
		req.set(http::field::host, host);
		// req.set(http::field::user_agent, "boost.beast");
		http::write(stream, req);

		// Http response get
		boost::beast::flat_buffer              buffer;
		http::response_parser<http::file_body> res;
		boost::beast::error_code               ec;
		res.get().body().open(save_to.data(), boost::beast::file_mode::write, ec);
		res.body_limit(std::numeric_limits<uint64_t>::max());
		http::read(stream, buffer, res);
		// Gracefully close the stream
		boost::system::error_code ec2;
		stream.shutdown(ec2);
		if (ec2 == boost::asio::error::eof || ec2 == boost::asio::ssl::error::stream_truncated) {
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			ec2.clear();
		}
		if (ec2)
			throw boost::system::system_error{ ec2 };
	}

	inline net::awaitable<void> async_download_https(const boost::urls::url& url, std::string_view save_to) {
		auto host = url.host();
		uint16_t port = url.has_port() ? url.port_number() : 443;

		ssl::context                ctx{ ssl::context::sslv23_client };
		ssl::stream<tcp::socket>    stream{ co_await net::this_coro::executor, ctx };
		tcp::resolver               resolver{ co_await net::this_coro::executor };
		// TODO should have a timer in case of a hang
		load_root_certificates(ctx);

		// Set SNI Hostname (many hosts need this to handshake successfully)
		if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
			boost::system::error_code ec{ static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category() };
			throw boost::system::system_error{ ec };
		}
		// TODO resolve is depreciated, use endpoint
		auto const results = co_await resolver.async_resolve(host, std::to_string(port), net::use_awaitable);

		co_await boost::asio::async_connect(stream.next_layer(), results.begin(), results.end(), net::use_awaitable);
		co_await stream.async_handshake(ssl::stream_base::client, net::use_awaitable);

		// Http request send
		http::request<http::string_body> req(http::verb::get, url.encoded_target(), 11);
		req.set(http::field::host, host);
		// req.set(http::field::user_agent, "boost.beast");
		co_await http::async_write(stream, req, net::use_awaitable);

		// Http response get
		boost::beast::flat_buffer buffer;
		http::response_parser<http::file_body> res;
		boost::beast::error_code ec;
		res.get().body().open(save_to.data(), boost::beast::file_mode::write, ec);
		res.body_limit(std::numeric_limits<uint64_t>::max());
		co_await http::async_read(stream, buffer, res, net::use_awaitable);
		// Gracefully close the stream
		boost::system::error_code sys_ec;
		co_await stream.async_shutdown(net::redirect_error(net::use_awaitable, sys_ec));
		if (sys_ec == boost::asio::error::eof || sys_ec == boost::asio::ssl::error::stream_truncated) {
			// Rationale:
			// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
			sys_ec.clear();
		}
		if (sys_ec)
			throw boost::system::system_error{ sys_ec };
	}

	inline void download(std::string_view str_url, std::string_view save_to) {
		boost::urls::url url(str_url);
		if (url.scheme() == "http")
			download_http(url, save_to);
		else if (url.scheme() == "https")
			download_https(url, save_to);
	}

	inline net::awaitable<void> async_download(std::string_view str_url, std::string_view save_to) {
		boost::urls::url url(str_url);
		if (url.scheme() == "http")
			co_await async_download_http(url, save_to);
		else if (url.scheme() == "https")
			co_await async_download_https(url, save_to);
	}

	inline net::awaitable<http_response> async_post_http(
		const boost::urls::url& url,
		const std::map<std::string, std::string>& headers,
		const std::string_view& body) {
		net::io_context ioc;
		auto host = url.host();
		uint16_t port = url.has_port() ? url.port_number() : 80;

		tcp::socket socket(co_await net::this_coro::executor);
		tcp::resolver resolver(co_await net::this_coro::executor);
		auto endpoint = co_await resolver.async_resolve(host, std::to_string(port), net::use_awaitable);
		co_await net::async_connect(socket, endpoint, net::use_awaitable);

		// Http request send
		http::request<http::string_body> req(http::verb::post, url.encoded_target(), 11);
		req.set(http::field::host, host);
		req.set(http::field::content_type, "text/plain");
		for(const auto& [k, v] : headers)
			req.set(k, v);
		req.body() = body;
		req.prepare_payload();
		co_await http::async_write(socket, req, net::use_awaitable);

		// This buffer is used for reading and must be persisted
		beast::flat_buffer buffer;

		// Get the response
		// http::response<http::string_body> res;

		// Receive the HTTP response
		// co_await http::async_read(socket, buffer, res, net::use_awaitable);

		http::response<http::dynamic_body> res;
		http::response_parser<http::dynamic_body> p;
		co_await http::async_read_header(socket, buffer, p, net::use_awaitable);
		// move the result over
		res = p.release();
		std::cout << res << std::endl;
		while (true) {
			http::response<http::string_body> res2;
			beast::flat_buffer buffer2;
			http::response_parser<http::string_body> p2;
			co_await http::async_read_some(socket, buffer2, p2, net::use_awaitable);
			res2 = p2.release();
			std::cout << res2 << std::endl;
		}

		http_response http_res;
		http_res.set_status_int(res.result_int());
		http_res.set_reason(res.reason());
		// http_res.set_body(std::move(res.body()));
		http_headers res_headers;
		for (auto it = res.begin(); it != res.end(); it++)
			res_headers[it->name_string()] = it->value();
		http_res.set_headers(res_headers);

		boost::system::error_code sys_ec;
		socket.shutdown(net::socket_base::shutdown_both, sys_ec);

		co_return http_res;
	}

	inline net::awaitable<http_response> async_post(
		std::string_view str_url,
		const std::map<std::string, std::string>& headers,
		std::string_view body) {
		boost::urls::url url(str_url);
		if (url.scheme() == "http")
			co_return co_await async_post_http(url, headers, body);
		//else if (url.scheme() == "https")
		//	return co_await async_post(url, headers, body);
		co_return http_response{};
	}
}		// namespace http_client
}		// namespace network
}		// namespace shadabase
