#include <gtest/gtest.h>
#include <ioteyeserver.hpp>

using namespace ioteye;

TEST(HttpRequestTest, SetAndGetArg) {
    HttpRequest request;
    request.setArg("key", "value");
    EXPECT_EQ(request.getArg("key"), "value");
}

TEST(HttpRequestTest, ClearArgs) {
    HttpRequest request;
    request.setArg("key1", "value1");
    request.setArg("key2", "value2");
    request.clearArgs();
    EXPECT_TRUE(request.getArgs().empty());
}

TEST(HttpRequestTest, SetAndGetMethod) {
    HttpRequest request;
    request.setMethod(HttpMethod::HTTP_GET);
    EXPECT_EQ(request.getMethod(), HttpMethod::HTTP_GET);
}

TEST(HttpRequestTest, SetAndGetUri) {
    HttpRequest request;
    request.setUri("/test");
    EXPECT_EQ(request.getUri(), "/test");
}

TEST(HttpRequestTest, SetAndGetHeaders) {
    HttpRequest request;
    request.setHeaders("Content-Type: text/html");
    EXPECT_EQ(request.getHeaders(), "Content-Type: text/html");
}

TEST(HttpRequestTest, SetAndGetBody) {
    HttpRequest request;
    request.setBody("Hello, World!");
    EXPECT_EQ(request.getBody(), "Hello, World!");
}