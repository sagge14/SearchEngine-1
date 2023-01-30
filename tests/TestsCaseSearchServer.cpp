#include "gtest/gtest.h"
#include "vector"
#include "InvertedIndex.h"
#include "SearchServer.h"

TEST(TestCaseSearchServer, TestSimple) {
    const std::vector<std::vector<RelativeIndex>> expected = {
            {
                {0, 0.699999988},
                {1, 0.300000012},
                {2, 1}
            },
            {
            }
    };
    ConverterJSON converterJson("configSearchSimple.json", "requestsSearchTestSimple.json");
    InvertedIndex idx;
    idx.UpdateDocumentBase(converterJson);

    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(converterJson);
    ASSERT_EQ(result, expected);
}

TEST(TestCaseSearchServer, TestTop5) {
    const std::vector<std::vector<RelativeIndex>> expected = {
            {
                    {0, 0.666666687},
                    {1, 0.666666687},
                    {2, 0.666666687},
                    {3, 0.666666687},
                    {4, 0.666666687},
                    {5, 0.666666687},
                    {6, 0.666666687},
                    {7, 1.000000000},
                    {8, 0.666666687},
                    {9, 0.666666687},
                    {10, 0.666666687},
                    {11, 0.666666687},
                    {12, 0.666666687},
                    {13, 0.666666687},
                    {14, 1.000000000},
                    {15, 0.666666687},
                    {16, 0.666666687},
                    {17, 0.666666687},
                    {18, 0.666666687},
                    {19, 0.666666687},
                    {20, 0.666666687},
                    {21, 0.666666687}
            }
    };
    ConverterJSON converterJson("configSearchTestTop5.json", "requestsSearchTestTop5.json");
    InvertedIndex idx;
    idx.UpdateDocumentBase(converterJson);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(converterJson);
    ASSERT_EQ(result, expected);
}