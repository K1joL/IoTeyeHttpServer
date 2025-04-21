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
    void clearArgs();

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