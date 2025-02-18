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

#include <iostream>
#include <memory>

#include "ioteyeserver.hpp"

bool canConvertToInt(const std::string& str) {
    try {
        std::stoi(str);
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

// Create a simple echo handler
class EchoHandler : public ioteye::HttpResourceHandler {
public:
    std::shared_ptr<ioteye::HttpResponse> renderGET(const ioteye::HttpRequest& req) override {
        auto response = std::make_shared<ioteye::HttpResponse>();
        response->setStatusCode(200);
        std::string p1Str = req.getArg("param1");
        std::string p2Str = req.getArg("param2");
        int p1 = 0, p2 = 0;

        response->setBody("You requested: " + req.getUri() + "\n");
        if (!p1Str.empty() && !p2Str.empty()) {
            if (canConvertToInt(p1Str) && canConvertToInt(p2Str)) {
                p1 = stoi(p1Str);
                p2 = stoi(p2Str);
                response->addBody("Summ: " + std::to_string(p1 + p2) + "\n");
            } else
                response->addBody("Cant sum " + p1Str + " and " + p2Str + '\n');
        }
        response->setHeader("Content-Type", "text/plain");
        return response;
    }

    std::shared_ptr<ioteye::HttpResponse> renderPOST(const ioteye::HttpRequest& req) override {
        auto response = std::make_shared<ioteye::HttpResponse>();
        response->setStatusCode(200);
        response->setBody("You sent a POST request to: " + req.getUri() + "\n" +
                          "With Body: " + req.getBody());
        response->setHeader("Content-Type", "text/plain");
        return response;
    }
    std::shared_ptr<ioteye::HttpResponse> renderPUT(const ioteye::HttpRequest& req) override {
        auto response = std::make_shared<ioteye::HttpResponse>();
        response->setStatusCode(200);
        response->setBody("You sent a PUT request to: " + req.getUri() + "\n" +
                          "With Body: " + req.getBody());
        response->setHeader("Content-Type", "text/plain");
        return response;
    }
    std::shared_ptr<ioteye::HttpResponse> renderDELETE(const ioteye::HttpRequest& req) override {
        auto response = std::make_shared<ioteye::HttpResponse>();
        response->setStatusCode(200);
        response->setBody("You sent a DELETE request to: " + req.getUri());
        response->setHeader("Content-Type", "text/plain");
        return response;
    }
};

int main() {
    try {
        auto echoHandler = std::make_shared<EchoHandler>();
        ioteye::Webserver server = ioteye::Webserver::Builder()
                                       .setTcpPort(8080)
                                       .setUdpOn()
                                       .setResource("/echo", echoHandler)
                                       .setResource("/echo/{param1}/{param2}", echoHandler)
                                       .build();
        server.start();

        char key;
        while (true) {
            key = getchar();
            if (key == 27) {
                server.shutdown();
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}