#include <gtest/gtest.h>
#include <ioteyeserver.hpp>

TEST(UtilsTest, SplitStringSplitsCorrectly) {
    using namespace ioteye::util;
    {
        auto result = splitString("a,b,c", ',');
        ASSERT_EQ(result, std::vector<std::string>({"a", "b", "c"}));
    }

    {
        auto result = splitString("", ',');
        EXPECT_TRUE(result.empty());
    }

    {
        auto result = splitString("abc", ',');
        ASSERT_EQ(result, std::vector<std::string>{"abc"});
    }
}
TEST(UtilsTest, StringToHttpMethodConvertsCorrectly) {
    using namespace ioteye;
    {
        EXPECT_EQ(util::stringToHttpMethod("GET"), HttpMethod::HTTP_GET);
        EXPECT_EQ(util::stringToHttpMethod("POST"), HttpMethod::HTTP_POST);
        EXPECT_EQ(util::stringToHttpMethod("PUT"), HttpMethod::HTTP_PUT);
        EXPECT_EQ(util::stringToHttpMethod("DELETE"), HttpMethod::HTTP_DELETE);
    }

    {
        EXPECT_EQ(util::stringToHttpMethod("UNKNOWN"), HttpMethod::UNKNOWN);
        EXPECT_EQ(util::stringToHttpMethod(""), HttpMethod::UNKNOWN);
    }
}

TEST(UtilsTest, HttpMethodToStringConvertsCorrectly) {
    using namespace ioteye;

    {
        EXPECT_EQ(util::httpMethodToString(HttpMethod::HTTP_GET), "GET");
        EXPECT_EQ(util::httpMethodToString(HttpMethod::HTTP_POST), "POST");
        EXPECT_EQ(util::httpMethodToString(HttpMethod::HTTP_PUT), "PUT");
        EXPECT_EQ(util::httpMethodToString(HttpMethod::HTTP_DELETE), "DELETE");
    }

    {
        EXPECT_EQ(util::httpMethodToString(HttpMethod::UNKNOWN), "UNKNOWN");
        EXPECT_EQ(util::httpMethodToString(static_cast<HttpMethod_t>(999)), "UNKNOWN");
    }
}
