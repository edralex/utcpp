#include "json.h"
#include <gtest/gtest.h>

TEST(JsonTypeDetectionTest, EmptyString)
{
	ASSERT_EQ(json::jtype::detect(""), json::jtype::not_valid);
}

TEST(JsonTypeDetectionTest, String)
{
	ASSERT_EQ(json::jtype::detect(" \"test string\""), json::jtype::jstring);
}

TEST(JsonTypeDetectionTest, Number)
{
	ASSERT_EQ(json::jtype::detect(" 123"), json::jtype::jnumber);
	ASSERT_EQ(json::jtype::detect(" -123"), json::jtype::jnumber);
}

TEST(JsonTypeDetectionTest, Object)
{
	ASSERT_EQ(json::jtype::detect(" {\"hello\":\"world\""), json::jtype::jobject);
}

TEST(JsonTypeDetectionTest, Array)
{
	ASSERT_EQ(json::jtype::detect(" [1,2,3]"), json::jtype::jarray);
}

TEST(JsonTypeDetectionTest, Boolean)
{
	ASSERT_EQ(json::jtype::detect(" true"), json::jtype::jbool);
	ASSERT_EQ(json::jtype::detect(" false"), json::jtype::jbool);
}

TEST(JsonTypeDetectionTest, Null)
{
	ASSERT_EQ(json::jtype::detect(" null"), json::jtype::jnull);
}

TEST(JsonTypeDetectionTest, Invalid)
{
	ASSERT_EQ(json::jtype::detect(" abc"), json::jtype::not_valid);
}
