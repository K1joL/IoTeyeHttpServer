#include "http_response.hpp"

namespace ioteye {

HttpResponse::HttpResponse(int statusCode, const std::string& body,
                           const std::unordered_map<std::string, std::string>& headers)
    : m_statusCode(statusCode), m_body(body), m_headers(headers) {
    if (m_headers.find("Content-Type") == m_headers.end()) {
        m_headers["Content-Type"] = "text/plain";
    }
}

std::string HttpResponse::toString() const {
    std::stringstream ss;
    ss << "HTTP/1.1 " << m_statusCode << " " << util::getStatusMessage(m_statusCode) << "\r\n";
    for (const auto& pair : m_headers) {
        ss << pair.first << ": " << pair.second << "\r\n";
    }
    if (!m_body.empty()) {
        ss << "Content-Length: " << m_body.size() << "\r\n";
        ss << "\r\n";
        ss << m_body;
    } else {
        ss << "\r\n";
    }
    return ss.str();
}

void HttpResponse::setHeader(const std::string& key, const std::string& value) {
    m_headers[key] = value;
}

std::string HttpResponse::getHeader(const std::string& key) const {
    auto it = m_headers.find(key);
    if (it != m_headers.end()) {
        return it->second;
    }
    return "";
}

int HttpResponse::getStatusCode() const {
    return m_statusCode;
}

void HttpResponse::setContentType(const std::string& contentType) {
    m_headers["Content-Type"] = contentType;
}

void HttpResponse::setBody(const std::string& body) {
    m_body = body;
}

void HttpResponse::addBody(const std::string& body) {
    m_body += body;
}

void HttpResponse::setStatusCode(int statusCode) {
    m_statusCode = statusCode;
}

std::string HttpResponse::getBody() const {
    return m_body;
}

std::shared_ptr<HttpResponse> createBadRequestResponse() {
    auto response = std::make_shared<HttpResponse>(HttpStatusCode::BAD_REQUEST, "Bad Request");
    return response;
}
std::shared_ptr<HttpResponse> createNotFoundResponse() {
    auto response = std::make_shared<HttpResponse>(HttpStatusCode::NOT_FOUND, "Not Found");
    return response;
}
std::shared_ptr<HttpResponse> createMethodNotAllowed(const std::string& allowedMethods) {
    auto response = std::make_shared<HttpResponse>(HttpStatusCode::METHOD_NOT_ALLOWED, "Method not allowed");
    response->setHeader("Allow", allowedMethods);
    return response;
}

void sendUdpResponse(const HttpResponse& response, std::shared_ptr<asio::ip::udp::socket> socket,
                     asio::ip::udp::endpoint& destination) {
    socket->async_send_to(asio::buffer(response.toString()), destination,
                          [](const asio::error_code& error, std::size_t bytesTransfered) {
                              if (!error) {
                                  debug::log("[UDP] Response sent successfully. BytesTransfered: ",
                                             bytesTransfered);
                              } else {
                                  debug::log("[UDP] Error sending response: ", error.message());
                              }
                          });
}

void sendTcpResponse(const HttpResponse& response, std::shared_ptr<asio::ip::tcp::socket> socket) {
    asio::async_write(*socket, asio::buffer(response.toString()),
                      [socket](const asio::error_code& error, std::size_t bytesTransfered) {
                          if (!error) {
                              debug::log("[TCP] Response sent successfully. BytesTransfered: ",
                                         bytesTransfered);
                          } else {
                              debug::log("[TCP] Error sending response: ", error.message());
                          }
                      });
}
}  // namespace ioteye