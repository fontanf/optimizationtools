#pragma once

#include <benchtools/dataset.hpp>

#include <string>
#include <vector>
#include <memory>

typedef int64_t InsId;

template <typename T>
class Dataset
{

public:

    virtual T instance(InsId i) = 0;
    virtual InsId size() = 0;
    virtual ~Dataset() { }

};

/**
 * Create a dataset from a list of files.
 */
template<typename T>
class DatasetFromFiles: public Dataset<T>
{

public:

    DatasetFromFiles(const std::vector<std::string>& s, std::string format): s_(s), format_(format) {}
    T instance(InsId i) { return T(s_[i], format_); }
    InsId size() { return s_.size(); }
    virtual ~DatasetFromFiles() { }

private:

    const std::vector<std::string> s_;
    std::string format_;

};

/**
 * Create a dataset for Problem T from a dataset for Problem T2.
 */
template<typename T, typename T2>
class DatasetFromOther: public Dataset<T>
{

public:

    DatasetFromOther(std::shared_ptr<Dataset<T2>> dataset): dataset_(dataset) {}
    T instance(InsId i) { return T(dataset_->instance(i)); }
    InsId size() { return dataset_->size(); }
    virtual ~DatasetFromOther() { }

private:

    std::shared_ptr<Dataset<T2>> dataset_;

};

