#include "json.h"
#include <doctest/doctest.h>
#include <math.h>
#include <cstring>

TEST_CASE("JsonPrettyPrintTest - ParseAndFormatObject")
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
    CHECK_FALSE(result.is_array());

    // Test string representations of object values
    CHECK_EQ(strcmp(result.get("number").c_str(), "123.456"), 0);
    CHECK_EQ(strcmp(result.get("string").c_str(), "\"hello \\\" world\""), 0);
    CHECK_EQ(strcmp(result.get("array").c_str(), "[1,2,3]"), 0);
    CHECK_EQ(strcmp(result.get("boolean").c_str(), "true"), 0);
    CHECK_EQ(strcmp(result.get("isnull").c_str(), "null"), 0);
    CHECK_EQ(strcmp(result.get("objarray").c_str(), "[{\"key\":\"value\"}]"), 0);
    CHECK_EQ(strcmp(result.get("strarray").c_str(), "[\"hello\",\"world\"]"), 0);
    CHECK_EQ(strcmp(result.get("emptyarray").c_str(), "[]"), 0);

    // Test key existence
    CHECK(result.has_key("number"));
    CHECK_FALSE(result.has_key("nokey"));

    // Test nested object access
    CHECK_EQ(strcmp(result["objarray"].array(0).get("key").as_string().c_str(), "value"), 0);

    // Test string array conversion
    std::vector<std::string> strarray = result["strarray"];
    CHECK_EQ(strarray.size(), 2);
    CHECK_EQ(strarray[0], "hello");
    CHECK_EQ(strarray[1], "world");

    // Test empty array
    std::vector<std::string> emptyarray = result["emptyarray"];
    CHECK_EQ(emptyarray.size(), 0);

    // Test pretty print serialization
    CHECK_EQ(strcmp(result.pretty().c_str(), input), 0);

    // Test copy constructor
    json::jobject copy(result);
    CHECK_EQ(strcmp(copy.as_string().c_str(), result.as_string().c_str()), 0);
}
