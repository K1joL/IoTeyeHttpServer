#include "ioteyeserver/httpserver/webserver.hpp"

namespace ioteye {

Webserver::Webserver(int tcpPort, int udpPort, bool udpOn, ResourceMap resourceMap, size_t bufferSize)
    : m_tcpPort(tcpPort),
      m_udpPort(udpPort),
      m_resourceMap(std::move(resourceMap)),
      m_tcpAcceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), tcpPort)),
      m_isUdpOn(udpOn),
      m_methodHandlers{
          {HttpMethod::GET, [](std::shared_ptr<HttpResourceHandler> handler,
                               const HttpRequest& request) { return handler->renderGET(request); }},
          {HttpMethod::POST, [](std::shared_ptr<HttpResourceHandler> handler,
                                const HttpRequest& request) { return handler->renderPOST(request); }},
          {HttpMethod::PUT, [](std::shared_ptr<HttpResourceHandler> handler,
                               const HttpRequest& request) { return handler->renderPUT(request); }},
          {HttpMethod::DELETE, [](std::shared_ptr<HttpResourceHandler> handler,
                                  const HttpRequest& request) { return handler->renderDELETE(request); }}},
      m_bufferSize(bufferSize) {
    if (m_isUdpOn)
        m_udpSocket = std::make_shared<asio::ip::udp::socket>(
            m_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), m_udpPort));
    debug::log("Webserver constructed");
}

Webserver::Webserver() : m_tcpAcceptor(m_ioContext) {
    debug::log("Webserver default constructed");
}

Webserver::~Webserver() {
    if (m_keepRunning)
        shutdown();
}

Webserver::Webserver(Webserver&& other) noexcept
    : m_tcpPort(other.m_tcpPort),
      m_udpPort(other.m_udpPort),
      m_resourceMap(std::move(other.m_resourceMap)),
      m_ioContext(),
      m_tcpAcceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 0)),
      m_isUdpOn(other.m_isUdpOn),
      m_remoteEndpoint(),
      m_keepRunning(false),
      m_coutMutex(),
      m_methodHandlers(std::move(other.m_methodHandlers)),
      m_ioContextThread(),
      m_bufferSize(other.m_bufferSize) {
    other.shutdown();
    debug::log("Webserver moved");
    m_tcpAcceptor =
        asio::ip::tcp::acceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), m_tcpPort));
    m_udpSocket = std::make_shared<asio::ip::udp::socket>(
        m_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), m_udpPort));
}

Webserver& Webserver::operator=(Webserver&& other) noexcept {
    debug::log("Webserver moved and assigned");
    if (this == &other)
        return *this;
    this->shutdown();
    other.shutdown();

    m_tcpPort = other.m_tcpPort;
    m_udpPort = other.m_udpPort;
    m_isUdpOn = other.m_isUdpOn;
    m_bufferSize = other.m_bufferSize;
    m_tcpAcceptor =
        asio::ip::tcp::acceptor(m_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), m_tcpPort));
    m_udpSocket = std::make_shared<asio::ip::udp::socket>(
        m_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), m_udpPort));
    m_resourceMap = std::move(other.m_resourceMap);
    m_methodHandlers = std::move(other.m_methodHandlers);
    m_keepRunning = false;
    return *this;
}

void Webserver::start() {
    if (m_keepRunning == true)
        return;
    else
        m_keepRunning = true;
    try {
        {
            std::lock_guard<std::mutex> lock(m_coutMutex);
            debug::log("[TCP] Server is listening on port ", m_tcpPort);
        }
        if (m_isUdpOn) {
            {
                std::lock_guard<std::mutex> lock(m_coutMutex);
                debug::log("[UDP] Server is listening on port ", m_udpPort);
            }
            receiveUdpRequest();
        }
        acceptTcpConnection();
        m_ioContextThread = std::thread([this]() { m_ioContext.run(); });
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void Webserver::shutdown() {
    debug::log("Shutdown called");
    m_keepRunning = false;
    if (!m_ioContext.stopped()) {
        debug::log("Stopping io_context");
        m_ioContext.stop();
    }
    if (m_tcpAcceptor.is_open()) {
        debug::log("Closing tcpAcceptor");
        m_tcpAcceptor.close();
    }
    if (m_udpSocket && m_udpSocket->is_open()) {
        debug::log("Closing udpSocket");
        asio::error_code ec;
        m_udpSocket->close(ec);
        if (ec) {
            std::cerr << "Error closing UDP socket: " << ec.message() << std::endl;
        }
    }
    if (m_ioContextThread.joinable()) {
        debug::log("Joining io_contextThread");
        m_ioContextThread.join();
    }
    debug::log("Shutdown complete");
}

void Webserver::acceptTcpConnection() {
    auto socketPtr = std::make_shared<asio::ip::tcp::socket>(m_ioContext);
    m_tcpAcceptor.async_accept(*socketPtr, [this, socketPtr](const asio::error_code& error) {
        if (!error) {
            {
                std::lock_guard<std::mutex> lock(m_coutMutex);
                debug::log("TCP client connected!");
            }
            handleTcpConnection(socketPtr);
        } else {
            {
                std::lock_guard<std::mutex> lock(m_coutMutex);
                std::cerr << "Error while accepting tcp connection! " << error.message() << std::endl;
            }
        }
        if (m_keepRunning)
            acceptTcpConnection();
    });
}

void Webserver::handleTcpConnection(std::shared_ptr<asio::ip::tcp::socket> socketPtr) {
    {
        std::lock_guard<std::mutex> lock(m_coutMutex);
        debug::log("[TCP] Client handling!");
    }
    auto buffer = std::make_shared<std::vector<char>>(m_bufferSize);
    socketPtr->async_read_some(
        asio::buffer(*buffer), [this, socketPtr, buffer](const asio::error_code& error, std::size_t length) {
            if (!error) {
                std::string requestData(buffer->begin(), buffer->begin() + length);
                {
                    std::lock_guard<std::mutex> lock(m_coutMutex);
                    debug::log("[TCP] request data: ", requestData);
                }
                handleRequestData(requestData, [socketPtr](const ioteye::HttpResponse& response) {
                    sendTcpResponse(response, socketPtr);
                });
            } else {
                std::lock_guard<std::mutex> lock(m_coutMutex);
                std::cerr << "[TCP] Error receiving request: " << error.message() << std::endl;
            }
        });
}

void Webserver::receiveUdpRequest() {
    auto buffer = std::make_shared<std::vector<char>>(m_bufferSize);
    m_udpSocket->async_receive_from(
        asio::buffer(*buffer), m_remoteEndpoint,
        [this, buffer](const asio::error_code& error, std::size_t length) mutable {
            {
                std::lock_guard<std::mutex> lock(m_coutMutex);
                debug::log("[UDP] Client handling!");
            }
            if (!error) {
                std::string requestData(buffer->begin(), buffer->begin() + length);
                {
                    std::lock_guard<std::mutex> lock(m_coutMutex);
                    debug::log("[UDP] request data: ", requestData);
                }
                handleRequestData(requestData, [this](const ioteye::HttpResponse& response) mutable {
                    sendUdpResponse(response, m_udpSocket, m_remoteEndpoint);
                });
            } else {
                {
                    std::lock_guard<std::mutex> lock(m_coutMutex);
                    std::cerr << "[UDP] Error receiving request: " << error.message() << std::endl;
                }
            }
            receiveUdpRequest();
        });
}

void Webserver::handleRequestData(const std::string& requestData,
                                  std::function<void(const ioteye::HttpResponse&)> sendResponse) {
    HttpRequest request;
    size_t firstLineEnd = 0;
    if ((firstLineEnd = requestData.find("\r\n")) == std::string::npos) {
        debug::log("Error, first line not found");
        sendResponse(*createBadRequestResponse());
        return;
    }
    std::string firstLine = requestData.substr(0, firstLineEnd);
    std::string requestPart = requestData.substr(firstLineEnd + 2);
    // Get the Method, URI and Protocol from the first line
    std::vector<std::string> parts = util::splitString(firstLine, ' ');
    if (parts.size() < 3) {
        debug::log("Incorrect first line");
        sendResponse(*createBadRequestResponse());
        return;
    }
    request.setMethod(util::stringToHttpMethod(parts[0]));
    if (parts[1].back() == '/')
        parts[1].pop_back();
    request.setUri(parts[1]);
    // Split Parts into header and body
    size_t headerEnd = requestPart.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        request.setHeaders(requestPart);
    } else {
        request.setHeaders(requestPart.substr(0, headerEnd));
        request.setBody(requestPart.substr(headerEnd + 4));
    }
    // Match to resources
    for (const auto& [pattern, resource] : m_resourceMap) {
        debug::log("Pattern: ", pattern, " request: ", request.getUri());
        if (matchUrl(request, resource)) {
            std::shared_ptr<HttpResponse> response = handleRequest(request, pattern);
            sendResponse(*response);
            return;
        }
    }
    sendResponse(*createNotFoundResponse());
}

std::string Webserver::getHeaderValue(const std::string& headers, const std::string& headerName) {
    std::string lowerHeader = headerName;
    std::transform(lowerHeader.begin(), lowerHeader.end(), lowerHeader.begin(), ::tolower);
    size_t pos = headers.find(lowerHeader + ": ");
    if (pos == std::string::npos)
        return "";
    pos += lowerHeader.length() + 2;  // Skip past the header and colon
    size_t endPos = headers.find("\r\n", pos);
    if (endPos == std::string::npos)
        return headers.substr(pos);  // Return the rest if no end of line is found
    return headers.substr(pos, endPos - pos);
}

std::shared_ptr<HttpResponse> Webserver::handleRequest(const HttpRequest& request,
                                                       const std::string& uriPattern) {
    debug::log("handleRequest: uriPattern = ", uriPattern);

    auto resourceIt = m_resourceMap.find(uriPattern);
    if (resourceIt == m_resourceMap.end()) {
        debug::log("handleRequest: Resource not found");
        return createNotFoundResponse();
    }
    auto resource = resourceIt->second;
    auto methodHandlerIt = m_methodHandlers.find(request.getMethod());
    if (methodHandlerIt == m_methodHandlers.end()) {
        debug::log("handleRequest: Method not allowed");
        return createMethodNotAllowed(resource->getAllowedMethodsAsString());
    }
    if (!resource) {
        debug::log("handleRequest: resource is null!");
        return createBadRequestResponse();
    }

    auto handler = resource->getHandler();
    if (!handler) {
        debug::log("handleRequest: handler is null!");
        return createBadRequestResponse();
    }

    try {
        return methodHandlerIt->second(handler, request);
    } catch (const std::exception& e) {
        std::cerr << "handleRequest: Exception in method handler: " << e.what() << std::endl;
        return createBadRequestResponse();
    } catch (...) {
        std::cerr << "handleRequest: Unknown exception in method handler" << std::endl;
        return createBadRequestResponse();
    }
}

bool Webserver::matchUrl(HttpRequest& request, const std::shared_ptr<HttpResource> resource) {
    if (!resource->getIsValid())
        return false;
    std::smatch matches;
    if (std::regex_match(request.getUri(), matches, resource->getRegexUri())) {
        // Extract parameters by looking at each submatch after the initial match
        for (size_t i = 1; i < matches.size(); ++i)
            if (i <= resource->getParamNames().size())
                request.setArg(resource->getParamNames()[i - 1], matches[i].str());

        return true;
    }
    return false;
}

Webserver::Builder& Webserver::Builder::setTcpPort(int port) {
    this->m_tcpPort = port;
    return *this;
}

Webserver::Builder& Webserver::Builder::setUdpPort(int port) {
    this->m_udpPort = port;
    return *this;
}
Webserver::Builder& Webserver::Builder::setUdpOn() {
    this->m_isUdpOn = true;
    return *this;
}

Webserver::Builder& Webserver::Builder::setBufferSize(size_t size) {
    this->m_bufferSize = size;
    return *this;
}
Webserver::Builder& Webserver::Builder::setResource(const std::string& path,
                                                    std::shared_ptr<HttpResourceHandler> resourceHandler) {
    m_resourceMap[path] = std::make_shared<HttpResource>(resourceHandler, path);
    return *this;
}

Webserver::Builder& Webserver::Builder::setResource(std::shared_ptr<HttpResource> resource) {
    m_resourceMap[resource->getUri()] = resource;
    return *this;
}

Webserver Webserver::Builder::build() {
    return Webserver(m_tcpPort, m_udpPort, m_isUdpOn, std::move(m_resourceMap), m_bufferSize);
}

}  // namespace ioteye
