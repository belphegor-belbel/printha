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
#include "printha.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <fstream>
#include <string>
#include <cstring>

// Start MACRO HELL.
#define PRINTHA_WRITE_CPP_RECT(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, \
        "  %s = rect_t(%f, %f, %f, %f);\n", \
        #__KEY__, \
        __KEY__.mStart.mX, \
        __KEY__.mStart.mY, \
        __KEY__.mEnd.mX, \
        __KEY__.mEnd.mY \
       )

#define PRINTHA_WRITE_CPP_POINT(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, \
        "  %s = point_t(%f, %f);\n", \
        #__KEY__, \
        __KEY__.mX, \
        __KEY__.mY \
       )

#define PRINTHA_WRITE_CPP_DOUBLE(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "  %s = %f;\n", #__KEY__, __KEY__)

#define PRINTHA_WRITE_CPP_BOOLEAN(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "  %s = %s;\n", #__KEY__ , __KEY__? "true " : "false")

#define PRINTHA_WRITE_CPP_STRING(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "  %s = \"%s\";\n", #__KEY__ , __KEY__.c_str())

#define PRINTHA_WRITE_CPP_CHAR(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "  %s = \'%s\';\n", #__KEY__ , cescape(__KEY__, strbuff))

#define PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__) \
  PRINTHA_WRITE_CPP_RECT(__FILENAME__,__KEY__.rect); \
  PRINTHA_WRITE_CPP_DOUBLE(__FILENAME__,__KEY__.fontsize); \
  PRINTHA_WRITE_CPP_DOUBLE(__FILENAME__,__KEY__.whitespace); \
  PRINTHA_WRITE_CPP_BOOLEAN(__FILENAME__,__KEY__.stretch); \
  PRINTHA_WRITE_CPP_BOOLEAN(__FILENAME__,__KEY__.bottom); \
  PRINTHA_WRITE_CPP_CHAR(__FILENAME__,__KEY__.linebreak)

#define PRINTHA_WRITE_CPP_PERSON(__FILENAME__,__KEY__) \
  PRINTHA_WRITE_CPP_CHAR(__FILENAME__ , __KEY__.dlmt); \
  PRINTHA_WRITE_CPP_DOUBLE(__FILENAME__ , __KEY__.zipfontsize); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.name); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.addr); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.extra[0]); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.extra[1]); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.extra[2]); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.extra[3]); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.extra[4]); \
  PRINTHA_WRITE_CPP_FRAME(__FILENAME__,__KEY__.extra[5]); \
  PRINTHA_WRITE_CPP_BOOLEAN(__FILENAME__,__KEY__.drawzipframe)

#define PRINTHA_WRITE_TXT_RECT(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, \
        "%-30s %f %f %f %f\n", \
        #__KEY__, \
        __KEY__.mStart.mX, \
        __KEY__.mStart.mY, \
        __KEY__.mEnd.mX, \
        __KEY__.mEnd.mY \
       )

#define PRINTHA_WRITE_TXT_POINT(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, \
        "%-30s %f %f\n", \
        #__KEY__, \
        __KEY__.mX, \
        __KEY__.mY \
       )

#define PRINTHA_WRITE_TXT_DOUBLE(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "%-30s %f\n", #__KEY__, __KEY__)

#define PRINTHA_WRITE_TXT_BOOLEAN(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "%-30s %s\n", #__KEY__ , __KEY__? "true " : "false")

#define PRINTHA_WRITE_TXT_STRING(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "%-30s %s\n", #__KEY__ , __KEY__.c_str())

#define PRINTHA_WRITE_TXT_CHAR(__FILENAME__,__KEY__) \
  flogf(__FILENAME__, "%-30s %s\n", #__KEY__ , escape(__KEY__, strbuff))

#define PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__) \
  PRINTHA_WRITE_TXT_RECT(__FILENAME__,__KEY__.rect); \
  PRINTHA_WRITE_TXT_DOUBLE(__FILENAME__,__KEY__.fontsize); \
  PRINTHA_WRITE_TXT_DOUBLE(__FILENAME__,__KEY__.whitespace); \
  PRINTHA_WRITE_TXT_BOOLEAN(__FILENAME__,__KEY__.stretch); \
  PRINTHA_WRITE_TXT_BOOLEAN(__FILENAME__,__KEY__.bottom); \
  PRINTHA_WRITE_TXT_CHAR(__FILENAME__,__KEY__.linebreak)

#define PRINTHA_WRITE_TXT_PERSON(__FILENAME__,__KEY__) \
  PRINTHA_WRITE_TXT_CHAR(__FILENAME__ , __KEY__.dlmt); \
  PRINTHA_WRITE_TXT_DOUBLE(__FILENAME__ , __KEY__.zipfontsize); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.name); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.addr); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.extra[0]); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.extra[1]); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.extra[2]); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.extra[3]); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.extra[4]); \
  PRINTHA_WRITE_TXT_FRAME(__FILENAME__,__KEY__.extra[5]); \
  PRINTHA_WRITE_TXT_BOOLEAN(__FILENAME__,__KEY__.drawzipframe)

#define PRINTHA_READ_TXT_RECT(__KEY__) \
    else  if (0 == strncasecmp(#__KEY__, line.c_str(), \
                                    int(std::strlen(#__KEY__) - 1))) { \
      std::sscanf(line.c_str(), "%30s %lf %lf %lf %lf", buff, \
                  &__KEY__.mStart.mX, \
                  &__KEY__.mStart.mY, \
                  &__KEY__.mEnd.mX, \
                  &__KEY__.mEnd.mY); \
    }
#define PRINTHA_READ_TXT_POINT(__KEY__) \
    else  if (0 == strncasecmp(#__KEY__, line.c_str(), \
                                    int(std::strlen(#__KEY__) - 1))) { \
      std::sscanf(line.c_str(), "%30s %lf %lf", buff, \
                  &__KEY__.mX, \
                  &__KEY__.mY); \
    }
#define PRINTHA_READ_TXT_DOUBLE(__KEY__) \
    else  if (0 == strncasecmp(#__KEY__, line.c_str(), \
                                    int(std::strlen(#__KEY__) - 1))) { \
      std::sscanf(line.c_str(), "%30s %lf", buff, &__KEY__);\
    }
#define PRINTHA_READ_TXT_BOOLEAN(__KEY__) \
    else  if (0 == strncasecmp(#__KEY__, line.c_str(), \
                                    int(std::strlen(#__KEY__) - 1))) { \
      std::sscanf(line.c_str(), "%30s %5s", buff, data);\
      if (0 == strncasecmp("true", data, 4)) { \
        __KEY__ = true; \
      } \
      else if (0 == strncasecmp("false", data, 5)) { \
        __KEY__ = false; \
      } \
    }
#define PRINTHA_READ_TXT_STRING(__KEY__) \
    else  if (0 == strncasecmp(#__KEY__, line.c_str(), \
                                    int(std::strlen(#__KEY__) - 1))) { \
      read = std::sscanf(line.c_str(), "%30s %127s", buff, data);\
      if (read == 2) { \
        __KEY__ = data; \
      }\
      else {\
        __KEY__ = ""; \
      } \
    }

#define PRINTHA_READ_TXT_CHAR(__KEY__) \
    else  if (0 == strncasecmp(#__KEY__, line.c_str(), \
                                    int(std::strlen(#__KEY__) - 1))) { \
      read = std::sscanf(line.c_str(), "%30s %127s", buff, data);\
      if (read == 2) { \
        __KEY__ = unescape(data); \
      }\
      else {\
        __KEY__ = '\0'; \
      } \
    }

#define    PRINTHA_READ_TXT_FRAME(__KEY__) \
    PRINTHA_READ_TXT_RECT(__KEY__.rect) \
    PRINTHA_READ_TXT_DOUBLE(__KEY__.fontsize) \
    PRINTHA_READ_TXT_DOUBLE(__KEY__.whitespace) \
    PRINTHA_READ_TXT_BOOLEAN(__KEY__.stretch) \
    PRINTHA_READ_TXT_BOOLEAN(__KEY__.bottom) \
    PRINTHA_READ_TXT_CHAR(__KEY__.linebreak) 

#define    PRINTHA_READ_TXT_PERSON(__KEY__) \
    PRINTHA_READ_TXT_CHAR(__KEY__.dlmt) \
    PRINTHA_READ_TXT_DOUBLE(__KEY__.zipfontsize) \
    PRINTHA_READ_TXT_FRAME(__KEY__.name) \
    PRINTHA_READ_TXT_FRAME(__KEY__.addr) \
    PRINTHA_READ_TXT_FRAME(__KEY__.extra[0]) \
    PRINTHA_READ_TXT_FRAME(__KEY__.extra[1]) \
    PRINTHA_READ_TXT_FRAME(__KEY__.extra[2]) \
    PRINTHA_READ_TXT_FRAME(__KEY__.extra[3]) \
    PRINTHA_READ_TXT_FRAME(__KEY__.extra[4]) \
    PRINTHA_READ_TXT_FRAME(__KEY__.extra[5]) \
    PRINTHA_READ_TXT_BOOLEAN(__KEY__.drawzipframe)

inline void flogf(const char* aFilename, const char* aFormat, ...) {
  // This is a little dangerous for compiler doesn't 
  // check arg types.
  FILE* fp = fopen(aFilename, "a");
  if (!fp) {
    perror(aFilename);
    return;
  }

  va_list args;
  va_start(args, aFormat);
  int count = vfprintf(fp, aFormat, args);
  va_end(args);

  if (count < 0) {
    perror(aFilename);
  }
  fclose(fp);
}

namespace printha {
namespace settings {

char unescape(const char* aStr) {
  char ret;
  if ('\\' == char(*aStr)) {
    char c = char(*(aStr + 1));
    switch (c) {
    case 'a': ret = '\a'; break;
    case 'b': ret = '\b'; break;
    case 'f': ret = '\f'; break;
    case 'n': ret = '\n'; break;
    case 'r': ret = '\r'; break;
    case 's': ret = ' '; break;
    case 't': ret = '\t'; break;
    case 'v': ret = '\v'; break;
    case '\\': ret = '\\'; break;
    case '\0': ret = '\\'; break;
    default: ret = c;
    }
  } else {
    ret = char(*aStr);
  }
  return ret;
}

const char* cescape(const char aC, std::string& aBuffer) {
  switch (aC & 0x7f) {
  case '\a': aBuffer = "\\a"; break;
  case '\b': aBuffer = "\\b"; break;
  case '\f': aBuffer = "\\f"; break;
  case '\n': aBuffer = "\\n"; break;
  case '\r': aBuffer = "\\r"; break;
  case '\t': aBuffer = "\\t"; break;
  case '\v': aBuffer = "\\v"; break;
  case '\\': aBuffer = "\\"; break;
  case '\0': aBuffer = ""; break;
  default: aBuffer = aC & 0x7f;
  }

  return aBuffer.c_str();
}

const char* escape(const char aC, std::string& aBuffer) {
  switch (aC & 0x7f) {
  case '\a': aBuffer = "\\a"; break;
  case '\b': aBuffer = "\\b"; break;
  case '\f': aBuffer = "\\f"; break;
  case '\n': aBuffer = "\\n"; break;
  case '\r': aBuffer = "\\r"; break;
  case ' ': aBuffer = "\\s"; break;
  case '\t': aBuffer = "\\t"; break;
  case '\v': aBuffer = "\\v"; break;
  case '\\': aBuffer = "\\"; break;
  case '\0': aBuffer = ""; break;
  default: aBuffer = aC & 0x7f;
  }

  return aBuffer.c_str();
}

bool read(const char* aFileName, textformat_t& aSettings) {
  std::string& sendfrompath = aSettings.sendfrompath;
  std::string& fontpath = aSettings.fontpath;
  std::string& zipfont = aSettings.zipfont;
  std::string& outputpath = aSettings.outputpath;
  char& pagedelimiter = aSettings.pagedelimiter;
  bool& drawnenga = aSettings.drawnenga;
  point_t& sendfrom_zipframe_offset =  aSettings.sendfrom_zipframe_offset;
  personformat_t& sendto = aSettings.sendto;
  personformat_t& sendfrom = aSettings.sendfrom;

  // set default values.
  std::ifstream file(aFileName);
  if (!file) {
    return false;
  }

  std::string line;
  char buff[40];
  char data[128];
  data[127] = 0;
  int read = 0;
  while (file) {
    std::getline(file, line, '\n');
    if (line.empty() || '#' == line.at(0)) {
      continue;
    }
    PRINTHA_READ_TXT_STRING(sendfrompath)
    PRINTHA_READ_TXT_STRING(outputpath)
    PRINTHA_READ_TXT_STRING(fontpath)
    PRINTHA_READ_TXT_STRING(zipfont)
    PRINTHA_READ_TXT_CHAR(pagedelimiter)
    PRINTHA_READ_TXT_BOOLEAN(drawnenga)
    PRINTHA_READ_TXT_POINT(sendfrom_zipframe_offset)
    PRINTHA_READ_TXT_PERSON(sendto)
    PRINTHA_READ_TXT_PERSON(sendfrom)
  }
  return true;
}

void write(const char* aFileName, const textformat_t& aSettings,
           WriteFormat aWriteFormat) {
  std::string strbuff;

  remove(aFileName);
  if (kWriteFormatCSRC == aWriteFormat) {
    flogf(aFileName, "void init(textformat_t& aSettings) {\n");
    PRINTHA_WRITE_CPP_STRING(aFileName, aSettings.sendfrompath);
    PRINTHA_WRITE_CPP_STRING(aFileName, aSettings.fontpath);
    PRINTHA_WRITE_CPP_STRING(aFileName, aSettings.zipfont);
    PRINTHA_WRITE_CPP_STRING(aFileName, aSettings.outputpath);
    PRINTHA_WRITE_CPP_CHAR(aFileName, aSettings.pagedelimiter);
    PRINTHA_WRITE_CPP_BOOLEAN(aFileName, aSettings.drawnenga);
    PRINTHA_WRITE_CPP_POINT(aFileName, aSettings.sendfrom_zipframe_offset);
    PRINTHA_WRITE_CPP_PERSON(aFileName, aSettings.sendfrom);
    PRINTHA_WRITE_CPP_PERSON(aFileName, aSettings.sendto);
    flogf(aFileName, "}\n");
  }
  else {
    const std::string& sendfrompath = aSettings.sendfrompath;
    const std::string& fontpath = aSettings.fontpath;
    const std::string& zipfont = aSettings.zipfont;
    const std::string& outputpath = aSettings.outputpath;
    const char& pagedelimiter = aSettings.pagedelimiter;
    const bool& drawnenga = aSettings.drawnenga;
    const point_t& sendfrom_zipframe_offset = 
      aSettings.sendfrom_zipframe_offset;
    const personformat_t& sendto = aSettings.sendto;
    const personformat_t& sendfrom = aSettings.sendfrom;

    PRINTHA_WRITE_TXT_STRING(aFileName, sendfrompath);
    PRINTHA_WRITE_TXT_STRING(aFileName, fontpath);
    PRINTHA_WRITE_TXT_STRING(aFileName, zipfont);
    PRINTHA_WRITE_TXT_STRING(aFileName, outputpath);
    PRINTHA_WRITE_TXT_CHAR(aFileName, pagedelimiter);
    PRINTHA_WRITE_TXT_BOOLEAN(aFileName, drawnenga);
    PRINTHA_WRITE_TXT_POINT(aFileName, sendfrom_zipframe_offset);
    PRINTHA_WRITE_TXT_PERSON(aFileName, sendto);
    PRINTHA_WRITE_TXT_PERSON(aFileName, sendfrom);
  }
}

} // namespace settings
} // namespace printha
