#ifndef IOTEYE_HTTP_RESPONSE_HPP
#define IOTEYE_HTTP_RESPONSE_HPP

#include <asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "http_status_codes.hpp"
#include "logging.hpp"

namespace ioteye {
class HttpResponse {
public:
    HttpResponse(int statusCode = HttpStatusCode::OK, const std::string& body = "",
                 const std::unordered_map<std::string, std::string>& headers = {});
    std::string getBody() const;
    std::string getHeader(const std::string& key) const;
    int getStatusCode() const;
    std::string toString() const;
    void setHeader(const std::string& key, const std::string& value);
    void setContentType(const std::string& contentType);
    void setBody(const std::string& body);
    void addBody(const std::string& body);
    void setStatusCode(int statusCode);

private:
    int m_statusCode;
    std::string m_body;
    std::unordered_map<std::string, std::string> m_headers;
};
std::shared_ptr<HttpResponse> createBadRequestResponse();
std::shared_ptr<HttpResponse> createNotFoundResponse();
std::shared_ptr<HttpResponse> createMethodNotAllowed(const std::string& allowedMethods);
void sendUdpResponse(const HttpResponse& response, std::shared_ptr<asio::ip::udp::socket> socket,
                     asio::ip::udp::endpoint& destination);
void sendTcpResponse(const HttpResponse& response, std::shared_ptr<asio::ip::tcp::socket> socket);
}  // namespace ioteye

#endif  // IOTEYE_HTTP_RESPONSE_HPP