#include "ioteyeserver/httpserver/http_request.hpp"
namespace ioteye {

std::string HttpRequest::getArg(const std::string& arg_name) const {
    auto it = m_args.find(arg_name);
    if (it != m_args.end()) {
        return it->second;
    }
    return "";
}

// Getters
HttpMethod HttpRequest::getMethod() const {
    return m_method;
}

const std::string& HttpRequest::getUri() const {
    return m_uri;
}

const std::unordered_map<std::string, std::string>& HttpRequest::getArgs() const {
    return m_args;
}

const std::string& HttpRequest::getHeaders() const {
    return m_headers;
}

const std::string& HttpRequest::getBody() const {
    return m_body;
}

// Setters
void HttpRequest::setMethod(HttpMethod method) {
    m_method = method;
}

void HttpRequest::setUri(const std::string& uri) {
    m_uri = uri;
}

void HttpRequest::setArgs(const std::unordered_map<std::string, std::string>& args) {
    m_args = args;
}

void HttpRequest::setHeaders(const std::string& headers) {
    m_headers = headers;
}

void HttpRequest::setBody(const std::string& body) {
    m_body = body;
}

}  // namespace ioteye