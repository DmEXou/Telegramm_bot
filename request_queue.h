#pragma once
#include <deque>

#include "search_server.h"

class RequestQueue {
public:
    RequestQueue(SearchServer& search_server)
    {
        search_server_ = &search_server;
    }

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        std::vector<Document> result_search = search_server_->FindTopDocuments(raw_query, document_predicate);
        UpdateQueue(result_search, raw_query);
        return result_search;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status) {
        auto document_predicate = [](int document_id, DocumentStatus status, int rating) {
            return status == DocumentStatus::ACTUAL;
        };
        return AddFindRequest(raw_query, document_predicate);
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query) {
        return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
    }

    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        std::string query;
        int sum_return = 0;
    };

private:
    static const int kMinToDay = 1440;

private:
    void UpdateQueue(std::vector<Document>& result_search, const std::string& raw_query);

    void AddInQueue(const size_t sum_return, const std::string& raw_query);

    void RemoveInQueue();

private:
    SearchServer* search_server_;
    std::deque<QueryResult> requests_;
    int empty_request_ = 0;
};
