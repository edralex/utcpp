#include "json.h"
#include <doctest/doctest.h>
#include <math.h>
#include <algorithm>
#include <cstring>

TEST_CASE("JsonParserTest - ParseObjectWithMixedTypes")
{
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
    CHECK_EQ(keys.size(), 8);
    CHECK_NE(std::find(keys.begin(), keys.end(), "number"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "string"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "array"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "boolean"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "isnull"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "objarray"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "strarray"), keys.end());
    CHECK_NE(std::find(keys.begin(), keys.end(), "emptyarray"), keys.end());

    // Test individual entries
    CHECK_FALSE(result.is_array());
    CHECK_EQ(strcmp(result.get("number").c_str(), "123.456"), 0);
    CHECK_EQ(strcmp(result.get("string").c_str(), "\"hello \\\" world\""), 0);
    CHECK_EQ(strcmp(result.get("array").c_str(), "[1,2,3]"), 0);
    CHECK_EQ(strcmp(result.get("boolean").c_str(), "true"), 0);
    CHECK_EQ(strcmp(result.get("isnull").c_str(), "null"), 0);
    CHECK_EQ(strcmp(result.get("objarray").c_str(), "[{\"key\":\"value\"}]"), 0);
    CHECK_EQ(strcmp(result.get("strarray").c_str(), "[\"hello\",\"world\"]"), 0);
    CHECK_EQ(strcmp(result.get("emptyarray").c_str(), "[]"), 0);
    CHECK(result.has_key("number"));
    CHECK_FALSE(result.has_key("nokey"));
    CHECK_EQ(strcmp(result["objarray"].array(0).get("key").as_string().c_str(), "value"), 0);
    std::vector<std::string> strarray = result["strarray"];
    CHECK_EQ(strarray.size(), 2);
    CHECK_EQ(strarray[0], "hello");
    CHECK_EQ(strarray[1], "world");
    std::vector<std::string> emptyarray = result["emptyarray"];
    CHECK_EQ(emptyarray.size(), 0);

    // Assign some new values
    result.set("newvalue", "789");
    CHECK_EQ(strcmp(result.get("newvalue").c_str(), "789"), 0);
    result.set("array", "[4,5,6]");
    CHECK_EQ(strcmp(result.get("array").c_str(), "[4,5,6]"), 0);

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
    CHECK_EQ((int)retest["int"], 123);
    CHECK_FALSE(retest["int"].is_string());
    CHECK(retest["int"].is_number());
    CHECK_FALSE(retest["int"].is_object());
    CHECK_FALSE(retest["int"].is_array());
    CHECK_FALSE(retest["int"].is_bool());
    CHECK_FALSE(retest["int"].is_null());

    // Float
    CHECK_EQ(doctest::Approx((float)retest["float"]), 12.3f);
    CHECK_FALSE(retest["float"].is_string());
    CHECK(retest["float"].is_number());
    CHECK_FALSE(retest["float"].is_object());
    CHECK_FALSE(retest["float"].is_array());
    CHECK_FALSE(retest["float"].is_bool());
    CHECK_FALSE(retest["float"].is_null());

    // String
    CHECK_EQ(strcmp(retest["string"].as_string().c_str(), "test \"string"), 0);
    CHECK(retest["string"].is_string());
    CHECK_FALSE(retest["string"].is_number());
    CHECK_FALSE(retest["string"].is_object());
    CHECK_FALSE(retest["string"].is_array());
    CHECK_FALSE(retest["string"].is_bool());
    CHECK_FALSE(retest["string"].is_null());

    // Array
    std::vector<int> retest_array = retest["array"];
    CHECK_EQ(retest_array, std::vector<int>(test_array, test_array + 3));
    CHECK_FALSE(retest["array"].is_string());
    CHECK_FALSE(retest["array"].is_number());
    CHECK(retest["array"].is_object());
    CHECK(retest["array"].is_array());
    CHECK_FALSE(retest["array"].is_bool());
    CHECK_FALSE(retest["array"].is_null());

    // Object
    json::jobject resubobj = retest["subobj"];
    CHECK_EQ(strcmp(resubobj["hello"].as_string().c_str(), "world"), 0);
    CHECK_FALSE(retest["subobj"].is_string());
    CHECK_FALSE(retest["subobj"].is_number());
    CHECK(retest["subobj"].is_object());
    CHECK_FALSE(retest["subobj"].is_array());
    CHECK_FALSE(retest["subobj"].is_bool());
    CHECK_FALSE(retest["subobj"].is_null());

    // Object array
    CHECK(retest["objarray"].array(0).as_object() == subobj);
    strarray = retest["strarray"];
    CHECK_EQ(strarray.size(), 2);
    CHECK_EQ(strarray[0], "hello");
    CHECK_EQ(strarray[1], "world");
    std::vector<json::jobject> objarrayecho = test["objarray"];
    CHECK_EQ(objarrayecho.size(), 2);
    CHECK_FALSE(retest["objarray"].is_string());
    CHECK_FALSE(retest["objarray"].is_number());
    CHECK(retest["objarray"].is_object());
    CHECK(retest["objarray"].is_array());
    CHECK_FALSE(retest["objarray"].is_bool());
    CHECK_FALSE(retest["objarray"].is_null());

    // Empty array
    emptyarray = retest["emptyarray"];
    CHECK_EQ(emptyarray.size(), 0);
    CHECK_FALSE(retest["emptyarray"].is_string());
    CHECK_FALSE(retest["emptyarray"].is_number());
    CHECK(retest["emptyarray"].is_object());
    CHECK(retest["emptyarray"].is_array());
    CHECK_FALSE(retest["emptyarray"].is_bool());
    CHECK_FALSE(retest["emptyarray"].is_null());

    // Boolean
    CHECK(retest.has_key("boolean"));
    CHECK_FALSE(retest["boolean"].is_string());
    CHECK_FALSE(retest["boolean"].is_number());
    CHECK_FALSE(retest["boolean"].is_object());
    CHECK_FALSE(retest["boolean"].is_array());
    CHECK(retest["boolean"].is_bool());
    CHECK_FALSE(retest["boolean"].is_null());
    CHECK(test["boolean"].is_true());

    // Null
    CHECK(test.has_key("null"));
    CHECK_FALSE(retest["null"].is_string());
    CHECK_FALSE(retest["null"].is_number());
    CHECK_FALSE(retest["null"].is_object());
    CHECK_FALSE(retest["null"].is_array());
    CHECK_FALSE(retest["null"].is_bool());
    CHECK(retest["null"].is_null());

    // Test copy constructor
    json::jobject copy(test);
    CHECK_EQ(strcmp(copy.as_string().c_str(), test.as_string().c_str()), 0);
}
