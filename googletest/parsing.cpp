#include "json.h"
#include <gtest/gtest.h>

typedef struct test_struct_struct
{
    const char *input;
    const char *output;
    json::jtype::jtype type;
} test_struct_t;

const test_struct_t test_data[] = {
    { "\"Hello World\"", "\"Hello World\"", json::jtype::jstring },
    { "\"Hello World\\\"\"", "\"Hello World\\\"\"", json::jtype::jstring },
    { "true", "true", json::jtype::jbool },
    { "false", "false", json::jtype::jbool },
    { "null", "null", json::jtype::jnull },
    { "[]", "[]", json::jtype::jarray },
    { "[\"Hello World\"]", "[\"Hello World\"]", json::jtype::jarray },
    { "[\"Hello World\",true,false,null]", "[\"Hello World\",true,false,null]", json::jtype::jarray },
    { "{}", "{}", json::jtype::jobject},
    { "{\"hello\":\"world\"}", "{\"hello\":\"world\"}", json::jtype::jobject},
    { "[{\"hello\":\"world\"},{\"hello\":\"world\"}]", "[{\"hello\":\"world\"},{\"hello\":\"world\"}]", json::jtype::jarray}
};

const char *test_numbers[] = { "0", "-0", "123", "-123", "0.123", "-0.123", "123.456", "-123.456", "123e456", "123e+456", "123e-456", "123.456e789", "123.456e+789", "123.456e-789" };

class JsonReaderTest : public ::testing::Test {
protected:
    json::reader stream;

    void SetUp() override {
        stream.clear();
    }
};

TEST_F(JsonReaderTest, ParseVariousJsonTypes)
{
    const size_t data_points = sizeof(test_data) / sizeof(test_struct_t);
    size_t i, j;

    for(i = 0; i < data_points; i++) {
        const test_struct_t data_point = test_data[i];
        for(j = 0; j < strlen(data_point.input); j++) {
            ASSERT_EQ(stream.push(data_point.input[j]), json::reader::ACCEPTED);
        }
        ASSERT_EQ(stream.type(), data_point.type);
        ASSERT_STREQ(stream.readout().c_str(), data_point.output);
        stream.clear();
    }

    // Verify iteration through the data
    ASSERT_GT(i, 0);
}

TEST_F(JsonReaderTest, ParseNumbers)
{
    const size_t num_num = sizeof(test_numbers) / sizeof(char*);
    size_t i, j;

    for(i = 0; i < num_num; i++) {
        const char *data_point = test_numbers[i];
        for(j = 0; j < strlen(data_point); j++) {
            ASSERT_EQ(stream.push(data_point[j]), json::reader::ACCEPTED);
        }
        ASSERT_EQ(stream.push(' '), json::reader::REJECTED);
        ASSERT_EQ(stream.type(), json::jtype::jnumber);
        ASSERT_STREQ(stream.readout().c_str(), data_point);
        stream.clear();
    }
}
