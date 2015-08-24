#include <gtest/gtest.h>
#include <cpp_redis/builders/bulk_string_builder.hpp>
#include <cpp_redis/redis_error.hpp>

TEST(BulkStringBuilder, WithNoData) {
    cpp_redis::builders::bulk_string_builder builder;

    EXPECT_EQ(false, builder.reply_ready());
}

TEST(BulkStringBuilder, WithNotEnoughData) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhell";
    builder << buffer;

    EXPECT_EQ(false, builder.reply_ready());
}

TEST(BulkStringBuilder, WithPartOfEndSequence) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhello\r";
    builder << buffer;

    EXPECT_EQ(false, builder.reply_ready());
}

TEST(BulkStringBuilder, Null) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "-1\r\n";
    builder << buffer;

    EXPECT_EQ(true, builder.reply_ready());
    EXPECT_EQ("", buffer);

    auto reply = std::dynamic_pointer_cast<cpp_redis::bulk_string_reply>(builder.get_reply());
    EXPECT_TRUE(reply != nullptr);
    EXPECT_EQ(true, reply->is_null());
}

TEST(BulkStringBuilder, WithAllInOneTime) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhello\r\n";
    builder << buffer;

    EXPECT_EQ(true, builder.reply_ready());
    EXPECT_EQ("", buffer);

    auto reply = std::dynamic_pointer_cast<cpp_redis::bulk_string_reply>(builder.get_reply());
    EXPECT_TRUE(reply != nullptr);
    EXPECT_EQ("hello", reply->str());
}

TEST(BulkStringBuilder, WithAllInMultipleTimes) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhell";
    builder << buffer;
    buffer += "o\r\n";
    builder << buffer;

    EXPECT_EQ(true, builder.reply_ready());
    EXPECT_EQ("", buffer);

    auto reply = std::dynamic_pointer_cast<cpp_redis::bulk_string_reply>(builder.get_reply());
    EXPECT_TRUE(reply != nullptr);
    EXPECT_EQ("hello", reply->str());
}

TEST(BulkStringBuilder, WithAllInMultipleTimes2) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhello";
    builder << buffer;
    buffer += "\r\n";
    builder << buffer;

    EXPECT_EQ(true, builder.reply_ready());
    EXPECT_EQ("", buffer);

    auto reply = std::dynamic_pointer_cast<cpp_redis::bulk_string_reply>(builder.get_reply());
    EXPECT_TRUE(reply != nullptr);
    EXPECT_EQ("hello", reply->str());
}

TEST(BulkStringBuilder, WithAllInMultipleTimes3) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhello\r";
    builder << buffer;
    buffer += "\n";
    builder << buffer;

    EXPECT_EQ(true, builder.reply_ready());
    EXPECT_EQ("", buffer);

    auto reply = std::dynamic_pointer_cast<cpp_redis::bulk_string_reply>(builder.get_reply());
    EXPECT_TRUE(reply != nullptr);
    EXPECT_EQ("hello", reply->str());
}

TEST(BulkStringBuilder, InvalidEndSequence) {
    cpp_redis::builders::bulk_string_builder builder;

    std::string buffer = "5\r\nhello\ra";
    EXPECT_THROW(builder << buffer, cpp_redis::redis_error);
}
