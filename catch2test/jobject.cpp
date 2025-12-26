#include "json.h"
#include <catch2/catch_test_macros.hpp>
#include <math.h>
#include <catch2/catch_approx.hpp>
#include <algorithm>

TEST_CASE("JsonParser parses object with mixed types", "[json]") {
    const char *input =
       "{"
       "  \"number\":123.456,"
       "  \"string\":\"hello \\\" world\","
       "  \"array\":[1,2,3],"
       "  \"boolean\":true,"
       "  \"isnull\":null,"
       "  \"objarray\":[{\"key\":\"value\"}],"
       "  \"strarray\":[\"hello\",\"world\"],"
       "  \"emptyarray\":[]"
       "}";

    json::jobject result = json::jobject::parse(input);

    // Test key list
    json::key_list_t keys = result.list_keys();
    REQUIRE(keys.size() == 8);
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("number")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("string")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("array")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("boolean")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("isnull")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("objarray")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("strarray")) != keys.end());
    REQUIRE(std::find(keys.begin(), keys.end(), std::string("emptyarray")) != keys.end());

    // Test individual entries
    REQUIRE_FALSE(result.is_array());
    REQUIRE(std::string(result.get("number").c_str()) == "123.456");
    REQUIRE(std::string(result.get("string").c_str()) == "\"hello \\\" world\"");
    REQUIRE(std::string(result.get("array").c_str()) == "[1,2,3]");
    REQUIRE(std::string(result.get("boolean").c_str()) == "true");
    REQUIRE(std::string(result.get("isnull").c_str()) == "null");
    REQUIRE(std::string(result.get("objarray").c_str()) == "[{\"key\":\"value\"}]");
    REQUIRE(std::string(result.get("strarray").c_str()) == "[\"hello\",\"world\"]");
    REQUIRE(std::string(result.get("emptyarray").c_str()) == "[]");
    REQUIRE(result.has_key("number"));
    REQUIRE_FALSE(result.has_key("nokey"));
    REQUIRE(result["objarray"].array(0).get("key").as_string() == "value");
    std::vector<std::string> strarray = result["strarray"];
    REQUIRE(strarray.size() == 2);
    REQUIRE(strarray[0] == "hello");
    REQUIRE(strarray[1] == "world");
    std::vector<std::string> emptyarray = result["emptyarray"];
    REQUIRE(emptyarray.size() == 0);

    // Assign some new values
    result.set("newvalue", "789");
    REQUIRE(std::string(result.get("newvalue").c_str()) == "789");
    result.set("array", "[4,5,6]");
    REQUIRE(std::string(result.get("array").c_str()) == "[4,5,6]");

    // Create a JSON object
    json::jobject test;
    test["int"] = 123;
    test["float"] = 12.3f;
    test["string"] = "test \"string";
    int test_array[3] = { 1, 2, 3 };
    test["array"] = std::vector<int>(test_array, test_array + 3);
    std::string test_string_array[2] = { "hello", "world" };
    test["strarray"] = std::vector<std::string>(test_string_array, test_string_array + 2);
    test["emptyarray"] = std::vector<std::string>();
    test["boolean"].set_boolean(true);
    test["null"].set_null();

    json::jobject subobj;
    char world[6] = "world";
    subobj["hello"] = world;
    test["subobj"] = subobj;

    std::vector<json::jobject> objarray;
    objarray.push_back(subobj);
    objarray.push_back(subobj);
    test["objarray"] = objarray;

    std::string serial = (std::string)test;
    json::jobject retest = json::jobject::parse(serial.c_str());

    // Integer
    REQUIRE((int)retest["int"] == 123);
    REQUIRE_FALSE(retest["int"].is_string());
    REQUIRE(retest["int"].is_number());
    REQUIRE_FALSE(retest["int"].is_object());
    REQUIRE_FALSE(retest["int"].is_array());
    REQUIRE_FALSE(retest["int"].is_bool());
    REQUIRE_FALSE(retest["int"].is_null());

    // Float
    REQUIRE((float)retest["float"] == Catch::Approx(12.3f));
    REQUIRE_FALSE(retest["float"].is_string());
    REQUIRE(retest["float"].is_number());
    REQUIRE_FALSE(retest["float"].is_object());
    REQUIRE_FALSE(retest["float"].is_array());
    REQUIRE_FALSE(retest["float"].is_bool());
    REQUIRE_FALSE(retest["float"].is_null());

    // String
    REQUIRE(retest["string"].as_string() == "test \"string");
    REQUIRE(retest["string"].is_string());
    REQUIRE_FALSE(retest["string"].is_number());
    REQUIRE_FALSE(retest["string"].is_object());
    REQUIRE_FALSE(retest["string"].is_array());
    REQUIRE_FALSE(retest["string"].is_bool());
    REQUIRE_FALSE(retest["string"].is_null());

    // Array
    std::vector<int> retest_array = retest["array"];
    REQUIRE(retest_array == std::vector<int>(test_array, test_array + 3));
    REQUIRE_FALSE(retest["array"].is_string());
    REQUIRE_FALSE(retest["array"].is_number());
    REQUIRE(retest["array"].is_object());
    REQUIRE(retest["array"].is_array());
    REQUIRE_FALSE(retest["array"].is_bool());
    REQUIRE_FALSE(retest["array"].is_null());

    // Object
    json::jobject resubobj = retest["subobj"];
    REQUIRE(resubobj["hello"].as_string() == "world");
    REQUIRE_FALSE(retest["subobj"].is_string());
    REQUIRE_FALSE(retest["subobj"].is_number());
    REQUIRE(retest["subobj"].is_object());
    REQUIRE_FALSE(retest["subobj"].is_array());
    REQUIRE_FALSE(retest["subobj"].is_bool());
    REQUIRE_FALSE(retest["subobj"].is_null());

    // Object array
    REQUIRE(retest["objarray"].array(0).as_object() == subobj);
    strarray = retest["strarray"];
    REQUIRE(strarray.size() == 2);
    REQUIRE(strarray[0] == "hello");
    REQUIRE(strarray[1] == "world");
    std::vector<json::jobject> objarrayecho = test["objarray"];
    REQUIRE(objarrayecho.size() == 2);
    REQUIRE_FALSE(retest["objarray"].is_string());
    REQUIRE_FALSE(retest["objarray"].is_number());
    REQUIRE(retest["objarray"].is_object());
    REQUIRE(retest["objarray"].is_array());
    REQUIRE_FALSE(retest["objarray"].is_bool());
    REQUIRE_FALSE(retest["objarray"].is_null());

    // Empty array
    emptyarray = retest["emptyarray"];
    REQUIRE(emptyarray.size() == 0);
    REQUIRE_FALSE(retest["emptyarray"].is_string());
    REQUIRE_FALSE(retest["emptyarray"].is_number());
    REQUIRE(retest["emptyarray"].is_object());
    REQUIRE(retest["emptyarray"].is_array());
    REQUIRE_FALSE(retest["emptyarray"].is_bool());
    REQUIRE_FALSE(retest["emptyarray"].is_null());

    // Boolean
    REQUIRE(retest.has_key("boolean"));
    REQUIRE_FALSE(retest["boolean"].is_string());
    REQUIRE_FALSE(retest["boolean"].is_number());
    REQUIRE_FALSE(retest["boolean"].is_object());
    REQUIRE_FALSE(retest["boolean"].is_array());
    REQUIRE(retest["boolean"].is_bool());
    REQUIRE_FALSE(retest["boolean"].is_null());
    REQUIRE(test["boolean"].is_true());

    // Null
    REQUIRE(test.has_key("null"));
    REQUIRE_FALSE(retest["null"].is_string());
    REQUIRE_FALSE(retest["null"].is_number());
    REQUIRE_FALSE(retest["null"].is_object());
    REQUIRE_FALSE(retest["null"].is_array());
    REQUIRE_FALSE(retest["null"].is_bool());
    REQUIRE(retest["null"].is_null());

    // Test copy constructor
    json::jobject copy(test);
    REQUIRE(copy.as_string() == test.as_string());
}
