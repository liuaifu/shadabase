#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <shadabase/network/httpclient.hpp>


namespace net = boost::asio;
namespace http_client = shadabase::network::http_client;

int main(int argc, char** argv) {
	std::string save_to = "ChangeLog-6.8.9.txt";
	auto url = "https://mirrors.edge.kernel.org/pub/linux/kernel/v6.x/ChangeLog-6.8.9";
	
	std::cout << "Start downloading " << url << std::endl;

	try {
		net::io_context ioc;
		net::co_spawn(ioc, http_client::async_download(url, save_to), net::detached);
		ioc.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	std::cout << "File downloaded" << std::endl;
	return 0;
}