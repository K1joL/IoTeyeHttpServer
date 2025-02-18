#ifndef IOTEYE_HTTP_REQUEST_HPP
#define IOTEYE_HTTP_REQUEST_HPP

#include <string>
#include <unordered_map>

#include "ioteyeserver/types.hpp"

namespace ioteye {
class HttpRequest {
public:
    HttpRequest() = default;

    std::string getArg(const std::string& arg_name) const;
    void setArg(const std::string& argName, const std::string& argValue) {
        m_args[argName] = argValue;
    }
    // Getters
    HttpMethod getMethod() const;
    const std::string& getUri() const;
    const std::unordered_map<std::string, std::string>& getArgs() const;
    const std::string& getHeaders() const;
    const std::string& getBody() const;

    // Setters
    void setMethod(HttpMethod method);
    void setUri(const std::string& uri);
    void setArgs(const std::unordered_map<std::string, std::string>& args);
    void setHeaders(const std::string& headers);
    void setBody(const std::string& body);

private:
    HttpMethod m_method = HttpMethod::HTTP_METHOD_MAX;
    std::string m_uri;
    std::unordered_map<std::string, std::string> m_args;
    std::string m_headers;
    std::string m_body;
};
}  // namespace ioteye

#endif  // IOTEYE_HTTP_REQUEST_HPP