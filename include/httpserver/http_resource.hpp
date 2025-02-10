#ifndef IOTEYE_HTTP_RESOURCE_HPP
#define IOTEYE_HTTP_RESOURCE_HPP

#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "http_request.hpp"
#include "http_response.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "utils.hpp"

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