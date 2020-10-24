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

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " title.scr\n";
        return 1;
    }

    auto const title_filename = argv[1];

    auto const file = read_binary_file(title_filename);
    std::cout << "Size = " << file.size() << "\n";

    return 0;
}
