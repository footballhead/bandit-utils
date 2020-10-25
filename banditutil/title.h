#pragma once

#include "color.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct title_t {
    std::array<uint8_t, 80*25> characters;
    std::array<color_t, 80*25> foreground;
    std::array<color_t, 80*25> background;
};

std::vector<uint8_t> title_rle_decode(std::vector<uint8_t> const& encoded);

std::vector<title_t> titles_from_file(std::string const& filename);
