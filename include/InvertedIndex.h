#pragma once
#include "nlohmann/json.hpp"
#include "ConverterJSON.h"


struct Entry {
    size_t doc_id, count;
// Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};


class InvertedIndex {
public:
    InvertedIndex();

// Обновить или заполнить базу документов, по которой будем совершать поиск
// @param texts_input содержимое документов
    void UpdateDocumentBase(std::vector<std::string> input_docs, std::vector<std::string> input_requests);
// Метод определяет количество вхождений слова word в загруженной базе документов
// @param word слово, частоту вхождений которого необходимо определить
// @return возвращает подготовленный список с частотой слов
//void updateRequestsBase(std::string input_requests)

    std::vector<Entry> GetWordCount(const std::string& word);

    std::map<std::string, std::vector<Entry>> get_freq_dictionary();

    std::vector<int> get_DocumentCountWords();
    std::map<std::string, int> get_requestsMap();


protected:
    std::map<std::string, int> wordsSplit(std::string);
    void Update_docs();
//    void Update_requests();

private:
    std::vector<std::string> docs;
    std::vector<int> countDocs; // добавил этот вектор что бы в split сразу считать и хранить в нем общее число слов
    // в документе - нужно для подсчета Rel relevance в конце (как знаменатель)

    std::map<std::string, std::vector<Entry>> freq_dictionary; // частотный словарь
    std::map<std::string, int> requestsMap;
};