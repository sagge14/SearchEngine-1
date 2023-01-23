#pragma once
#include "nlohmann/json.hpp"
#include "ConverterJSON.h"


struct Entry {
    size_t doc_id, count;
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex();
    void UpdateDocumentBase(ConverterJSON);
    void UpdateDocumentBase1(std::vector<std::string> input_docs);

    std::vector<Entry> GetWordCount(const std::string& word);


protected:
    std::map<std::string, int> wordsSplit(std::string);
    void Update_docs();


private:
    std::vector<std::string> docs;
    std::vector<int> countDocs; // добавил этот вектор что бы в split сразу считать и хранить в нем общее число слов
    // в документе - нужно для подсчета Rel relevance в конце (как знаменатель)

    std::map<std::string, std::vector<Entry>> freq_dictionary; // частотный словарь
};