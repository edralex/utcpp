#include "json.h"
#include <catch2/catch_test_macros.hpp>
#include <math.h>

TEST_CASE("JsonPrettyPrint parse and format object", "[json][pretty]") {
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
    REQUIRE_FALSE(result.is_array());

    // Test string representations of object values
    REQUIRE(std::string(result.get("number").c_str()) == "123.456");
    REQUIRE(std::string(result.get("string").c_str()) == "\"hello \\\" world\"");
    REQUIRE(std::string(result.get("array").c_str()) == "[1,2,3]");
    REQUIRE(std::string(result.get("boolean").c_str()) == "true");
    REQUIRE(std::string(result.get("isnull").c_str()) == "null");
    REQUIRE(std::string(result.get("objarray").c_str()) == "[{\"key\":\"value\"}]");
    REQUIRE(std::string(result.get("strarray").c_str()) == "[\"hello\",\"world\"]");
    REQUIRE(std::string(result.get("emptyarray").c_str()) == "[]");

    // Test key existence
    REQUIRE(result.has_key("number"));
    REQUIRE_FALSE(result.has_key("nokey"));

    // Test nested object access
    REQUIRE(result["objarray"].array(0).get("key").as_string() == "value");

    // Test string array conversion
    std::vector<std::string> strarray = result["strarray"];
    REQUIRE(strarray.size() == 2);
    REQUIRE(strarray[0] == "hello");
    REQUIRE(strarray[1] == "world");

    // Test empty array
    std::vector<std::string> emptyarray = result["emptyarray"];
    REQUIRE(emptyarray.size() == 0);

    // Test pretty print serialization
    REQUIRE(result.pretty() == std::string(input));

    // Test copy constructor
    json::jobject copy(result);
    REQUIRE(copy.as_string() == result.as_string());
}
