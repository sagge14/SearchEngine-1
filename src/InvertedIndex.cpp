#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include "../include/InvertedIndex.h"

void runThread(int n) {
    //std::cout << "Thread number " << n << " is running ..." << std::endl;
    }

InvertedIndex::InvertedIndex() {}

std::map<std::string, int> InvertedIndex::wordsSplit(std::string str) {
    std::map<std::string, int> words;
    int start = 0;
    int end = str.find(" ");
    while (end != -1) {
        std::string word = str.substr(start, end - start);

        if (words.find(word) == words.end()) {
            words[word] = 1;
        } else {
            words[word] = ++words[word];
        }
        start = end + 1;
        end = str.find(" ", start);
    }
    return words;
}

void InvertedIndex::Update_docs() {

    std::mutex entry_access;
    std::mutex vEntry_access;

    std::mutex freq_dictionary_access;
    std::mutex countDocs_access;

    Entry entry;
    std::vector<Entry> vEntry;

    std::vector<std::thread> threads;

    for(int iDoc = 0; iDoc < docs.size(); ++iDoc) {

        threads.push_back(std::thread(runThread,iDoc));

        // для подсчета повторов слов в документе
        std::map<std::string, int> wordsCount;

        // по заданию надо заполнить структуру Entry - формируем map <слово ->  {doc_id, count}>;
        std::map<std::string, Entry> entryCount;

        // делим документ на слова, считаем их повторения и кладем в wordsCount
        wordsCount = wordsSplit(docs[iDoc]);

        // кладем номер документа как первый элемент в структуру Entry
        entry_access.lock();
        entry.doc_id = iDoc;
        entry_access.unlock();

        // и надо еще иметь инфу сколько слов вообще в документе - для знаменателя в Rel relevance.
        // конечно вопрос - почему это не посчитать при разбивке на слова в split-e
        // но я этот wordsSplit использую не только для парсинга документов но и для парсинга запросов,
        // и надо будет возвращать не только map<слово, число повторов> но и еще число слов всего
        // короче что там счетчик прибавлять, что тут маленькую арифметику и результат в private countDocs
        // ну и геттер на него
        int tmpCount=0;

        // бежим по wordsCount
        for (auto it = wordsCount.begin(); it != wordsCount.end(); ++it) {

            entry_access.lock();
            entry.count = it->second; // добавляем в структуру Entry второй элемент
            entry_access.unlock();

            entryCount[it->first] = entry; // и сформированную сруктуру пишем в std::map<std::string, Entry> entryCount

            tmpCount += it->second; // складываем количество повторов и получаем количество слов в документе
        }

        countDocs_access.lock();
        countDocs.push_back(tmpCount); // таким образом порядковый номер вектора это порядковый номер документа (idoc)
        countDocs_access.unlock();

        // значение - общее число слов документа включая повторы

        // таким образом тут у нас по документу iDoc сформирована
        // std::map<std::string, Entry> entryCount

        // теперь надо ДОБАВИТЬ в частотный словарь ( private переменная класса)
        // std::map<std::string, std::vector<Entry>> freq_dictionary;
        // посчитанный для iDoc entryCount

        for (auto it1 = entryCount.begin(); it1 != entryCount.end(); ++it1) {
            std::vector<Entry> vEntry;

            vEntry_access.lock();
            vEntry.push_back(it1->second);
            vEntry_access.unlock();

            freq_dictionary_access.lock();
            freq_dictionary[it1->first].push_back(it1->second);
            freq_dictionary_access.unlock();
        }
    }
    // тут по всем документам пробежали и freq_dictionary заполнили

    std::cout << "synchronizing all threads...\n";
    for (auto& th : threads) th.join();
}

void InvertedIndex::UpdateDocumentBase(
        std::vector<std::string> input_docs,  // вектор документов (имен файлов) - где искать
        std::vector<std::string> input_requests) // вектор слов - какие искать
{
    // бежим по всем документам и каждый документ пишем в вектор
    for(int i = 0; i < input_docs.size(); ++i){
        std::ifstream  file;
        file.open(input_docs[i], std::ios::in);

        std::string str, strNew;
        while(getline(file, strNew)) {
            str += strNew + " ";
        }
        docs.push_back(str);
    }
    Update_docs(); // считаем индекс freq_dictionary

    // тут решил обрабатать и requests (подал как параметр) - главным образом из за того что бы использовать
    // уже написанную wordsSplit
    // результат положил в private класса std::map<std::string, int> requestsMap;
    // и три геттера внизу для SearchServer - вроде логично
    std::string str;
    for(int i = 0; i < input_requests.size(); ++i) {
        str += input_requests[i] + " ";
    }
    requestsMap = wordsSplit(str);
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) {
//    std::vector<Entry> tmp;
//    tmp = freq_dictionary[word];
//    std::cout << "Word " << word << std::endl;
//    for (int i = 0; i < tmp.size(); ++i) {
//        std::cout << "DocId = " << tmp[i].doc_id << " ";
//        std::cout << "Count = " << tmp[i].count << std::endl;
//    }
    return freq_dictionary[word];
}

std::map<std::string, std::vector<Entry>> InvertedIndex::get_freq_dictionary() {
    return std::map<std::string, std::vector<Entry>>(freq_dictionary);
}

std::map<std::string, int> InvertedIndex::get_requestsMap() {
    return std::map<std::string, int>(requestsMap);
}

std::vector<int> InvertedIndex::get_DocumentCountWords() {
    return std::vector<int>(countDocs);
}