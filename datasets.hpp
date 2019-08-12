#pragma once

#include <string>
#include <vector>

template<typename T>
class DatasetFromFiles: public std::iterator<std::input_iterator_tag, int, ptrdiff_t, T const*, T>
{

public:

    DatasetFromFiles(const std::vector<std::string>& s, std::string format):
        s_(s), format_(format), ins_(s_[0], format_) {}

    operator bool() const { return i_ < s_.size(); }

    DatasetFromFiles& operator++()
    {
        assert(*this);
        ins_ = T(s_[++i_], format_);
        return *this;
    }

    DatasetFromFiles operator++(int)
    {
        DatasetFromFiles tmp = *this;
        ++*this;
        return tmp;
    }

    T operator*() const { assert(*this); return ins_; }
    T const* operator->() const { assert(*this); return &ins_; }

private:

    const std::vector<std::string> s_;
    std::string format_;
    size_t i_ = 0;
    T ins_;

};

