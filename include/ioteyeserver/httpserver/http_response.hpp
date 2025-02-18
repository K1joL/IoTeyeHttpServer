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

#ifndef IOTEYE_HTTP_RESPONSE_HPP
#define IOTEYE_HTTP_RESPONSE_HPP

#include <asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "ioteyeserver/http_status_codes.hpp"
#include "ioteyeserver/logging.hpp"

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