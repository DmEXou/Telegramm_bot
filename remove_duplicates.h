#pragma once
#include "search_server.h"

void RemoveDuplicates(SearchServer& search_server) {
    size_t Count = search_server.GetDocumentCount();
    vector<map<string, double>> tmp;
    vector<int> remuve_id;
    for (auto& ss : search_server){
//    for (int i = 1; i < Count; ++i) {
        tmp.push_back(search_server.GetWordFrequencies(ss));
        if (ss == Count) break;
        if (count(tmp.begin(), tmp.end(), search_server.GetWordFrequencies(ss +1)) > 0) {
            remuve_id.push_back(ss+1);
            cout << "Found duplicate document id " << ss+1 << endl;
            //search_server.RemoveDocument(ss+1);
        }
    }
    for (int id : remuve_id) {
        search_server.RemoveDocument(id);
    }
    //O(WNlogN)
//    search_server.GetWordFrequencies();
//    search_server.RemoveDocument();
}