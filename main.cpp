#include "include/ConverterJSON.h"
#include "include/InvertedIndex.h"
#include "include/SearchServer.h"

int main() {
    // в конструкторе считываем config.json,
    // сами запросы из requests.json
    // и имена файлов где искать
    // все это в структуре conf к которой сделаны getters
    // GetTextDocuments() и GetRequests()
    ConverterJSON converterJson;
    InvertedIndex invertedIndex;

    // обновляем базы запросов и самих документов где искать
    invertedIndex.UpdateDocumentBase(
            converterJson.GetTextDocuments(),
            converterJson.GetRequests()
    );
    SearchServer searchServer(invertedIndex);
    return 0;
}
