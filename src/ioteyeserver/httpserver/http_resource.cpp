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

#include "ioteyeserver/httpserver/http_resource.hpp"

namespace ioteye {
HttpResource::HttpResource(std::shared_ptr<HttpResourceHandler> handler,
                           std::string uri)
    : m_handler(handler), m_uri(uri) {
    m_isValid = createRegexFromURI(uri);
    m_allowedMethods = {{HttpMethod::HTTP_GET, true},
                        {HttpMethod::HTTP_POST, true},
                        {HttpMethod::HTTP_PUT, true},
                        {HttpMethod::HTTP_DELETE, true}};
}

std::shared_ptr<HttpResponse> HttpResourceHandler::render(
    const HttpRequest& req) {
    return empty_render(req);
}
std::shared_ptr<HttpResponse> HttpResourceHandler::renderPOST(
    const HttpRequest& req) {
    debug::log("Default POST render called!");
    return render(req);
}
std::shared_ptr<HttpResponse> HttpResourceHandler::renderGET(
    const HttpRequest& req) {
    debug::log("Default GET render called!");
    return render(req);
}
std::shared_ptr<HttpResponse> HttpResourceHandler::renderPUT(
    const HttpRequest& req) {
    debug::log("Default PUT render called!");
    return render(req);
}
std::shared_ptr<HttpResponse> HttpResourceHandler::renderDELETE(
    const HttpRequest& req) {
    debug::log("Default DELETE render called!");
    return render(req);
}

std::shared_ptr<HttpResponse> HttpResourceHandler::empty_render(
    const HttpRequest& req) {
    (void)req;  // Suppresses unused parameter warning
    return std::make_shared<HttpResponse>();
}

void HttpResource::setAllowing(HttpMethod_t httpMethod, bool allowed) {
    if (m_allowedMethods.empty()) {
        debug::log("setAllowing: m_allowedMethods is empty!");
        return;
    }
    auto method = m_allowedMethods.find(httpMethod);
    if (method == m_allowedMethods.end()) {
        debug::log("setAllowing: Method ", util::httpMethodToString(httpMethod),
                   " not found!");
    } else {
        method->second = allowed;
        debug::log("setAllowing: Method ", util::httpMethodToString(httpMethod),
                   " is now allowed: ", allowed ? "True" : "False");
    }
}

void HttpResource::disallowAll() {
    for (auto it = m_allowedMethods.begin(); it != m_allowedMethods.end();
         ++it) {
        it->second = false;
    }
    debug::log("disallowAll: all methods are disallowed!");
}

void HttpResource::allowAll() {
    for (auto it = m_allowedMethods.begin(); it != m_allowedMethods.end();
         ++it) {
        it->second = true;
    }
    debug::log("allowAll: all methods are allowed!");
}

bool HttpResource::isAllowed(HttpMethod_t httpMethod) {
    if (m_allowedMethods.empty()) {
        debug::log("isAllowed: m_allowedMethods is empty!");
        return false;
    }
    auto method = m_allowedMethods.find(httpMethod);
    if (method == m_allowedMethods.end()) {
        return false;
    } else {
        return method->second;
    }
}

std::string HttpResource::getAllowedMethodsAsString() {
    std::string allowedMethods;
    for (auto it = m_allowedMethods.cbegin(); it != m_allowedMethods.end();
         ++it) {
        if (!allowedMethods.empty())
            allowedMethods += ", ";
        if (it->second) {
            if (it->first > 0 && it->first < HttpMethod::HTTP_METHOD_MAX)
                allowedMethods += (util::httpMethodToString(it->first));
        }
    }
    return allowedMethods;
}

std::vector<HttpMethod> HttpResource::getAllowedMethods() {
    std::vector<HttpMethod> allowedMethods;
    for (auto it = m_allowedMethods.cbegin(); it != m_allowedMethods.end();
         ++it) {
        if (it->second) {
            if (it->first > 0 && it->first < HttpMethod::HTTP_METHOD_MAX)
                allowedMethods.push_back(static_cast<HttpMethod>(it->first));
        }
    }
    return allowedMethods;
}

bool HttpResource::createRegexFromURI(std::string uri) {
    std::string regexPattern = uri;
    m_paramNames.clear();
    size_t pos = regexPattern.find("{");
    while (pos != std::string::npos) {
        size_t endPos = regexPattern.find("}", pos);
        if (endPos == std::string::npos) {
            debug::log("Error: Invalid route pattern detected '", uri,
                       ". Missing closing brace. This resource will be marked "
                       "as invalid!");
            return false;
        }
        std::string paramName = regexPattern.substr(pos + 1, endPos - pos - 1);
        m_paramNames.push_back(paramName);
        regexPattern.replace(pos, endPos - pos + 1, "([^/]+)");
        pos = regexPattern.find("{", pos + 6);
    }
    m_regexUri = std::regex(regexPattern);
    return true;
}

}  // namespace ioteye
