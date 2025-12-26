#include "json.h"
#include <gtest/gtest.h>
#include <math.h>
#include <algorithm>
TEST(JsonParserTest, ParseObjectWithMixedTypes)
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
    ASSERT_EQ(keys.size(), 8);
    ASSERT_NE(std::find(keys.begin(), keys.end(), "number"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "string"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "array"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "boolean"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "isnull"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "objarray"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "strarray"), keys.end());
    ASSERT_NE(std::find(keys.begin(), keys.end(), "emptyarray"), keys.end());

    // Test individual entries
    ASSERT_FALSE(result.is_array());
    ASSERT_STREQ(result.get("number").c_str(), "123.456");
    ASSERT_STREQ(result.get("string").c_str(), "\"hello \\\" world\"");
    ASSERT_STREQ(result.get("array").c_str(), "[1,2,3]");
    ASSERT_STREQ(result.get("boolean").c_str(), "true");
    ASSERT_STREQ(result.get("isnull").c_str(), "null");
    ASSERT_STREQ(result.get("objarray").c_str(), "[{\"key\":\"value\"}]");
    ASSERT_STREQ(result.get("strarray").c_str(), "[\"hello\",\"world\"]");
    ASSERT_STREQ(result.get("emptyarray").c_str(), "[]");
    ASSERT_TRUE(result.has_key("number"));
    ASSERT_FALSE(result.has_key("nokey"));
    ASSERT_STREQ(result["objarray"].array(0).get("key").as_string().c_str(), "value");
    std::vector<std::string> strarray = result["strarray"];
    ASSERT_EQ(strarray.size(), 2);
    ASSERT_STREQ(strarray[0].c_str(), "hello");
    ASSERT_STREQ(strarray[1].c_str(), "world");
    std::vector<std::string> emptyarray = result["emptyarray"];
    ASSERT_EQ(emptyarray.size(), 0);

    // Assign some new values
    result.set("newvalue", "789");
    ASSERT_STREQ(result.get("newvalue").c_str(), "789");
    result.set("array", "[4,5,6]");
    ASSERT_STREQ(result.get("array").c_str(), "[4,5,6]");

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
    ASSERT_EQ((int)retest["int"], 123);
    ASSERT_FALSE(retest["int"].is_string());
    ASSERT_TRUE(retest["int"].is_number());
    ASSERT_FALSE(retest["int"].is_object());
    ASSERT_FALSE(retest["int"].is_array());
    ASSERT_FALSE(retest["int"].is_bool());
    ASSERT_FALSE(retest["int"].is_null());

    // Float
    ASSERT_NEAR((float)retest["float"], 12.3f, 1.0e-6);
    ASSERT_FALSE(retest["float"].is_string());
    ASSERT_TRUE(retest["float"].is_number());
    ASSERT_FALSE(retest["float"].is_object());
    ASSERT_FALSE(retest["float"].is_array());
    ASSERT_FALSE(retest["float"].is_bool());
    ASSERT_FALSE(retest["float"].is_null());

    // String
    ASSERT_STREQ(retest["string"].as_string().c_str(), "test \"string");
    ASSERT_TRUE(retest["string"].is_string());
    ASSERT_FALSE(retest["string"].is_number());
    ASSERT_FALSE(retest["string"].is_object());
    ASSERT_FALSE(retest["string"].is_array());
    ASSERT_FALSE(retest["string"].is_bool());
    ASSERT_FALSE(retest["string"].is_null());

    // Array
    std::vector<int> retest_array = retest["array"];
    ASSERT_EQ(retest_array, std::vector<int>(test_array, test_array + 3));
    ASSERT_FALSE(retest["array"].is_string());
    ASSERT_FALSE(retest["array"].is_number());
    ASSERT_TRUE(retest["array"].is_object());
    ASSERT_TRUE(retest["array"].is_array());
    ASSERT_FALSE(retest["array"].is_bool());
    ASSERT_FALSE(retest["array"].is_null());

    // Object
    json::jobject resubobj = retest["subobj"];
    ASSERT_STREQ(resubobj["hello"].as_string().c_str(), "world");
    ASSERT_FALSE(retest["subobj"].is_string());
    ASSERT_FALSE(retest["subobj"].is_number());
    ASSERT_TRUE(retest["subobj"].is_object());
    ASSERT_FALSE(retest["subobj"].is_array());
    ASSERT_FALSE(retest["subobj"].is_bool());
    ASSERT_FALSE(retest["subobj"].is_null());

    // Object array
    ASSERT_TRUE(retest["objarray"].array(0).as_object() == subobj);
    strarray = retest["strarray"];
    ASSERT_EQ(strarray.size(), 2);
    ASSERT_STREQ(strarray[0].c_str(), "hello");
    ASSERT_STREQ(strarray[1].c_str(), "world");
    std::vector<json::jobject> objarrayecho = test["objarray"];
    ASSERT_EQ(objarrayecho.size(), 2);
    ASSERT_FALSE(retest["objarray"].is_string());
    ASSERT_FALSE(retest["objarray"].is_number());
    ASSERT_TRUE(retest["objarray"].is_object());
    ASSERT_TRUE(retest["objarray"].is_array());
    ASSERT_FALSE(retest["objarray"].is_bool());
    ASSERT_FALSE(retest["objarray"].is_null());

    // Empty array
    emptyarray = retest["emptyarray"];
    ASSERT_EQ(emptyarray.size(), 0);
    ASSERT_FALSE(retest["emptyarray"].is_string());
    ASSERT_FALSE(retest["emptyarray"].is_number());
    ASSERT_TRUE(retest["emptyarray"].is_object());
    ASSERT_TRUE(retest["emptyarray"].is_array());
    ASSERT_FALSE(retest["emptyarray"].is_bool());
    ASSERT_FALSE(retest["emptyarray"].is_null());

    // Boolean
    ASSERT_TRUE(retest.has_key("boolean"));
    ASSERT_FALSE(retest["boolean"].is_string());
    ASSERT_FALSE(retest["boolean"].is_number());
    ASSERT_FALSE(retest["boolean"].is_object());
    ASSERT_FALSE(retest["boolean"].is_array());
    ASSERT_TRUE(retest["boolean"].is_bool());
    ASSERT_FALSE(retest["boolean"].is_null());
    ASSERT_TRUE(test["boolean"].is_true());

    // Null
    ASSERT_TRUE(test.has_key("null"));
    ASSERT_FALSE(retest["null"].is_string());
    ASSERT_FALSE(retest["null"].is_number());
    ASSERT_FALSE(retest["null"].is_object());
    ASSERT_FALSE(retest["null"].is_array());
    ASSERT_FALSE(retest["null"].is_bool());
    ASSERT_TRUE(retest["null"].is_null());

    // Test copy constructor
    json::jobject copy(test);
    ASSERT_STREQ(copy.as_string().c_str(), test.as_string().c_str());
}
