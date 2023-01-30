#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include "../include/InvertedIndex.h"

void runThread(int n) {
    //std::cout << "Thread number " << n << " is running ..." << std::endl;
}

InvertedIndex::InvertedIndex() {}

// разделяет строку на слова - разделитель "пробел" и считает повторы слов если они есть
// резуотат в map<слово, количество повторов>
std::map<std::string, int> InvertedIndex::wordsSplit(std::string str) {
    std::map<std::string, int> words;
    if(str.back() != ' ')
        str +=" ";
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
    // заведем набор mutex-ов для безопасности доступа к внешним данным
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
        int tmpCount=0;
        // бежим по wordsCount
        for (auto it = wordsCount.begin(); it != wordsCount.end(); ++it){
            entry_access.lock();
            entry.count = it->second; // добавляем в структуру Entry второй элемент
            entry_access.unlock();
            entryCount[it->first] = entry; // и сформированную сруктуру пишем в std::map<std::string, Entry> entryCount
            tmpCount += it->second; // складываем количество повторов и получаем количество слов в документе
        }
        countDocs_access.lock();
        countDocs.push_back(tmpCount); // т е  порядковый номер вектора это порядковый номер документа (idoc)
        countDocs_access.unlock();
        // таким образом тут у нас по документу iDoc сформирован map<std::string, Entry> entryCount
        // теперь надо ДОБАВИТЬ в частотный словарь ( private переменная класса)
        // map<string, vector<Entry>> freq_dictionary посчитанный для iDoc entryCount

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

void InvertedIndex::UpdateDocumentBase(ConverterJSON converterJson)
{
// считываем документы в которых искать из структуры conf
    std::vector<std::string> vDocs = converterJson.GetTextDocuments();
    for(int i = 0; i < vDocs.size(); ++i){
        std::ifstream  file;
        file.open(vDocs[i], std::ios::in);
        std::string str, strNew;
        while(getline(file, strNew)) {
            str += strNew + " ";
        }
        docs.push_back(str);
    }
    Update_docs(); // там считаем freq_dictionary
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string &word) {
    return freq_dictionary[word];
}

