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

#ifndef IOTEYE_WEBSERVER_HPP
#define IOTEYE_WEBSERVER_HPP

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <atomic>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "ioteyeserver/httpserver/http_request.hpp"
#include "ioteyeserver/httpserver/http_resource.hpp"
#include "ioteyeserver/httpserver/http_response.hpp"
#include "ioteyeserver/logging.hpp"
#include "ioteyeserver/types.hpp"
#include "ioteyeserver/utils.hpp"

namespace ioteye {
using ResourceMap =
    std::unordered_map<std::string, std::shared_ptr<HttpResource>>;
using MethodHandler = std::function<std::shared_ptr<HttpResponse>(
    std::shared_ptr<HttpResourceHandler>, const HttpRequest&)>;
class Webserver {
public:
    Webserver();
    ~Webserver();

    Webserver(const Webserver&) = delete;
    Webserver& operator=(const Webserver&) = delete;

    Webserver(Webserver&& other) noexcept;
    Webserver& operator=(Webserver&& other) noexcept;

    void start();
    void shutdown();

    class Builder {
    public:
        Builder();
        Builder& setTcpPort(int port);
        Builder& setUdpPort(int port);
        Builder& setUdpOn();
        Builder& setBufferSize(size_t size);
        Builder& setResource(
            const std::string& path,
            std::shared_ptr<HttpResourceHandler> resourceHandler,
            const std::string& description = "");
        Builder& setResource(std::shared_ptr<HttpResource> resource);
        Builder& setCustomInit(const std::string& path,
                               std::shared_ptr<InitHandler> initHandler);
        Builder& setCustomInit(std::shared_ptr<HttpResource> initResource);
        Webserver build();

    private:
        int m_tcpPort = 8080;  // Default port
        int m_udpPort = 8081;  // Default port
        bool m_isUdpOn = false;
        size_t m_bufferSize = 1024;
        size_t m_threadCount = 1;
        std::shared_ptr<ResourceMap> m_resourceMap;
        std::shared_ptr<HttpResource> m_initResource;
    };

private:
    Webserver(int tcpPort, int udpPort, bool udpOn,
              std::shared_ptr<ResourceMap> resourceMap, size_t bufferSize);

    void acceptTcpConnection();
    void handleTcpConnection(std::shared_ptr<asio::ip::tcp::socket> socketPtr);

    void receiveUdpRequest();
    void handleRequestData(
        const std::string& requestData,
        std::function<void(const ioteye::HttpResponse&)> sendResponse);
    std::string getHeaderValue(const std::string& headers,
                               const std::string& headerName);
    std::shared_ptr<HttpResponse> handleRequest(const HttpRequest& request,
                                                const std::string& uriPattern);
    bool matchUrl(HttpRequest& request,
                  const std::shared_ptr<HttpResource> resource);

private:
    int m_tcpPort = 8080;  // Default tcp port
    int m_udpPort = 8081;  // Default udp port
    std::shared_ptr<ResourceMap> m_resourceMap;
    // TCP Socket
    asio::io_context m_ioContext;
    asio::ip::tcp::acceptor m_tcpAcceptor;
    // UDP Socket
    bool m_isUdpOn = false;
    std::shared_ptr<asio::ip::udp::socket> m_udpSocket;
    asio::ip::udp::endpoint m_remoteEndpoint;
    // Server utils
    std::atomic<bool> m_keepRunning{false};
    std::mutex m_coutMutex;
    std::unordered_map<HttpMethod, MethodHandler> m_methodHandlers;
    std::thread m_ioContextThread;
    size_t m_bufferSize = 1024;
};
}  // namespace ioteye

#endif  // IOTEYE_WEBSERVER_H