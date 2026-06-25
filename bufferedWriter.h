#ifndef BUFFERED_WRITER_H
#define BUFFERED_WRITER_H

#include <fstream>
#include <sstream>
#include <string>

static constexpr std::streamsize BUFSIZE = 16 * 1024 * 1024;

class BufferedWriter
{
    std::ofstream file;
    std::ostringstream buf;
    std::streamsize threshold;

public:
    explicit BufferedWriter(const std::string& path,
                            std::ios_base::openmode mode = std::ios::out,
                            std::streamsize flush_threshold = BUFSIZE);
    ~BufferedWriter();

    // No copy — owns an open file handle
    BufferedWriter(const BufferedWriter&) = delete;
    BufferedWriter& operator=(const BufferedWriter&) = delete;

    template<typename T>
    BufferedWriter& operator<<(const T& val)
    {
        buf << val;
        if (buf.tellp() >= threshold)
            flush();
        return *this;
    }

    void flush();
    void reopen(const std::string& path, std::ios_base::openmode mode = std::ios::out);
    bool is_open() const { return file.is_open(); }
};


#endif
