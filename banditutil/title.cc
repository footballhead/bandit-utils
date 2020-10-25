#include "title.h"

#include "file.h"
#include "palette.h"

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

std::vector<title_t> titles_from_file(std::string const& filename)
{
    auto const decoded = title_rle_decode(read_binary_file(filename));

    std::vector<title_t> builder(3);

    for (auto screen = 0; screen < 3; ++screen) {
        auto i = 0;
        auto base = screen * 2 * 82 * 25;

        for (auto y = 0; y < 25; ++y) {
            for (auto x = 0; x < 82; ++x) {
                if (x == 80 || x == 81) {
                    continue;
                }

                builder[screen].characters[i] = decoded[base + (y * 82) + x];
                ++i;
            }
        }

        i = 0;
        for (auto y = 0; y < 25; ++y) {
            for (auto x = 0; x < 82; ++x) {
                if (x == 80 || x == 81) {
                    continue;
                }

                auto const attribute = decoded[base + 82 * 25 + (y * 82) + x];
                auto const foreground = attribute & 0xF;
                auto const background = (attribute >> 4) & 0x7;
                builder[screen].foreground[i] = ega_palette[foreground];
                builder[screen].background[i] = ega_palette[background];
                ++i;
            }
        }
    }

    return builder;
}
