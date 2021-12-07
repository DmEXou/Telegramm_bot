#include "request_queue.h"

RequestQueue::RequestQueue(SearchServer search_server)
    : search_server_(search_server)
{
}

int RequestQueue::GetNoResultRequests() const {
    return empty_request_;
}

void RequestQueue::UpdateQueue(std::vector<Document>& result_search, const std::string& raw_query) {
    AddInQueue(result_search.size(), raw_query);
    RemoveInQueue();
}

void RequestQueue::AddInQueue(const size_t sum_return, const std::string& raw_query) {
    QueryResult query_result;
    query_result.sum_return = sum_return;
    if (query_result.sum_return != 0) {
        query_result.query = raw_query;
    }
    else ++empty_request_;
    requests_.push_back(query_result);
}

void RequestQueue::RemoveInQueue() {
    if (requests_.size() > kMinToDay) {
        requests_.pop_front();
        --empty_request_;
    }
}