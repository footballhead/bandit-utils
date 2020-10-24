#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std::string_literals;

namespace {

std::vector<uint8_t> read_binary_file(std::string const& file)
{
    std::ifstream in(file, std::ios_base::binary);
    if (!in.good()) {
        throw std::runtime_error("Failed to open file: "s + file);
    }

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

void dump_binary_file(std::string const& file, std::vector<uint8_t> const& data)
{
    std::ofstream out(file);
    if (!out.good()) {
        throw std::runtime_error("Failed to open file: "s + file);
    }
    out.write(reinterpret_cast<char const*>(data.data()), data.size());
}

std::vector<uint8_t> rle_decode(std::vector<uint8_t> const& encoded)
{
    std::vector<uint8_t> decoded;
    for (int i = 0; i < encoded.size(); /*intentionally blank*/) {
        int8_t const c = encoded[i];
        i += 1;

        if (c == -128) {
            return decoded;
        }
        if (c < 0) {
            auto const cc = (-c) + 1;
            if (i+1 >= encoded.size()) {
                throw std::runtime_error("bad i");
            }

            decoded.insert(decoded.end(), cc, encoded[i]);
            i += 1;
        } else {
            auto const cc = c + 1;
            if (i+cc >= encoded.size()) {
                throw std::runtime_error("bad i");
            }

            auto copy_begin = std::next(encoded.begin(), i);
            auto copy_end = std::next(copy_begin, cc);
            decoded.insert(decoded.end(), copy_begin, copy_end);
            i += cc;
        }
    }

    std::cerr << "WARNING: didn't find terminating 0x80\n";
    return decoded;
}

}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " title.scr outfile\n";
        return 1;
    }

    auto const title_filename = argv[1];
    auto const out_filename = argv[2];

    auto const encoded = read_binary_file(title_filename);
    std::cout << "encoded Size = " << encoded.size() << "\n";

    auto const decoded = rle_decode(encoded);
    std::cout << "decoded Size = " << decoded.size() << "\n";

    dump_binary_file(out_filename, decoded);

    return 0;
}
