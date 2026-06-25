#include "bufferedWriter.h"


BufferedWriter::BufferedWriter(const std::string& path,
                               std::ios_base::openmode mode,
                               std::streamsize flush_threshold)
    : threshold(flush_threshold)
{
    file.open(path, mode);
}


BufferedWriter::~BufferedWriter()
{
    flush();
}


void BufferedWriter::flush()
{
    if (!file.is_open()) return;
    if (buf.tellp() > 0)
    {
        file << buf.str();
        file.flush();
        buf.str("");
        buf.clear();
    }
}


// Flush pending data, close the current file, open a new one.
// Useful for the per-step numbered config files.
void BufferedWriter::reopen(const std::string& path, std::ios_base::openmode mode)
{
    flush();
    file.close();
    file.open(path, mode);
}
