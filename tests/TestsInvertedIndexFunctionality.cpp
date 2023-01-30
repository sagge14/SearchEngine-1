#include "gtest/gtest.h"
#include "vector"
#include "InvertedIndex.h"
#include "ConverterJSON.h"

void TestInvertedIndexFunctionality(
        const std::string &docs,
        const std::string &requests,
        const std::vector<std::vector<Entry>>& expected
)
{
    std::vector<std::vector<Entry>> result;
    ConverterJSON converterJson(docs,requests);
    InvertedIndex idx;
    idx.UpdateDocumentBase(converterJson);

    for(auto& request : converterJson.GetRequests()) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
    }
    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
    const std::vector<std::vector<Entry>> expected = {
            {
                    {0, 1}
            },
            {
                    {0, 1}, {1, 3}
            }
    };
    TestInvertedIndexFunctionality("configTestBasic.json", "requestsTestBasic.json", expected);
}

TEST(TestCaseInvertedIndex, TestBasic2) {
    const std::vector<std::vector<Entry>> expected = {
            {
                    {0, 4}, {1, 1}, {2, 5}
            }, {
                    {0, 3}, {1, 2}, {2, 5}
            }, {
                    {3, 1}
            }
    };
    TestInvertedIndexFunctionality("configTestBasic2.json", "requestsTestBasic2.json", expected);
}

TEST(TestCaseInvertedIndex, MissingWord) {
    const std::vector<std::vector<Entry>> expected = {
            { },
            { {1, 1} }
    };
    TestInvertedIndexFunctionality("configMissingWord.json", "requestsMissingWord.json", expected);
}
