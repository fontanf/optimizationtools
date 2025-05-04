#pragma once

#include <iostream>
#include <fstream>
#include <vector>

namespace optimizationtools
{

class ComposeStream: public std::ostream
{

private:

    class ComposeBuffer: public std::streambuf
    {

    public:

        void add_buffer(std::streambuf* buf)
        {
            bufs_.push_back(buf);
        }

        void set_buffer_stdout(bool b)
        {
            if (b) {
                buf_stdout_ = std::cout.rdbuf();
            } else {
                buf_stdout_ = nullptr;
            }
        }

        void set_buffer_stderr(bool b)
        {
            if (b) {
                buf_stderr_ = std::cerr.rdbuf();
            } else {
                buf_stderr_ = nullptr;
            }
        }

        void set_buffer_file(
                std::ostream& file)
        {
            if (file.good()) {
                buf_file_ = file.rdbuf();
            } else {
                buf_file_ = nullptr;
            }
        }

        virtual int overflow(int c)
        {
            for (auto& buf: bufs_)
                buf->sputc(c);
            if (buf_stdout_ != nullptr)
                buf_stdout_->sputc(c);
            if (buf_stderr_ != nullptr)
                buf_stderr_->sputc(c);
            if (buf_file_ != nullptr) {
                buf_file_->sputc(c);
            }
            return c;
        }

    private:

        std::vector<std::streambuf*> bufs_;

        std::streambuf* buf_stdout_ = nullptr;

        std::streambuf* buf_stderr_ = nullptr;

        std::streambuf* buf_file_ = nullptr;

    };

public:

    ComposeStream(
            bool to_stdout,
            bool to_stderr,
            const std::string& file_path,
            const std::vector<std::ostream*>& ostreams):
        std::ostream(NULL)
    {
        std::ostream::rdbuf(&buffer_);
        for (std::ostream* os: ostreams) {
            os->flush();
            buffer_.add_buffer(os->rdbuf());
        }
        if (to_stdout) {
            std::cout.flush();
            buffer_.set_buffer_stdout(true);
        }
        if (to_stderr) {
            std::cerr.flush();
            buffer_.set_buffer_stderr(true);
        }
        if (!file_path.empty()) {
            file_.open(file_path);
            buffer_.set_buffer_file(file_);
        }
    }

private:

    ComposeBuffer buffer_;

    std::ofstream file_;

};

}
