#include "utils.hpp"

namespace ioteye::util {
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

HttpMethod stringToHttpMethod(const std::string& httpMethodName) {
    static std::unordered_map<std::string, HttpMethod> stringToHttpMethod_map = {
        {"GET", HttpMethod::GET},
        {"POST", HttpMethod::POST},
        {"PUT", HttpMethod::PUT},
        {"DELETE", HttpMethod::DELETE},
    };
    if (stringToHttpMethod_map.find(httpMethodName) == stringToHttpMethod_map.end())
        return HttpMethod::UNKNOWN;
    else
        return stringToHttpMethod_map[httpMethodName];
}

std::string httpMethodToString(HttpMethod httpMethodCode) {
    static std::unordered_map<HttpMethod, std::string> httpMethodToString_map = {
        {HttpMethod::GET, "GET"},
        {HttpMethod::POST, "POST"},
        {HttpMethod::PUT, "PUT"},
        {HttpMethod::DELETE, "DELETE"},
    };
    if (httpMethodToString_map.find(httpMethodCode) == httpMethodToString_map.end())
        return "UNKNOWN";
    else
        return httpMethodToString_map[httpMethodCode];
}

}  // namespace ioteye::util