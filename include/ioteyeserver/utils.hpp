#ifndef IOTEYE_UTILS_HPP
#define IOTEYE_UTILS_HPP

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "types.hpp"

namespace ioteye::util {
std::vector<std::string> splitString(const std::string& str, char delimiter);

HttpMethod stringToHttpMethod(const std::string& httpMethodName);
std::string httpMethodToString(HttpMethod httpMethodCode);

}  // namespace ioteye::util

#endif  // IOTEYE_UTILS_HPP