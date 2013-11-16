/* 
 *    Copyright (C) 2013 Torisugari <torisugari@gmail.com>
 *
 *     Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <cairo-ps.h>
#include <cairo-ft.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftadvanc.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h>

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

#include "printha.h"
using namespace printha;

#define HAGAKI_WIDTH_MM   100.0                          // 100mm
#define HAGAKI_HEIGHT_MM  148.0                          // 148mm
#define MM_PER_INCH       25.4                           // 1 inch = 24.5mm
#define PT_PER_INCH       72.0                           // 1 inch = 72pt
#define MM_TO_PT(_MM_) ((_MM_*PT_PER_INCH)/MM_PER_INCH)
#define PT_TO_MM(_PT_) ((_PT_*MM_PER_INCH)/PT_PER_INCH)
#define HAGAKI_WIDTH_PT   (MM_TO_PT(HAGAKI_WIDTH_MM))    // 283.464567pt 
#define HAGAKI_HEIGHT_PT  (MM_TO_PT(HAGAKI_HEIGHT_MM))   // 419.527559pt

#define X_RESOLUTION 72.                                 // dpi
#define Y_RESOLUTION 72.                                 // dpi

#define ZIPRECT_TICK_LINE_WIDTH (0.7/2)                  // 0.6mm - 0.8mm (JIS)
#define ZIPRECT_THIN_LINE_WIDTH (0.4/2)                  // 0.3mm - 0.5mm (JIS)

#ifdef DEBUG
void setBackground(cairo_surface_t* aCS) {
  cairo_surface_t* bg =
    cairo_image_surface_create_from_png(CMAKE_SOURCE_DIR "/resources/bg.png");
  int32_t width = cairo_image_surface_get_width(bg);
  int32_t height = cairo_image_surface_get_height(bg);

  cairo_t* c = cairo_create(aCS);
  cairo_scale(c, HAGAKI_WIDTH_PT/width, HAGAKI_HEIGHT_PT/height);
  cairo_set_source_surface(c, bg, 0, 0);
  cairo_paint(c);
  cairo_surface_destroy (bg);
  cairo_destroy(c);
}
#endif

void GetVerticalOriginFromRect(const rect_t& aRect, point_t* aOrigin) {
  aOrigin->mX = (aRect.mStart.mX + aRect.mEnd.mX) / 2;
  aOrigin->mY = aRect.mStart.mY;
}

void GetHorizontalOriginFromRect(const rect_t& aRect, point_t* aOrigin) {
  aOrigin->mX = aRect.mStart.mX;
  aOrigin->mY = (aRect.mStart.mY + aRect.mEnd.mY) / 2;
}

void GetRectFromVerticalOrigin(const point_t& aOrigin,
                               double aWidth, double aHeight,
                               rect_t* aRect) {
  aRect->mStart.mX = aOrigin.mX - (aWidth / 2);
  aRect->mStart.mY = aOrigin.mY;
  aRect->mEnd.mX = aOrigin.mX + (aWidth / 2);
  aRect->mEnd.mY = aOrigin.mY + aHeight;
}

void GetRectFromHorizontalOrigin(const point_t& aOrigin,
                                 double aWidth, double aHeight,
                                 rect_t* aRect) {
  aRect->mStart.mX = aOrigin.mX;
  aRect->mStart.mY = aOrigin.mY- (aHeight / 2);
  aRect->mEnd.mX = aOrigin.mX + aWidth;
  aRect->mEnd.mY = aOrigin.mY + (aHeight / 2);
}

void printZipcode(cairo_surface_t* aCS, const char* aZipcode, 
                  const char* aFontFace, float aFontsize,
                  const ziprect_t& aRects, FrameType aFrameType) {
  cairo_t* c = cairo_create(aCS);
  cairo_scale(c, PT_PER_INCH/MM_PER_INCH, PT_PER_INCH/MM_PER_INCH);

  const char* ptr = aZipcode;

  uint32_t i;
  for (i =0; i < 7; i++) {
    const point_t& start = aRects[i].mStart;
    const point_t& end = aRects[i].mEnd;

    if (aFrameType == DRAW_SENDTO) {
      cairo_set_source_rgb(c, 1., 0.3, 0.3);
      double linewidth = (i < 3)? ZIPRECT_TICK_LINE_WIDTH :
                                  ZIPRECT_THIN_LINE_WIDTH;

      cairo_set_line_width (c, linewidth);
      double width = end.mX - start.mX + (linewidth * 2);
      double height = end.mY - start.mY + (linewidth * 2);
      cairo_rectangle(c, start.mX - (linewidth),
                         start.mY - (linewidth),
                         width, height);
      cairo_stroke(c);
    }

    cairo_select_font_face(c, aFontFace,
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_status_t cs = cairo_status(c);
    if (cs) {
      fprintf(stderr, "cairo_select_font_face:%s",
                      cairo_status_to_string(cs));
    }
    cairo_set_source_rgb(c, 0., 0., 0.);
    cairo_set_font_size(c, PT_TO_MM(aFontsize));

    char buff = char(*ptr);
    
    if ((3 == i) && ('-' == buff)) {
      ptr++;
      buff = char(*ptr);
    }

    if (('0' <= buff) && (buff <= '9')) {
      ptr++;
    }
    else {
      break;
    }

    char number[] = "0";
    number[0] = buff;

    cairo_text_extents_t extents;
    cairo_text_extents (c, number, &extents);
    double x = ((start.mX + end.mX) / 2) -
               (extents.width/2 + extents.x_bearing);
    double y = ((start.mY + end.mY) / 2) -
               (extents.height/2 + extents.y_bearing);
    cairo_move_to(c, x, y);
    cairo_show_text(c, number);
  }

  if (i != 7 || (*ptr)) {
    fprintf(stderr,
            "Malformed ZIP code:%s\n"
            "It must be like 888-8888 or 8888888.\n",
            aZipcode);
  }

  //draw hyhpen
  if (aFrameType == DRAW_SENDTO) { 
    cairo_set_source_rgb(c, 1., 0.3, 0.3);
    cairo_set_line_width(c, ZIPRECT_TICK_LINE_WIDTH);
    cairo_move_to(c, aRects[2].mEnd.mX + ZIPRECT_TICK_LINE_WIDTH, 
                   (aRects[2].mStart.mY + aRects[2].mEnd.mY)/2);
    cairo_line_to(c, aRects[3].mStart.mX - ZIPRECT_THIN_LINE_WIDTH, 
                   (aRects[3].mStart.mY + aRects[3].mEnd.mY)/2);
    cairo_stroke(c);
  }
  else if (aFrameType == DRAW_SENDFROM){
    cairo_set_source_rgb(c, 1., 0.3, 0.3);
    cairo_set_line_width(c, ZIPRECT_THIN_LINE_WIDTH);
    double dashes[] = {.6, .5};
    cairo_set_dash (c, dashes, 2, 0.);

    cairo_rectangle(c, aRects[0].mStart.mX, aRects[0].mStart.mY,
                       aRects[0].width() * 3, aRects[0].height());
    cairo_rectangle(c, aRects[3].mStart.mX, aRects[3].mStart.mY,
                       aRects[3].width() * 4, aRects[3].height());

    cairo_move_to(c, aRects[0].mEnd.mX, aRects[0].mStart.mY);
    cairo_line_to(c, aRects[0].mEnd.mX, aRects[0].mEnd.mY);
    cairo_move_to(c, aRects[1].mEnd.mX, aRects[1].mStart.mY);
    cairo_line_to(c, aRects[1].mEnd.mX, aRects[1].mEnd.mY);
    cairo_move_to(c, aRects[3].mEnd.mX, aRects[3].mStart.mY);
    cairo_line_to(c, aRects[3].mEnd.mX, aRects[3].mEnd.mY);
    cairo_move_to(c, aRects[4].mEnd.mX, aRects[4].mStart.mY);
    cairo_line_to(c, aRects[4].mEnd.mX, aRects[4].mEnd.mY);
    cairo_move_to(c, aRects[5].mEnd.mX, aRects[5].mStart.mY);
    cairo_line_to(c, aRects[5].mEnd.mX, aRects[5].mEnd.mY);

    cairo_stroke(c);
  }
  cairo_destroy(c);
}

#ifdef DEBUG
void drawDebugRect(cairo_t* aC, const rect_t& aRect) {
  cairo_set_source_rgb(aC, 0.8, 0.8, 0.8);
  cairo_set_line_width(aC, 0.1);
  cairo_rectangle(aC, aRect.mStart.mX, aRect.mStart.mY,
                      aRect.width(), aRect.height());
  cairo_move_to(aC, aRect.mStart.mX, aRect.mStart.mY);
  cairo_line_to(aC, aRect.mEnd.mX, aRect.mEnd.mY);

  cairo_stroke(aC);
}
#endif

inline bool isWhiteSpace(const char* aString, uint32_t aCluster) {
  return ' ' == char(*(aString + aCluster));
}

inline int32_t hb_direction_to_cairo_direction(hb_direction_t aDirection) {
  switch (aDirection) {
  case HB_DIRECTION_TTB:
    return -1;
  case HB_DIRECTION_RTL:
  case HB_DIRECTION_LTR:
  case HB_DIRECTION_BTT:
    return 1;
  case HB_DIRECTION_INVALID:
    return 0;
  }
  return 0;
}

inline void 
reverseCairoCluster(cairo_text_cluster_t aBuff[], uint32_t aLength) {
  uint32_t length = aLength / 2;
  cairo_text_cluster_t temp;
  uint32_t i;
  for (i = 0; i < length; i++) {
    temp = aBuff[i];
    aBuff[i] = aBuff[aLength - i - 1];
    aBuff[aLength - i - 1] = temp;
  }
}

double printString(FT_Face aFTSeletedFont, cairo_surface_t* aCS,
                   const char* aString, const rect_t& aRect,
                   double& aMaxFontSize, double aWhiteSpace,
                   bool aStretch, bool aBottom = false, bool aRed = false,
                   hb_direction_t aDirection = HB_DIRECTION_TTB) {
  uint32_t strlength = strlen(aString);
  if (!strlength || aMaxFontSize <= 0) {
    return 0.;
  }
#ifdef DEBUG
  fprintf(stderr, "line:%s\n", aString);
#endif

  const bool isVertical = HB_DIRECTION_IS_VERTICAL(aDirection);
  const bool isBackward = HB_DIRECTION_IS_BACKWARD(aDirection);
  double absMaxAdvance = (isVertical)? aRect.height() : aRect.width();
  double linewidth = (isVertical)? aRect.width() : aRect.height();

  if (aMaxFontSize > linewidth)
    aMaxFontSize = linewidth;
  double fontsize = aMaxFontSize;

  double x_resolution, y_resolution;
  cairo_surface_get_fallback_resolution(aCS, &x_resolution, &y_resolution);
#ifdef DEBUG
  fprintf(stderr, "res:%f, %f\n", x_resolution, y_resolution);
#endif

  // [72 dot per inch] = [1 dot per point]
  FT_Error fte = FT_Set_Char_Size(aFTSeletedFont, 0, fontsize,
                                  FT_UInt(x_resolution), FT_UInt(y_resolution));

  if (fte) {
    fprintf(stderr, "FT_Set_Char_Size:0x%x\n", fte);
    exit(-1);
  }

  hb_font_t* hbSeletedFont = hb_ft_font_create(aFTSeletedFont, nullptr);


  hb_buffer_t* buff = hb_buffer_create();
  hb_buffer_set_unicode_funcs(buff, hb_icu_get_unicode_funcs());
  hb_buffer_set_direction(buff, aDirection);

  if (isVertical) {
    hb_buffer_set_script(buff, HB_SCRIPT_KATAKANA);
    hb_buffer_set_language(buff, hb_language_from_string("ja", -1));
  }

  hb_buffer_add_utf8(buff, aString, -1, 0, -1);
  hb_buffer_guess_segment_properties(buff);

  hb_shape(hbSeletedFont, buff, nullptr, 0);

  uint32_t length;
  hb_glyph_info_t* hbInfo = hb_buffer_get_glyph_infos(buff, &length);
  hb_glyph_position_t*
    hbPosition = hb_buffer_get_glyph_positions(buff, &length);


  // 26.6 fractional format. See FreeType's spec.
  const double kXFactor = 64.0 * x_resolution / PT_PER_INCH;
  const double kYFactor = 64.0 * y_resolution / PT_PER_INCH;
  const double& kAdvanceFactor = isVertical? kYFactor : kXFactor;
  const int32_t kHBAdvanceToCairoAdvance = 
    hb_direction_to_cairo_direction(aDirection);
  const int32_t kAbsWhiteSpaceAdvance = int32_t(kAdvanceFactor * aWhiteSpace);
  const int32_t kHBWhiteSpaceAdvance =
    kAbsWhiteSpaceAdvance * kHBAdvanceToCairoAdvance;
  int32_t hbTotalAdvance = 0;
  uint32_t i;
  for (i = 0; i < length; i++) {
#ifdef DEBUG
    fprintf(stderr, "\ni:%d\n", i);
    uint32_t start = hbInfo[i].cluster;
    uint32_t end = 0;
    if (isBackward) {
      end = (0 == i)? strlength : hbInfo[i - 1].cluster;
    }
    else {
      end = (length -1 == i)? strlength : hbInfo[i + 1].cluster;
    }
    char charbuff[40];
    if (end - start < sizeof(charbuff)) {
       memcpy(charbuff, aString + start, end - start);
       charbuff[end - start] = 0;
       fprintf(stderr, "str :%s\nutf8:0x ", charbuff);
       uint32_t j;
       for (j = 0; j < end -start; j++) {
       fprintf(stderr, "%2x ", (unsigned char)(charbuff[j]));
        }
       fprintf(stderr, "\n");
     }

    fprintf(stderr, "x_advance:%8d, y_advance: %8d\n"
            "x_offset :%8d, y_offset : %8d\n",
            hbPosition[i].x_advance, hbPosition[i].y_advance, 
            hbPosition[i].x_offset, hbPosition[i].y_offset);
    fprintf(stderr, "codepoint:0x%x, cluster:0x%x, mask:0x%x\n",
            hbInfo[i].codepoint, hbInfo[i].cluster, hbInfo[i].mask);
#endif
    int& glyphAdvance = isVertical? hbPosition[i].y_advance :
                                    hbPosition[i].x_advance;
    if (isWhiteSpace(aString, hbInfo[i].cluster)) {
      hbTotalAdvance += kHBWhiteSpaceAdvance;
    }
    else {
      hbTotalAdvance += glyphAdvance;
    }
  }

  while ((absMaxAdvance * kAdvanceFactor) < (abs(hbTotalAdvance) * fontsize)) {
    fontsize -= 1.0;
  }

  if (fontsize <= 0.) {
    hb_font_destroy(hbSeletedFont);
    hb_buffer_destroy(buff);
    return 0.;
  }
  // Feedback for next line.
  aMaxFontSize = fontsize;

  point_t origin;
  if (isVertical) {
    GetVerticalOriginFromRect(aRect, &origin);
  }
  else {
    GetHorizontalOriginFromRect(aRect, &origin);
  }

  double& origin_parallel_to_advance = isVertical? origin.mY : origin.mX;

  int32_t absPadding = 0;
  if (aMaxFontSize == fontsize) {
    int32_t left = ((absMaxAdvance * kAdvanceFactor / fontsize) -
               abs(hbTotalAdvance));
    if (aStretch) {
      absPadding = left / (length - 1);
    }
    else if (aBottom ^ isBackward) {
      origin_parallel_to_advance += left *  fontsize / kAdvanceFactor;
      absMaxAdvance -= left *  fontsize / kAdvanceFactor;
    }
  }

  fte = FT_Set_Char_Size(aFTSeletedFont, 0, fontsize,
                         FT_UInt(x_resolution), FT_UInt(y_resolution));
  if (fte) {
    fprintf(stderr, "FT_Set_Char_Size:0x%x\n", fte);
    exit(-1);
  }

  hb_font_destroy(hbSeletedFont);

  hb_segment_properties_t properties;
  hb_buffer_get_segment_properties(buff, &properties);
 
  hb_buffer_clear_contents(buff);

  hb_buffer_set_segment_properties(buff, &properties);
  hb_buffer_add_utf8(buff, aString, -1, 0, -1);

  hbSeletedFont = hb_ft_font_create(aFTSeletedFont, nullptr);
  hb_shape(hbSeletedFont, buff, nullptr, 0);

  hbInfo = hb_buffer_get_glyph_infos(buff, &length);
  hbPosition = hb_buffer_get_glyph_positions(buff, &length);

  cairo_t* ca = cairo_create(aCS);
#ifdef DEBUG
  fprintf(stderr, "length:%d, totalAdvance: %d\n", length, hbTotalAdvance);
  fprintf(stderr, "fontsize:%f\n", fontsize);
  fprintf(stderr, "padding:%d\n", absPadding);

  drawDebugRect(ca, aRect);

  rect_t textbox;
  if (isVertical) {
    GetRectFromVerticalOrigin(origin, fontsize, absMaxAdvance, &textbox);
  }
  else {
    GetRectFromHorizontalOrigin(origin, absMaxAdvance, fontsize, &textbox);
  }

  fprintf(stderr, "textbox:%f %f %f %f\n",
          textbox.mStart.mX, textbox.mStart.mY,
          textbox.mEnd.mX, textbox.mEnd.mY);

  drawDebugRect(ca, textbox);
#endif
  if (aRed) {
    cairo_set_source_rgb(ca, 1., 0.3, 0.3);
  } 
  else {
    cairo_set_source_rgb(ca, 0., 0., 0.);
  }

  cairo_font_face_t* caSeletedFont =
    cairo_ft_font_face_create_for_ft_face
     (aFTSeletedFont, isVertical? FT_LOAD_VERTICAL_LAYOUT : 0);

  cairo_set_font_face(ca, caSeletedFont);
  cairo_set_font_size(ca, fontsize);

  uint32_t glyphIndex = 0;
  const uint32_t kGlyphLength = 40;
  cairo_glyph_t glyphbuffer[kGlyphLength];

  cairo_text_cluster_t clusterbuffer[kGlyphLength];
  uint32_t clusterTotalLength = 0;
  const char* clusterStr(nullptr);
  cairo_text_cluster_flags_t clusterFlag = (isBackward)?
    CAIRO_TEXT_CLUSTER_FLAG_BACKWARD : cairo_text_cluster_flags_t(0);

  if (!isVertical) {
#ifdef DEBUG
    fprintf(stderr, "FT_Face: a %d, d %d, h %d\n",
            aFTSeletedFont->ascender,
            aFTSeletedFont->descender,
            aFTSeletedFont->height);
#endif
    origin.mY += fontsize * (double(aFTSeletedFont->ascender) / 
                 double(aFTSeletedFont->height) - 0.5);
  }

  for (i = 0; i < length; i++) {
    if (glyphIndex == 0) {
      if (!isBackward) {
        clusterStr = aString + hbInfo[i].cluster;
      }
      clusterTotalLength = 0;
    }

    if (isWhiteSpace(aString, hbInfo[i].cluster)) {
      if (glyphIndex) {
        if (isBackward) {
          reverseCairoCluster(clusterbuffer, glyphIndex);
        }
        cairo_show_text_glyphs(ca, clusterStr, clusterTotalLength,
                               glyphbuffer, glyphIndex,
                               clusterbuffer, glyphIndex, clusterFlag);
      }
      // We'd rather like to go just forward
      // than to make cairo render "white-space" glyph.
      origin_parallel_to_advance +=
        (kAbsWhiteSpaceAdvance + absPadding) * fontsize / kAdvanceFactor;
      glyphIndex = 0;
      continue;
    }

    glyphbuffer[glyphIndex].index = hbInfo[i].codepoint;
    glyphbuffer[glyphIndex].x = origin.mX;
    glyphbuffer[glyphIndex].y = origin.mY;

    if (!isVertical) {
      glyphbuffer[glyphIndex].x += (fontsize * hbPosition[i].x_offset) /
                                     kXFactor;
      glyphbuffer[glyphIndex].y -= (fontsize * hbPosition[i].y_offset) /
                                     kYFactor;
    }

    uint32_t cluster_start = hbInfo[i].cluster;
    uint32_t cluster_next = 0;
    if (isBackward) {
      cluster_next = (0 == i)? strlength : hbInfo[i - 1].cluster;
      clusterStr = aString + hbInfo[i].cluster;
    }
    else {
      cluster_next = (length - 1 == i)? strlength : hbInfo[i + 1].cluster;
    }
    uint32_t clusterbytes = cluster_next - cluster_start;
    clusterbuffer[glyphIndex].num_bytes = clusterbytes;
    clusterbuffer[glyphIndex].num_glyphs = 1;
    clusterTotalLength += clusterbytes;

    origin.mX += (fontsize * hbPosition[i].x_advance) / kXFactor;
    origin.mY -= (fontsize * hbPosition[i].y_advance) / kYFactor;
    origin_parallel_to_advance += fontsize * absPadding / kAdvanceFactor;

    glyphIndex++;

    if (kGlyphLength == glyphIndex) {
      if (isBackward) {
        reverseCairoCluster(clusterbuffer, glyphIndex);
      }
      cairo_show_text_glyphs(ca, clusterStr, clusterTotalLength,
                             glyphbuffer, glyphIndex,
                             clusterbuffer, glyphIndex, clusterFlag);
      glyphIndex = 0;
    }
  }

  if (glyphIndex) {
    if (isBackward) {
      reverseCairoCluster(clusterbuffer, glyphIndex);
    }

    cairo_show_text_glyphs(ca, clusterStr, clusterTotalLength,
                           glyphbuffer, glyphIndex,
                           clusterbuffer, glyphIndex, clusterFlag);
  }
  cairo_surface_flush(aCS);

  cairo_destroy(ca);
  cairo_font_face_destroy(caSeletedFont);
  hb_font_destroy(hbSeletedFont);
  hb_buffer_destroy(buff);

  return origin.mY - aRect.mStart.mY;
}

inline uint32_t countLineNumber(const char* aString, const char aDelimiter) {
  uint32_t count = 1;
  while (*aString) {
    if (aDelimiter == (*aString)) {
      count++;
    }
    aString++;
  }
  return count;
}

void printLines(FT_Face aFTSeletedFont, cairo_surface_t* aCS,
                const std::string& aString, const frameformat_t& aFrame) {
  if (aFrame.rect.width() <= 0. || aFrame.rect.height() <= 0.) {
    return;
  }

  double maxFontSize = aFrame.fontsize;
  uint32_t count = countLineNumber(aString.c_str(), aFrame.linebreak);
  if (1 == count) {
    printString(aFTSeletedFont, aCS, aString.c_str(), aFrame.rect,
                maxFontSize, aFrame.whitespace, aFrame.stretch);
    return;
  }

  double width = aFrame.rect.width() / count;
  double height = aFrame.rect.height();

  point_t start(aFrame.rect.mEnd.mX - width, aFrame.rect.mStart.mY);
  bool isFirstLine = true;

  std::istringstream iss(aString);
  while (iss) {
    std::string line;
    std::getline(iss, line, aFrame.linebreak);

    rect_t linerect(start, width, height);
    double advance = printString(aFTSeletedFont, aCS, line.c_str(), linerect,
                                 maxFontSize, aFrame.whitespace, aFrame.stretch,
                                 aFrame.bottom && (!isFirstLine));
    if (isFirstLine) {
      if (aFrame.bottom) {
        height = advance;
      }
      isFirstLine = false;
    }
    start.mX -= width;
  }
}

void paint(FT_Face aFTSeletedFont, cairo_surface_t* aCS,
           const std::string& aArgv, const textformat_t& aSettings) {
#ifdef DEBUG
  setBackground(aCS);
#endif
  // See: <http://www.post.japanpost.jp/zipcode/zipmanual/p05.html>
  // unit: milimeter
  static const ziprect_t zipRects = {
    rect_t(point_t(44.3, 12.0), point_t(50.0, 20.0)),
    rect_t(point_t(51.3, 12.0), point_t(57.0, 20.0)), // +7.0mm
    rect_t(point_t(58.3, 12.0), point_t(64.0, 20.0)), // +7.0mm
    rect_t(point_t(65.9, 12.0), point_t(71.6, 20.0)), // +7.6mm
    rect_t(point_t(72.7, 12.0), point_t(78.4, 20.0)), // +6.8mm
    rect_t(point_t(79.5, 12.0), point_t(85.2, 20.0)), // +6.8mm
    rect_t(point_t(86.3, 12.0), point_t(92.0, 20.0))  // +6.8mm
  };

  static const ziprect_t zipRects2 = {
    rect_t(point_t(6., 122.0), 4., 7.),
    rect_t(point_t(10., 122.0), 4., 7.),
    rect_t(point_t(14., 122.0), 4., 7.),
    rect_t(point_t(19., 122.0), 4., 7.),
    rect_t(point_t(23., 122.0), 4., 7.),
    rect_t(point_t(27., 122.0), 4., 7.),
    rect_t(point_t(31., 122.0), 4., 7.),
  };

  std::string token;
  const int kExtraSize = sizeof(personformat_t::extra) / 
                           sizeof(personformat_t::extra[0]);

  if (!aArgv.empty()) {
    std::istringstream iss(aArgv);
    if (iss && !iss.eof()) {
      std::getline(iss, token, aSettings.sendto.dlmt);
      printLines(aFTSeletedFont, aCS, token, aSettings.sendto.name);
    }

    if (iss && !iss.eof()) {
      std::getline(iss, token, aSettings.sendto.dlmt);
      printZipcode(aCS, token.c_str(),
                   aSettings.zipfont.c_str(), aSettings.sendto.zipfontsize, 
                   zipRects, aSettings.sendto.drawzipframe? DRAW_SENDTO :
                                                           NO_FRAME);
    }

    if (iss && !iss.eof()) {
      std::getline(iss, token, aSettings.sendto.dlmt);
      printLines(aFTSeletedFont, aCS, token, aSettings.sendto.addr);
    }

    int i;
    for (i = 0; i < kExtraSize; i++) {
      if (!iss || iss.eof()) {
        break;
      }
      std::getline(iss, token, aSettings.sendto.dlmt);
      printLines(aFTSeletedFont, aCS, token, aSettings.sendto.extra[i]);
    }
  }

  std::ifstream file(aSettings.sendfrompath.c_str());
  if (!file) {
    if (!aSettings.sendfrompath.empty()) {
      fprintf(stderr, "Missing file:%s\n", aSettings.sendfrompath.c_str());
    }
  }
  else {
    if (file) {
      std::getline(file, token, aSettings.sendfrom.dlmt);
      printLines(aFTSeletedFont, aCS, token, aSettings.sendfrom.name);
    }

    if (file) {
      std::getline(file, token, aSettings.sendfrom.dlmt);
      printZipcode(aCS, token.c_str(),
                   aSettings.zipfont.c_str(), aSettings.sendfrom.zipfontsize,
                   zipRects2, aSettings.sendfrom.drawzipframe? DRAW_SENDFROM :
                                                              NO_FRAME);
    }

    if (file) {
      std::getline(file, token, aSettings.sendfrom.dlmt);
      printLines(aFTSeletedFont, aCS, token, aSettings.sendfrom.addr);
    }

    int i;
    for (i = 0; i < kExtraSize; i++) {
      if (!file || file.eof()) {
        break;
      }
      std::getline(file, token, aSettings.sendfrom.dlmt);
      printLines(aFTSeletedFont, aCS, token, aSettings.sendfrom.extra[i]);
    }
  }

  if (aSettings.drawnenga) {
    double fontsize = 14.;
    printString(aFTSeletedFont, aCS, "\xe5\xb9\xb4\xe8\xb3\x80",
                rect_t(27., 103., 66., 117.),
                fontsize, 0., true, false, true, HB_DIRECTION_LTR);
  }

  { // Setup PDF's pages.
    cairo_t* c = cairo_create(aCS);
    cairo_scale(c, PT_PER_INCH/MM_PER_INCH, PT_PER_INCH/MM_PER_INCH);
#ifdef DEBUG
    drawDebugRect(c, zipRects2[0]);
    drawDebugRect(c, zipRects2[1]);
    drawDebugRect(c, zipRects2[2]);
    drawDebugRect(c, zipRects2[3]);
    drawDebugRect(c, zipRects2[4]);
    drawDebugRect(c, zipRects2[5]);
    drawDebugRect(c, zipRects2[6]);
#endif
    cairo_show_page(c);
    cairo_destroy(c);
  }
}

cairo_status_t caStdout(void *aClosure,
                        const unsigned char *aData,
                        unsigned int aLength){
  for (unsigned int i = 0; i < aLength; i++) {
    std::cout << char(*(aData+i));
  }
  return CAIRO_STATUS_SUCCESS;
}

inline bool fileExists(const char* aFilePath) {
  std::ifstream file(aFilePath);
  return bool(file);
}

inline bool fileCopy(const char* aSource, const char* aDest) {
  std::ifstream input(aSource);
  std::ofstream output(aDest);
  output << input.rdbuf();
  return (bool(output) && bool(input));
}

//#include <sys/stat.h>
int main (int argc, char* argv[]) {

  // The candidate is either ${CMAKE_SOURCE_DIR}/config.txt or
  // /user/local/share/printha/config.txt
  static const char kBuildDirConfigFile[] =
    CMAKE_SOURCE_DIR "/settings/config.txt"; 
  static const char kBuildDirSendFromFile[] =
    CMAKE_SOURCE_DIR "/settings/sendfrom.txt";
  static const char kBuildDirSendToFile[] =
    CMAKE_SOURCE_DIR "/settings/sendto.txt";
  static const char kBuildDirFontFile[] =
    CMAKE_SOURCE_DIR "/resources/ipaexm00201/ipaexm.ttf"; 

  textformat_t settings;

  settings::read("printha.config.log.txt", settings);
  bool isBuildDirConfig =  settings::read(kBuildDirConfigFile, settings);
  if (isBuildDirConfig) {
#ifdef DEBUG
    fprintf(stderr, "Successfully loaded config file:%s\n",
                    kBuildDirConfigFile);
    settings::write(CMAKE_SOURCE_DIR "/src/printha_init.cpp", settings,
                    settings::kWriteFormatCSRC);
#endif
  }
  else {
    fprintf(stderr, "Missing file:%s\n", kBuildDirConfigFile);
  }

  char fileNameBuffer[FILENAME_MAX];
  // XXX realpath(...) may cause buffer overflow, I'm afraid.
  realpath("printha.config.txt", fileNameBuffer);
  std::string userDirConfig = fileNameBuffer;

  bool isUserDirConfig = settings::read(userDirConfig.c_str(), settings);
  if (isUserDirConfig) {
#ifdef DEBUG
    fprintf(stderr, "Successfully loaded config file:%s\n",
                    userDirConfig.c_str());
#endif
  }
  else {
    fprintf(stderr, "Missing file:%s\n", userDirConfig.c_str());
  }

  int32_t i;
  std::string sendtoData;
  bool skipPrint = false;
  enum PrintMode {
    kPDF,
    kSVG,
    kPS
  };
  PrintMode printMode = kPDF;
  for (i = 1; i < argc; i++) {
    if ((0 == strcasecmp("-h", argv[i])) ||
        (0 == strcasecmp("--help", argv[i]))) {
      printf(
"Usage: printha [OPTION...] [DATA]\n"
"\n"
"Options:\n"
"  -h, --help            : Display this help document.\n"
"  -i, --init            : Export clean <printha.config.txt> to the current \n"
"                          working directory.\n"
"                          If <sendfrom.txt> and <sendto.txt> are missing,\n"
"                          they will also be created.\n"
"  -s, --sendfrom <file> : Set the location of the text file which contains\n"
"                          information about your name, zipcode, adress etc.\n"
"                          And this command updates <printha.config.txt>.\n"
"  -o, --output <file>   : Set PDF file location instead of stdout and\n"
"                          upadates <printha.config.txt>.\n"
"  --import <file>       : Import settings from <file>, but temporally unlike\n"
"                          -s or -o.\n"
"  --export <file>       : Export current settings to <file>.\n"
"  --svg                 : Temporally output data as a SVG file.\n"
"  --ps                  : Temporally output data as a PostScript file.\n"
      );
      skipPrint = true;
    }
    else if ((0 == strcasecmp("-i", argv[i])) ||
        (0 == strcasecmp("--init", argv[i]))) {
      settings::read(kBuildDirConfigFile, settings);
      settings.fontpath = kBuildDirFontFile;

      realpath("sendfrom.txt", fileNameBuffer);
      settings.sendfrompath = fileNameBuffer;
      if (!fileExists(fileNameBuffer)) {
        fprintf(stderr, "Creating file:%s\n", fileNameBuffer);
        fileCopy(kBuildDirSendFromFile, fileNameBuffer);
      }

      realpath("sendto.txt", fileNameBuffer);
      if (!fileExists(fileNameBuffer)) {
        fprintf(stderr, "Creating file:%s\n", fileNameBuffer);
        fileCopy(kBuildDirSendToFile, fileNameBuffer);
      }

      fprintf(stderr, "Saving settings at:%s\n", userDirConfig.c_str());
      settings::write(userDirConfig.c_str(), settings);

      skipPrint = true;
    }
    else if ((0 == strcasecmp("-s", argv[i])) ||
        (0 == strncasecmp("--sendfrom", argv[i], strlen("--sendfrom")))) {
      i++;
      if (i < argc) {
        if (0 != char(*argv[i])) {
          realpath(argv[i], fileNameBuffer);
          settings.sendfrompath = fileNameBuffer;
        }
        else {
          settings.sendfrompath.erase();
        }

        fprintf(stderr, "Saving settings at:%s\n",
                        userDirConfig.c_str());
        settings::write(userDirConfig.c_str(), settings);
        skipPrint = true;
      }
      else {
        fprintf(stderr, "Syntax Error. Not enough args: %s\n", argv[i-1]);
        exit(-1);
      }
    }
    else if ((0 == strcasecmp("-o", argv[i])) ||
        (0 == strcasecmp("--output", argv[i]))) {
      i++;
      if (i < argc) {
        if (0 != char(*argv[i])) {
          realpath(argv[i], fileNameBuffer);
          settings.outputpath = fileNameBuffer;
        }
        else {
          settings.outputpath.erase();
        }

        fprintf(stderr, "Saving settings at:%s\n",
                        userDirConfig.c_str());
        settings::write(userDirConfig.c_str(), settings);
        skipPrint = true;
      }
      else {
        fprintf(stderr, "Syntax Error. Not enough args: %s\n", argv[i-1]); 
        exit(-1);
      }
    }
    else if (0 == strcasecmp("--import", argv[i])) {
      i++;
      if (i < argc) {
        if (0 != char(*argv[i])) {
          realpath(argv[i], fileNameBuffer);
          isUserDirConfig = settings::read(fileNameBuffer, settings);
          if (isUserDirConfig) {
#ifdef DEBUG
            fprintf(stderr, "Successfully loaded config file:%s\n",
                    fileNameBuffer);
#endif
          }
          else {
            fprintf(stderr, "Missing file:%s\n", fileNameBuffer);
          }
        }
      }
      else {
        fprintf(stderr, "Syntax Error. Not enough args: %s\n", argv[i-1]); 
        exit(-1);
      }
    }
    else if (0 == strcasecmp("--export", argv[i])) {
      i++;
      if (i < argc) {
        if (0 != char(*argv[i])) {
          realpath(argv[i], fileNameBuffer);
          settings::write(fileNameBuffer, settings);
        }
      }
      else {
        fprintf(stderr, "Syntax Error. Not enough args: %s\n", argv[i-1]); 
        exit(-1);
      }
    }
    else if (0 == strncasecmp("--svg", argv[i], strlen("--svg"))) {
      printMode = kSVG;
    }
    else if (0 == strncasecmp("--ps", argv[i], strlen("--ps"))) {
      printMode = kPS;
    }
    else {
      sendtoData = argv[i];
      skipPrint = false;
    }
  }

  if (skipPrint) {
    exit(0);
  }

  cairo_surface_t* cs;
  if (!settings.outputpath.empty()) {
    switch(printMode) {
    case kSVG:
      cs = cairo_svg_surface_create(settings.outputpath.c_str(),
                                    HAGAKI_WIDTH_PT, HAGAKI_HEIGHT_PT);
      break;
    case kPS:
      cs = cairo_ps_surface_create(settings.outputpath.c_str(),
                                   HAGAKI_WIDTH_PT, HAGAKI_HEIGHT_PT);
      break;
    default:
      cs = cairo_pdf_surface_create(settings.outputpath.c_str(),
                                    HAGAKI_WIDTH_PT, HAGAKI_HEIGHT_PT);
    }
  }
  else {
    switch(printMode) {
    case kSVG:
      cs = cairo_svg_surface_create_for_stream(caStdout, nullptr,
                                               HAGAKI_WIDTH_PT,
                                               HAGAKI_HEIGHT_PT);
      break;
    case kPS:
      cs = cairo_ps_surface_create_for_stream(caStdout, nullptr,
                                              HAGAKI_WIDTH_PT,
                                              HAGAKI_HEIGHT_PT);
      break;
    default:
      cs = cairo_pdf_surface_create_for_stream(caStdout, nullptr,
                                               HAGAKI_WIDTH_PT,
                                               HAGAKI_HEIGHT_PT);
    }
  }
  cairo_surface_set_fallback_resolution(cs, X_RESOLUTION, Y_RESOLUTION);

  FT_Library ftlib;
  FT_Error fte = FT_Init_FreeType(&ftlib);

  if (fte) {
    fprintf(stderr, "FT_Init_FreeType:0x%x\n", fte);
    exit(-1);
  }

  FT_Face ftSeletedFont;
  fte = FT_New_Face(ftlib, settings.fontpath.c_str(), 0, &ftSeletedFont);

  if (fte) {
    fprintf(stderr, "FT_New_Face:0x%x, %s\n", fte, settings.fontpath.c_str());
    exit(-1);
  }

  if (sendtoData.empty()) {
    std::cin >> std::noskipws;
    std::getline(std::cin, sendtoData, char(0));

    // XXX I'm not too sure what inserts this line feed. Shell?
    //     Cut it off anyway.
    if (sendtoData.size() > 0 && '\n' == char(*(sendtoData.end() - 1))) {
      sendtoData.resize(sendtoData.size() - 1);
    }
  }

  if (!sendtoData.empty()) {
    if (kSVG == printMode) {
      paint(ftSeletedFont, cs, sendtoData, settings);
    }
    else {
      std::istringstream iss(sendtoData);
      std::string row;
      while (std::getline(iss, row, settings.pagedelimiter)) {
        paint(ftSeletedFont, cs, row, settings);
      }
    }
  }

  cairo_surface_flush(cs);
  cairo_surface_destroy(cs);
  FT_Done_FreeType(ftlib);
  return 0;
}
