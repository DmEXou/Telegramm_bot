#pragma once
#include <vector>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first_(begin)
        , last_(end)
        , size_(std::distance(first_, last_)) {
    }

    Iterator begin() const {
        return first_;
    }

    Iterator end() const {
        return last_;
    }

    size_t size() const {
        return size_;
    }

private:
    Iterator first_, last_;
    size_t size_;
};

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator first, Iterator last, size_t page_size)
    {
        int left = std::distance(first, last);
        Iterator page_begin = first;
        while (left > 0){
            if (left >= page_size) {
                IteratorRange<Iterator> elem_vec(page_begin, (page_begin + page_size));
                pages_.push_back(elem_vec);
                page_begin += page_size;
                left -= page_size;
            }
            else {
                IteratorRange<Iterator> elem_vec(page_begin, last);
                pages_.push_back(elem_vec);
                left = 0;
            }
        }
    }

    auto begin() const {
        return pages_.begin();
    }
    auto end() const {
        return pages_.end();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
