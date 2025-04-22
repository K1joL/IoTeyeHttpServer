#include <gtest/gtest.h>

#include <ioteyeserver.hpp>

TEST(HttpResponseTest, ConstructsAndFormatsCorrectly) {
    using namespace ioteye;
    // Basic response
    {
        HttpResponse response(200, "OK");
        auto str = response.toString();
        ASSERT_NE(str.find("HTTP/1.1 200 OK"), std::string::npos);
        ASSERT_NE(str.find("Content-Type: text/plain"), std::string::npos);
    }

    // With headers
    {
        HttpResponse response(201, "Created", {{"X-Test", "Value"}});
        auto str = response.toString();
        ASSERT_NE(str.find("X-Test: Value"), std::string::npos);
    }

    // Empty body
    {
        HttpResponse response(204);
        auto str = response.toString();
        ASSERT_EQ(str.find("Content-Length"), std::string::npos);
    }
}

TEST(HttpResponseTest, SettersWorkCorrectly) {
    ioteye::HttpResponse response;

    // Header manipulation
    {
        response.setHeader("X-Test", "Value");
        ASSERT_EQ(response.getHeader("X-Test"), "Value");
    }

    // Content type
    {
        response.setContentType("application/json");
        ASSERT_EQ(response.getHeader("Content-Type"), "application/json");
    }

    // Body manipulation
    {
        response.setBody("test");
        ASSERT_EQ(response.getBody(), "test");

        response.addBody(" append");
        ASSERT_EQ(response.getBody(), "test append");
    }
}
