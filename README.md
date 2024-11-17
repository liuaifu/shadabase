# shadabase

shadabase 是一个高效、模块化的 C++ 基础类库，旨在为开发者提供一系列常用功能接口。本库遵循 LGPL 许可协议，鼓励代码共享与再分发。

## 功能概览

- **日志库** - *计划中*：一个轻量级的日志系统，用于应用程序的日志记录需求。
- **JSON 接口** - *计划中*：提供便捷的 JSON 数据处理功能。
- **字符集编码转换** - *计划中*：支持包括 GB18030、UTF-8、UTF-16 在内的多种字符集编码转换。
- **Redis 客户端接口** - *计划中*：集成 Redis 数据库的客户端操作接口。
- **MySQL 客户端接口** - *计划中*：集成 MySQL 数据库的客户端操作接口。
- **HTTP 客户端接口** - *已完成*：实现 HTTP 请求与响应处理，支持同步和C++20协程操作。
- **WebSocket 客户端接口** - *计划中*：提供 WebSocket 通信协议的实现，用于网络通信。
- **UDP 操作** - *计划中*：支持 UDP 协议的单播和组播功能。
- **HASH 接口** - *计划中*：提供 MD5、SHA-1、SHA-256、SHA-512 等哈希值计算功能。

## 依赖

- C++20
- OpenSSL 1.0+
- Boost 1.81+

## 开始使用

1. 克隆代码库：
   ```bash
   git clone https://github.com/liuaifu/shadabase.git
   ```
2. 安装：
   ```bash
   cd shadabase
   mkdir builds
   cd builds
   cmake ..
   make
   sudo make install
   ```
2. 包含头文件到自己的项目中：
   ```c++
   #include <shadabase/network/httpclient.hpp>
   ```

## 示例

```c++
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
```

## 许可证
shadabase 使用 LGPL 许可证发布，详情见 LICENSE 文件。

## 贡献
欢迎任何形式的贡献，包括但不限于文档改进、bug 修复、新功能开发。
