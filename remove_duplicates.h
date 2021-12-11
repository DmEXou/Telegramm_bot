#pragma once

void RemoveDuplicates(SearchServer& search_server){
    size_t counter = search_server.GetDocumentCount();
    std::vector<int> remuve_id;
    std::vector<std::string> aligned;
    for (auto& worker_id : search_server) {
        std::string tmp_str;
        for (auto& tmp_map : search_server.GetWordFrequencies(worker_id)) {
            tmp_str += tmp_map.first;
        }
        aligned.push_back(tmp_str);
        if (worker_id == counter) break;
        tmp_str.clear();
        for (auto& tmp_map : search_server.GetWordFrequencies(worker_id +1)) {
            tmp_str += tmp_map.first;
        }
        if (std::count(aligned.begin(), aligned.end(), tmp_str) > 0) {
            remuve_id.push_back(worker_id + 1);
            std::cout << "Found duplicate document id " << worker_id +1 << std::endl;
        }
    }
    for (int id : remuve_id) {
        search_server.RemoveDocument(id);
    }
}