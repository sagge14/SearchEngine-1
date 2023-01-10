#include "gtest/gtest.h"
#include "vector"
#include "InvertedIndex.h"



void TestInvertedIndexFunctionality(
        const std::vector<std::string>& docs,
        const std::vector<std::string>& requests,
        const std::vector<std::vector<Entry>>& expected
) {
std::vector<std::vector<Entry>> result;

    InvertedIndex idx;
    idx.UpdateDocumentBase(docs, requests);
    for(auto& request : requests) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
    }
    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
//    const std::vector<std::string> docs = {
//            "london is the capital of great britain",
//            "big ben is the nickname for the Great bell of the striking clock"
//    };

    const std::vector<std::string> docs = {
            "file0001.txt",
            "file0002.txt"
    };


    const std::vector<std::string> requests = {"london", "the"};
    const std::vector<std::vector<Entry>> expected = {
            {
                    {0, 1}
            },
            {
                    {0, 1}, {1, 3}
            }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}