#include <asterkv/core/result.h>
#include <asterkv/core/status.h>

#include <gtest/gtest.h>

#include <string>

namespace {
    TEST(StatusTest, DefaultStatusIsOk) {
    const AsterKV::Core::Status status;

    EXPECT_TRUE(status.isOk());
    EXPECT_EQ(AsterKV::Core::ErrorCode::Ok, status.code());
    EXPECT_EQ("ok", status.codeString());
    EXPECT_TRUE(status.message().empty());
}

TEST(StatusTest, CreatesInvalidArgumentStatus) {
    const AsterKV::Core::Status status =
        AsterKV::Core::Status::invalidArgument("invalid key");

    EXPECT_FALSE(status.isOk());
    EXPECT_EQ(AsterKV::Core::ErrorCode::InvalidArgument, status.code());
    EXPECT_EQ("invalid_argument", status.codeString());
    EXPECT_EQ("invalid key", status.message());
}

TEST(StatusTest, ConvertsErrorCodesToStrings) {
    EXPECT_EQ("ok", AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::Ok));
    EXPECT_EQ("unknown", AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::Unknown));
    EXPECT_EQ("invalid_argument",
              AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::InvalidArgument));
    EXPECT_EQ("not_found", AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::NotFound));
    EXPECT_EQ("already_exists",
              AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::AlreadyExists));
    EXPECT_EQ("conflict", AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::Conflict));
    EXPECT_EQ("unavailable",
              AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::Unavailable));
    EXPECT_EQ("internal", AsterKV::Core::errorCodeToString(AsterKV::Core::ErrorCode::Internal));
}

TEST(ResultTest, CreatesSuccessfulResult) {
    auto result = AsterKV::Core::Result<std::string>::success("value");

    ASSERT_TRUE(result.isOk());
    EXPECT_FALSE(result.isError());
    EXPECT_EQ("value", result.value());
    ASSERT_NE(nullptr, result.valuePtr());
    EXPECT_EQ("value", *result.valuePtr());
    EXPECT_TRUE(result.status().isOk());
}

TEST(ResultTest, CreatesFailedResult) {
    auto result = AsterKV::Core::Result<int>::failure(
        AsterKV::Core::Status::notFound("key not found"));

    EXPECT_FALSE(result.isOk());
    EXPECT_TRUE(result.isError());
    EXPECT_EQ(nullptr, result.valuePtr());

    const AsterKV::Core::Status status = result.status();

    EXPECT_EQ(AsterKV::Core::ErrorCode::NotFound, status.code());
    EXPECT_EQ("not_found", status.codeString());
    EXPECT_EQ("key not found", status.message());
}

TEST(ResultTest, ConvertsOkFailureStatusToInternalError) {
    auto result = AsterKV::Core::Result<int>::failure(AsterKV::Core::Status::ok());

    EXPECT_FALSE(result.isOk());

    const AsterKV::Core::Status status = result.status();

    EXPECT_EQ(AsterKV::Core::ErrorCode::Internal, status.code());
    EXPECT_EQ("internal", status.codeString());
    EXPECT_EQ("Result failure cannot be constructed from OK status.", status.message());
}
}
