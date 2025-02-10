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