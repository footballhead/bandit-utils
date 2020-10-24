#include "title.h"

#include <iostream>
#include <iterator>
#include <stdexcept>

std::vector<uint8_t> title_rle_decode(std::vector<uint8_t> const& encoded)
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
