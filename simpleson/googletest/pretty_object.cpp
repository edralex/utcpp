#include "json.h"
#include <gtest/gtest.h>
#include <math.h>

TEST(JsonPrettyPrintTest, ParseAndFormatObject)
{
    const char *input =
       "{\n"
           "\t\"number\": 123.456,\n"
           "\t\"string\": \"hello \\\" world\",\n"
           "\t\"array\": [\n"
                "\t\t1,\n"
                "\t\t2,\n"
                "\t\t3\n"
            "\t],\n"
           "\t\"boolean\": true,\n"
           "\t\"isnull\": null,\n"
           "\t\"objarray\": [\n"
                "\t\t{\n"
                    "\t\t\t\"key\": \"value\"\n"
                "\t\t}\n"
            "\t],\n"
           "\t\"strarray\": [\n"
                "\t\t\"hello\",\n"
                "\t\t\"world\"\n"
            "\t],\n"
           "\t\"emptyarray\": []\n"
       "}";

    json::jobject result = json::jobject::parse(input);

    // Test object type
    ASSERT_FALSE(result.is_array());

    // Test string representations of object values
    ASSERT_STREQ(result.get("number").c_str(), "123.456");
    ASSERT_STREQ(result.get("string").c_str(), "\"hello \\\" world\"");
    ASSERT_STREQ(result.get("array").c_str(), "[1,2,3]");
    ASSERT_STREQ(result.get("boolean").c_str(), "true");
    ASSERT_STREQ(result.get("isnull").c_str(), "null");
    ASSERT_STREQ(result.get("objarray").c_str(), "[{\"key\":\"value\"}]");
    ASSERT_STREQ(result.get("strarray").c_str(), "[\"hello\",\"world\"]");
    ASSERT_STREQ(result.get("emptyarray").c_str(), "[]");

    // Test key existence
    ASSERT_TRUE(result.has_key("number"));
    ASSERT_FALSE(result.has_key("nokey"));

    // Test nested object access
    ASSERT_STREQ(result["objarray"].array(0).get("key").as_string().c_str(), "value");

    // Test string array conversion
    std::vector<std::string> strarray = result["strarray"];
    ASSERT_EQ(strarray.size(), 2);
    ASSERT_STREQ(strarray[0].c_str(), "hello");
    ASSERT_STREQ(strarray[1].c_str(), "world");

    // Test empty array
    std::vector<std::string> emptyarray = result["emptyarray"];
    ASSERT_EQ(emptyarray.size(), 0);

    // Test pretty print serialization
    ASSERT_STREQ(result.pretty().c_str(), input);

    // Test copy constructor
    json::jobject copy(result);
    ASSERT_STREQ(copy.as_string().c_str(), result.as_string().c_str());
}
