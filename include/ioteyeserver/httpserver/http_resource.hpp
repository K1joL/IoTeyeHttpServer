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

#ifndef IOTEYE_HTTP_RESOURCE_HPP
#define IOTEYE_HTTP_RESOURCE_HPP

#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "ioteyeserver/httpserver/http_request.hpp"
#include "ioteyeserver/httpserver/http_response.hpp"
#include "ioteyeserver/logging.hpp"
#include "ioteyeserver/types.hpp"
#include "ioteyeserver/utils.hpp"

namespace ioteye {
class HttpResourceHandler {
public:
    virtual ~HttpResourceHandler() = default;
    virtual std::shared_ptr<HttpResponse> render(const HttpRequest& req);
    virtual std::shared_ptr<HttpResponse> renderPOST(const HttpRequest& req);
    virtual std::shared_ptr<HttpResponse> renderGET(const HttpRequest& req);
    virtual std::shared_ptr<HttpResponse> renderPUT(const HttpRequest& req);
    virtual std::shared_ptr<HttpResponse> renderDELETE(const HttpRequest& req);

private:
    std::shared_ptr<HttpResponse> empty_render(const HttpRequest& req);
};

class HttpResource {
public:
    HttpResource(std::shared_ptr<HttpResourceHandler> handler, std::string uri);
    ~HttpResource() = default;
    std::shared_ptr<HttpResourceHandler> getHandler() const {
        return m_handler;
    }
    std::string getUri() const {
        return m_uri;
    }
    std::regex getRegexUri() const {
        return m_regexUri;
    }
    std::vector<std::string> getParamNames() const {
        return m_paramNames;
    }
    bool getIsValid() const {
        return m_isValid;
    }
    void setAllowing(HttpMethod_t httpMethod, bool allowed);
    void disallowAll();
    void allowAll();
    bool isAllowed(HttpMethod_t httpMethod);
    std::string getAllowedMethodsAsString();
    std::vector<HttpMethod> getAllowedMethods();

private:
    bool createRegexFromURI(std::string uri);

private:
    std::shared_ptr<HttpResourceHandler> m_handler;
    std::string m_uri;
    std::regex m_regexUri;
    std::vector<std::string> m_paramNames;
    bool m_isValid = false;
    std::unordered_map<HttpMethod_t, bool> m_allowedMethods;
};
}  // namespace ioteye

#endif  // IOTEYE_HTTP_RESOURCE_HPP