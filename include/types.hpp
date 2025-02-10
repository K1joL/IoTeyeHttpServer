#ifndef IOTEYE_TYPES_HPP
#define IOTEYE_TYPES_HPP

#include <string>
#include <unordered_map>

namespace ioteye {

using HttpMethod_t = uint16_t;

enum HttpMethod {
    UNKNOWN,
    GET,
    POST,
    PUT,
    DELETE,
    HTTP_METHOD_MAX,
};

}  // namespace ioteye

#endif  // IOTEYE_TYPES_HPP
