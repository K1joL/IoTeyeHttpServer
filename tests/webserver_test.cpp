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

#include <gtest/gtest.h>

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <chrono>
#include <iostream>
#include <ioteyeserver.hpp>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

namespace ioteye {

// Mock HttpResponseHandler for testing purposes
class MockResourceHandler : public HttpResourceHandler {
public:
    std::shared_ptr<HttpResponse> renderGET(const HttpRequest& req) override {
        (void)req;
        auto response = std::make_shared<HttpResponse>();
        response->setStatusCode(200);
        response->setBody("GET Response");
        return response;
    }

    std::shared_ptr<HttpResponse> renderPOST(const HttpRequest& req) override {
        (void)req;
        auto response = std::make_shared<HttpResponse>();
        response->setStatusCode(201);
        response->setBody("POST Response: " + req.getBody());
        return response;
    }

    std::shared_ptr<HttpResponse> renderPUT(const HttpRequest& req) override {
        (void)req;
        auto response = std::make_shared<HttpResponse>();
        response->setStatusCode(200);
        response->setBody("PUT Response: " + req.getArg("id"));
        return response;
    }

    std::shared_ptr<HttpResponse> renderDELETE(
        const HttpRequest& req) override {
        (void)req;
        auto response = std::make_shared<HttpResponse>();
        response->setStatusCode(204);
        response->setBody("");
        return response;
    }
};

class BaseClass : public ::testing::Test {
public:
    BaseClass() : mockHandler(std::make_shared<MockResourceHandler>()) {
    }

protected:
    std::shared_ptr<MockResourceHandler> mockHandler;
};

// Test fixture for Webserver tests
class WebserverTest : public BaseClass {
protected:
    WebserverTest()
        : tcpPort(nextAvailablePort),
          udpPort(nextAvailablePort + 1),
          webserver(Webserver::Builder()
                        .setTcpPort(tcpPort)
                        .setUdpPort(udpPort)
                        .setUdpOn()
                        .setBufferSize(1024)
                        .setResource("/test", mockHandler)
                        .setResource("/test/{id}", mockHandler)
                        .build()) {
        nextAvailablePort += 2;  // Increment for the next test
    }

    void SetUp() override {
        std::cout << "SetUp: tcpPort=" << tcpPort << ", udpPort=" << udpPort
                  << std::endl;

        // Start the webserver in a separate thread
        webserver.start();
    }

    void TearDown() override {
        webserver.shutdown();
    }

    int tcpPort;
    int udpPort;
    Webserver webserver;

    // Helper function to make HTTP requests using asio
    std::string makeHttpRequest(const std::string& method,
                                const std::string& path,
                                const std::string& body = "") {
        asio::io_context io_context;
        asio::ip::tcp::socket socket(io_context);
        asio::ip::tcp::resolver resolver(io_context);

        try {
            auto endpoints =
                resolver.resolve("localhost", std::to_string(tcpPort));

            asio::connect(socket, endpoints);

            asio::streambuf request_stream;
            std::ostream request(&request_stream);
            request << method << " " << path << " HTTP/1.1\r\n";
            request << "Host: localhost\r\n";
            request << "Content-Length: " << body.length() << "\r\n";
            request << "Connection: close\r\n";
            request << "\r\n";
            request << body;

            asio::write(socket, request_stream);

            asio::streambuf response_stream;
            asio::error_code error;
            asio::read_until(socket, response_stream, "\r\n\r\n", error);

            if (error) {
                std::cerr << "makeHttpRequest: Error during read_until: "
                          << error.message() << std::endl;
                return "ERROR: " + error.message();
            }

            std::istream response(&response_stream);
            std::string http_version;
            int status_code;
            std::string status_message;
            response >> http_version >> status_code;
            std::getline(response, status_message);
            // Remove leading space from status message
            if (!status_message.empty() && status_message[0] == ' ') {
                status_message = status_message.substr(1);
            }

            // Reconstruct headers
            std::stringstream header_stream;
            std::string header_line;
            while (std::getline(response, header_line) && header_line != "\r") {
                header_stream << header_line << "\n";
            }
            std::string headers = header_stream.str();

            std::stringstream body_stream;
            body_stream << response.rdbuf();
            std::string response_body = body_stream.str();

            return http_version + " " + std::to_string(status_code) + " " +
                   status_message + "\n" + headers + "\r\n" + response_body;

        } catch (std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            return "ERROR: " + std::string(e.what());
        }

        return "";
    }

private:
    static int nextAvailablePort;  // Static member to track the port range
};

int WebserverTest::nextAvailablePort = 8090;  // Start range at 8090

TEST_F(WebserverTest, TestHttpGet) {
    std::string expectedResponse =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
        "12\r\n\r\nGET Response";
    std::string response = makeHttpRequest("GET", "/test");
    ASSERT_EQ(response, expectedResponse);
}

TEST_F(WebserverTest, TestHttpPost) {
    std::string expectedResponse =
        "HTTP/1.1 201 Created\r\nContent-Type: text/plain\r\nContent-Length: "
        "24\r\n\r\nPOST Response: Test Body";
    std::string response = makeHttpRequest("POST", "/test", "Test Body");
    ASSERT_EQ(response, expectedResponse);
}

TEST_F(WebserverTest, TestHttpPut) {
    std::string expectedResponse =
        "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: "
        "17\r\n\r\nPUT Response: 123";
    std::string response = makeHttpRequest("PUT", "/test/123");
    ASSERT_EQ(response, expectedResponse);
}

TEST_F(WebserverTest, TestHttpDelete) {
    std::string expectedResponse =
        "HTTP/1.1 204 No Content\r\nContent-Type: text/plain\r\n\r\n";
    std::string response = makeHttpRequest("DELETE", "/test/123");
    ASSERT_EQ(response, expectedResponse);
}

TEST_F(WebserverTest, TestHttpGetNotFound) {
    std::string expectedResponse =
        "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: "
        "9\r\n\r\nNot Found";
    std::string response = makeHttpRequest("GET", "/nonexistent");
    ASSERT_EQ(response, expectedResponse);
}

}  // namespace ioteye