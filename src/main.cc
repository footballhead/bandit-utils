#include <banditutil/color.h>
#include <banditutil/file.h>
#include <banditutil/title.h>

#include <pdcurses/font437.h>
#include <stb_image/stb_image.h>
#include <stb_image_write/stb_image_write.h>

#include <array>
#include <iostream>

namespace {

constexpr auto color_black = color_t{0, 0, 0};
constexpr auto color_white = color_t{0xFF, 0xFF, 0xFF};

struct image_t {
    int width = 0;
    int height = 0;
    int channels = 3; // TODO we only support 3 channels

    std::vector<color_t> data;
};

template <typename Iterator>
image_t image_from_memory(Iterator beginI, Iterator endI)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    auto const font_img = stbi_load_from_memory(&(*beginI), std::distance(beginI, endI), &width, &height, &channels, 3);
    if (!font_img) {
        throw std::runtime_error("Font load failed");
    }

    if (channels != 3) {
        throw std::runtime_error("Unimplemented for channels <> 3");
    }

    std::vector<color_t> data;
    data.reserve(width * height);
    for (int i = 0; i < width * height * 3; i += 3) {
        data.push_back({font_img[i], font_img[i+1], font_img[i+2]});
    }

    return {width, height, channels, data};
}

image_t image_get_subregion(image_t const& img, int x, int y, int width, int height)
{
    std::vector<color_t> new_data;
    new_data.reserve(width * height);

    for (int yi = y; yi < y + height; ++yi) {
        for (int xi = x; xi < x + width; ++xi) {
            auto const color = img.data[yi * img.width + xi];
            new_data.push_back(color);
        }
    }

    return {width, height, img.channels, std::move(new_data)};
}

std::vector<image_t> image_to_spritesheet(image_t const& img, int cell_width, int cell_height)
{
    std::vector<image_t> builder;

    for (int y = 0; y < img.height; y += cell_height) {
        for (int x = 0; x < img.width; x += cell_width) {
            builder.emplace_back(image_get_subregion(img, x, y, cell_width, cell_height));
        }
    }

    return builder;
}

bool save_image(std::string const& file, image_t const& img)
{
    // This is sort of a cop-out
    // img.data.data() will probably wind up being laid out in memory (on x86) in such a way that we don't need to
    // serialize it into a raw buffer first.
    return stbi_write_png(file.c_str(), img.width, img.height, img.channels, img.data.data(), img.width * img.channels);
}

void image_blit(image_t const& src, image_t& dest, int dest_x, int dest_y, color_t const& foreground, color_t const& background)
{
    for (int y = 0; y < src.height; ++y) {
        for (int x = 0; x < src.width; ++x) {
            auto const color = src.data[y * src.width + x];
            dest.data[(y+dest_y) * dest.width + (x+dest_x)] = (color == color_white ? foreground : background);
        }
    }
}

image_t draw_title(title_t title, std::vector<image_t> const& spritesheet)
{
    // spritesheets are supposed to have constant dimensions and channels
    auto const& cell = spritesheet[0];
    image_t builder{cell.width * 80, cell.height * 25, cell.channels, std::vector<color_t>(cell.width * 80 * cell.height * 25, color_black)};

    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            auto i = y * 80 + x;
            auto const c = title.characters[i];
            image_blit(spritesheet[c], builder, x * 8, y * 16, title.foreground[i], title.background[i]);
        }
    }

    return builder;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " title.scr outfile\n";
        return 1;
    }

    auto const title_filename = argv[1];
    auto const out_filename = argv[2];

    auto const titles = titles_from_file(title_filename);

    auto const font_img = image_from_memory(font437, font437 + sizeof(font437));
    auto const spritesheet = image_to_spritesheet(font_img, 8, 16);
    auto const drawn_title = draw_title(titles[2], spritesheet);

    auto const did_save = save_image(out_filename, drawn_title);
    if (!did_save) {
        throw std::runtime_error("stbi_write_png failed");
    }

    return 0;
}
