/**
 * @class TrueTypeFont
 * @brief Allows to load and render to a bitmap a true type font
 *        It also implements a cache of pre-renderered letters for
 *        later use. The cache is empty upon construction
 *
 * @author	Roberto Cano (http://www.robertocano.es)
 */
#pragma once

#include "TrueTypeFont.hpp"

#include <stdint.h>
#include <string>
#include <vector>

// FreeType Headers
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H
#include FT_MODULE_H

class FreeTypeFont : public TrueTypeFont
{
  public:
    FreeTypeFont();
    ~FreeTypeFont();
    bool init(const std::string &font_path, uint32_t size);
    const uint8_t *getBitmap(char letter, uint32_t &width, uint32_t &height, uint32_t &offsetLeft, uint32_t &offsetTop, uint32_t &advance);

  private:
    uint32_t _size;
    FT_Library _ftLibrary;
    FT_Face _ftFace;
    std::vector<FT_BitmapGlyph> _ftGlyphCache;
    std::vector<uint32_t> _advanceCache;
};
