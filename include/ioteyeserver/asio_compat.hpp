#pragma once

#ifdef IOTEYE_USE_BOOST_ASIO

#include <boost/asio.hpp>
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>

namespace asio {
using namespace boost::asio;
using error_code = boost::system::error_code;
}  // namespace asio

#else

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#endif