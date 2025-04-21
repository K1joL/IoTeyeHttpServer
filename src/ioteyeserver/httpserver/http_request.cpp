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

#include "ioteyeserver/httpserver/http_request.hpp"
namespace ioteye {

std::string HttpRequest::getArg(const std::string& arg_name) const {
    auto it = m_args.find(arg_name);
    if (it != m_args.end()) {
        return it->second;
    }
    return "";
}

void HttpRequest::clearArgs() {
    m_args.clear();
}

// Getters
HttpMethod HttpRequest::getMethod() const {
    return m_method;
}

const std::string& HttpRequest::getUri() const {
    return m_uri;
}

const std::unordered_map<std::string, std::string>& HttpRequest::getArgs()
    const {
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

void HttpRequest::setArgs(
    const std::unordered_map<std::string, std::string>& args) {
    m_args = args;
}

void HttpRequest::setHeaders(const std::string& headers) {
    m_headers = headers;
}

void HttpRequest::setBody(const std::string& body) {
    m_body = body;
}

}  // namespace ioteye