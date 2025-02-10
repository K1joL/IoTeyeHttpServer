#include "ioteyeserver.hpp"

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