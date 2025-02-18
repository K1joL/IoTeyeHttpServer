# IoTeye Server

IoTeye Server is a lightweight, configurable HTTP server based on [ASIO library](https://think-async.com/Asio/). This project aims to provide a robust and efficient server implementation for handling HTTP requests, suitable for embedded systems and IoT devices.

## Features

- **Resource Handling**: Easily manage and serve HTTP resources.
- **Configurable**: Customizable to fit different use cases.
- **Performance**: Optimized for performance with minimal resource usage.

## Getting Started

### Prerequisites

- C++17 or later
- CMake 3.18 or later
- ASIO 1.30.2 or later

### Installation

1. **Clone the repository**:
   ```sh
   git clone https://github.com/K1joL/IoTeyeHttpServer.git
   cd IoTeyeHttpServer
   ```

2. **Build the project**:
   ```sh
   mkdir build
   cd build
   cmake .. && cmake --build .
   ```
3. **Install library**:
    ```sh
    sudo make install
    ```
4. **Run the examples (optional)**:
   ```sh
   ./main
   ./simple_webserver
   ```
5. **Uninstall library**:
    ```sh
    sudo make uninstall
    ```

## Usage Example

Below is an example of how to use the `IotEye Server` to set up a simple web server that handles basic HTTP requests.

```CMake
# Just use find_package to find all dependencies
find_package(asio REQUIRED)
find_package(ioteyeserver REQUIRED)
find_package(Threads REQUIRED)
...
# Then link them
target_link_libraries(main PRIVATE ioteyeserver Threads::Threads asio::asio)
```

```cpp
#include <ioteyeserver.hpp>

using namespace ioteye;

class HelloWorldHandler : public ioteye::HttpResourceHandler {
public:
    std::shared_ptr<ioteye::HttpResponse> renderGET(const ioteye::HttpRequest& req) override {
        auto response = std::make_shared<ioteye::HttpResponse>(ioteye::OK, "Hello World!");
        return response;
    }
};

int main() {
    try {
        // Initialize the web server
        auto helloHandler = std::make_shared<HelloWorldHandler>();
        auto helloResource = std::make_shared<HttpResource>(helloHandler, "/hello");

        // Add the resource to the web server
        // Start the web server on port 8080 for TCP
        // Then build() to make webserver object
        Webserver webserver =
            ioteye::Webserver::Builder()
            .setTcpPort(8080)
            .setResource(helloResource)
            .build();
        // Start method is non-blocking and will return immediately
        // So we need to wait for a key press to exit or whatever you want
        webserver.start();
        // Wait for a key press (Esc) to exit
        char key;
        while (true) {
            key = getchar();
            if (key == 27) {
                webserver.shutdown();
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

In this example:
- We initialize a `WebServer` object.
- We define an `HttpResourceHandler` that responds with "Hello, World!" to any GET request at the `/hello` endpoint.
- We add this resource to the web server.
- We start the web server on port 8080 for TCP connections.
- We wait for a key press (Esc) to exit the program, ensuring the server shuts down gracefully

## License
This project is licensed under the MIT License - see the [COPYING](COPYING) file for details.