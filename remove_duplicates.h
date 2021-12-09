#pragma once
#include "search_server.h"

void RemoveDuplicates(SearchServer& search_server) {
    size_t Count = search_server.GetDocumentCount();
    vector<map<string, double>> tmp;
    for (int i = 1; i < Count; ++i) {
        tmp.push_back(search_server.GetWordFrequencies(i));
        if (count(tmp.begin(), tmp.end(), search_server.GetWordFrequencies(i+1)) > 0) {
            cout << "Found duplicate document id " << i+1 << endl;
            search_server.RemoveDocument(i+1);
        }
    }
    //O(WNlogN)
//    search_server.GetWordFrequencies();
//    search_server.RemoveDocument();
}