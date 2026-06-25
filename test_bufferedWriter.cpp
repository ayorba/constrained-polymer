#include <cassert>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include "bufferedWriter.h"


static std::string read_file(const std::string& path)
{
    std::ifstream f(path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}


int main()
{
    const std::string path = "/tmp/bw_test.txt";

    // Basic write + explicit flush
    {
        BufferedWriter bw(path);
        bw << "hello" << " " << "world\n";
        bw << 42 << "\n";
        bw.flush();
        assert(read_file(path) == "hello world\n42\n");
        std::cout << "PASS: basic write + explicit flush\n";
    }

    // Destructor flushes
    {
        BufferedWriter bw(path);
        bw << "dtor\n";
        // bw goes out of scope here — destructor must flush
    }
    assert(read_file(path) == "dtor\n");
    std::cout << "PASS: destructor flush\n";

    // Auto-flush at threshold (use threshold=10 bytes)
    {
        bool flushed_early = false;
        BufferedWriter bw(path, std::ios::out, 10);
        bw << "12345";                       // 5 bytes — no flush yet
        assert(read_file(path).empty());
        bw << "67890X";                      // crosses 10 — auto flush
        assert(!read_file(path).empty());
        std::cout << "PASS: auto-flush at threshold\n";
    }

    // Append mode
    {
        BufferedWriter bw(path, std::ios::out);
        bw << "line1\n";
    }
    {
        BufferedWriter bw(path, std::ios::app);
        bw << "line2\n";
    }
    assert(read_file(path) == "line1\nline2\n");
    std::cout << "PASS: append mode\n";

    // reopen()
    {
        const std::string path2 = "/tmp/bw_test2.txt";
        BufferedWriter bw(path);
        bw << "file1\n";
        bw.reopen(path2);
        bw << "file2\n";
        // destructor flushes file2
        assert(read_file(path) == "file1\n");   // checked after reopen flushed it
        // path2 checked after dtor
        (void)path2;
    }
    assert(read_file("/tmp/bw_test2.txt") == "file2\n");
    std::cout << "PASS: reopen\n";

    // Large data: many auto-flushes, verify byte count and content integrity
    {
        const std::string large_path = "/tmp/bw_test_large.txt";
        const int N = 1'000'000;
        std::size_t expected_bytes = 0;

        // Write in inner scope so destructor flushes before we read back
        {
            BufferedWriter bw(large_path, std::ios::out, 65536);
            for (int i = 0; i < N; ++i)
            {
                bw << i << "\n";
                int digits = (i == 0) ? 1 : (int)std::to_string(i).size();
                expected_bytes += digits + 1;
            }
        } // destructor flushes remainder here

        std::string content = read_file(large_path);
        assert(content.size() == expected_bytes);

        // Spot-check first, middle, last lines
        std::istringstream ss(content);
        std::string line;
        std::getline(ss, line); assert(line == "0");

        std::istringstream ss2(content);
        for (int i = 0; i <= N/2; ++i) std::getline(ss2, line);
        assert(line == std::to_string(N/2));

        // rfind is safer than while(getline) which runs an extra iteration on trailing '\n'
        auto last_nl = content.rfind('\n', content.size() - 2);
        assert(content.substr(last_nl + 1, content.size() - last_nl - 2) == std::to_string(N - 1));

        std::cout << "PASS: large data (" << expected_bytes / 1024 << " KB, " << N << " lines)\n";
    }

    std::cout << "All tests passed.\n";
    return 0;
}
