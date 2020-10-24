#include <banditutil/file.h>
#include <banditutil/title.h>

#include <iostream>

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

    auto const decoded = title_rle_decode(encoded);
    std::cout << "decoded Size = " << decoded.size() << "\n";

    dump_binary_file(out_filename, decoded);

    return 0;
}
