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
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "http_request.hpp"
#include "http_resource.hpp"
#include "http_response.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace ioteye {
using ResourceMap = std::unordered_map<std::string, std::shared_ptr<HttpResource>>;
using MethodHandler =
    std::function<std::shared_ptr<HttpResponse>(std::shared_ptr<HttpResourceHandler>, const HttpRequest&)>;
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
        Builder() = default;
        Builder& setTcpPort(int port);
        Builder& setUdpPort(int port);
        Builder& setUdpOn();
        Builder& setBufferSize(size_t size);
        Builder& setResource(const std::string& path, std::shared_ptr<HttpResourceHandler> resourceHandler);
        Builder& setResource(std::shared_ptr<HttpResource> resource);
        Webserver build();

    private:
        int m_tcpPort = 8080;  // Default port
        int m_udpPort = 8081;  // Default port
        bool m_isUdpOn = false;
        size_t m_bufferSize = 1024;
        size_t m_threadCount = 1;
        ResourceMap m_resourceMap;
    };

private:
    Webserver(int tcpPort, int udpPort, bool udpOn, ResourceMap resourceMap, size_t bufferSize);

    void acceptTcpConnection();
    void handleTcpConnection(std::shared_ptr<asio::ip::tcp::socket> socketPtr);

    void receiveUdpRequest();
    void handleRequestData(const std::string& requestData,
                           std::function<void(const ioteye::HttpResponse&)> sendResponse);
    std::string getHeaderValue(const std::string& headers, const std::string& headerName);
    std::shared_ptr<HttpResponse> handleRequest(const HttpRequest& request, const std::string& uriPattern);
    bool matchUrl(HttpRequest& request, const std::shared_ptr<HttpResource> resource);

private:
    int m_tcpPort = 8080;  // Default tcp port
    int m_udpPort = 8081;  // Default udp port
    ResourceMap m_resourceMap;
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