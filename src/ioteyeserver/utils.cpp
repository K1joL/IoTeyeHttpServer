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

#include "ioteyeserver/utils.hpp"

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