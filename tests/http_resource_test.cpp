#include <gtest/gtest.h>

#include <ioteyeserver.hpp>

class TestHandler : public ioteye::HttpResourceHandler {
public:
    std::shared_ptr<ioteye::HttpResponse> render(
        const ioteye::HttpRequest& req) override {
        (void)req;
        auto resp = std::make_shared<ioteye::HttpResponse>();
        resp->setBody("TestHandler response");
        return resp;
    }
};

TEST(HttpResourceTest, DefaultMethodAllowance) {
    auto handler = std::make_shared<TestHandler>();
    ioteye::HttpResource resource(handler, "/test");

    EXPECT_TRUE(resource.isAllowed(ioteye::HttpMethod::HTTP_GET));
    EXPECT_TRUE(resource.isAllowed(ioteye::HttpMethod::HTTP_POST));
}

TEST(HttpResourceTest, MethodDisallowing) {
    auto handler = std::make_shared<TestHandler>();
    ioteye::HttpResource resource(handler, "/test");

    resource.setAllowing(ioteye::HttpMethod::HTTP_POST, false);
    EXPECT_FALSE(resource.isAllowed(ioteye::HttpMethod::HTTP_POST));
}

TEST(HttpResourceTest, DisallowAll) {
    auto handler = std::make_shared<TestHandler>();
    ioteye::HttpResource resource(handler, "/test");

    resource.disallowAll();
    EXPECT_FALSE(resource.isAllowed(ioteye::HttpMethod::HTTP_GET));
}

TEST(HttpResourceTest, ValidURI) {
    auto handler = std::make_shared<TestHandler>();
    ioteye::HttpResource resource(handler, "/user/{id}/profile");

    EXPECT_TRUE(resource.getIsValid());
    EXPECT_EQ(resource.getParamNames().size(), 1);
    EXPECT_EQ(resource.getParamNames()[0], "id");
}

TEST(HttpResourceTest, InvalidURI) {
    auto handler = std::make_shared<TestHandler>();
    ioteye::HttpResource resource(handler, "/user/{id");

    EXPECT_FALSE(resource.getIsValid());
}

TEST(HttpResourceTest, RenderRequest){
    auto handler = std::make_shared<TestHandler>();
    ioteye::HttpResource resource(handler, "/user/{id");
    auto response = resource.getHandler()->render(ioteye::HttpRequest());
    EXPECT_EQ(response->getBody(), "TestHandler response");
}