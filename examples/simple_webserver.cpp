/*
MIT License

Copyright (c) 2025 Shults Bogdan aka K1joL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <ioteyeserver.hpp>

using namespace ioteye;

class HelloWorldHandler : public ioteye::HttpResourceHandler {
public:
    std::shared_ptr<ioteye::HttpResponse> renderGET(const ioteye::HttpRequest& req) override {
        auto response = std::make_shared<ioteye::HttpResponse>(ioteye::OK, "Hello World!");
        (void)req; 
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
            .setTcpPort(8080) // Try another one if you get an error Example: 8081 
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