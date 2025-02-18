// logging.h
#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <sstream>
namespace ioteye::debug {
#ifdef ENABLE_LOGGING
template <typename... Args>
inline void log(Args&&... args) {
    std::ostringstream oss;
    (oss << ... << std::forward<Args>(args));
    std::cout << "LOG: " << oss.str() << std::endl;
}
#else
template <typename... Args>
inline void log(Args&&... args) {
    // Dummy code to prevent unused parameter warning
    (void)std::initializer_list<int>{(std::forward<Args>(args), 0)...};
}
#endif
}  // namespace ioteye::debug
#endif  // LOGGING_H