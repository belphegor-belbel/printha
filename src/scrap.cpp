  char* str = strndup(aString, 2048);
  char* ptr = str;

  double width = aFrame.rect.width() / count;
  double height = aFrame.rect.height();
  point_t start(aFrame.rect.mEnd.mX - width, aFrame.rect.mStart.mY);
  line = strsep(&ptr, "\n");
  bool isFirstLine = true;
  while (line) {
    rect_t linerect(start, width, height);

    double advance = printString(aFTSeletedFont, aCS, line, linerect,
                                 maxFontSize, aFrame.stretch,
                                 aFrame.bottom && (!isFirstLine));
    if (isFirstLine) {
      if (aFrame.bottom) {
        height = advance;
      }
      isFirstLine = false;
    }
    start.mX -= width;
    line = strsep(&ptr, "\n");
  }
  free(str);
#if 0
#if _NO_PRESIDENT
  typedef rect_t r;
  settings.sendto.name.rect =
    rect_t(point_t(HAGAKI_WIDTH_PT/2 - 24.0, 80.0), 48.0, 280.0);
  settings.sendto.name.fontsize = 32.0;
  settings.sendto.name.stretch = true;
  settings.sendto.name.bottom = false;
#else
  settings.sendto.name.rect =
    rect_t(point_t(HAGAKI_WIDTH_PT/2 - 24.0, 120.0), 48.0, 240.0);
  settings.sendto.name.fontsize = 32.0;
  settings.sendto.name.stretch = true;
  settings.sendto.name.bottom = false;
#endif

  settings.sendto.addr.rect =
    rect_t(point_t(HAGAKI_WIDTH_PT - 64.0, 80.0), 64.0, 250.0);
  settings.sendto.addr.fontsize = 20.0;
  settings.sendto.addr.stretch = false;

#if _NO_PRESIDENT
  settings.sendto.extra[0].rect =
    rect_t(point_t(0.0, 0.0), point_t(0.0, 0.0));
  settings.sendto.extra[0].fontsize = 20.0;
  settings.sendto.extra[0].stretch = false;
#else
  settings.sendto.extra[0].rect =
    rect_t(point_t(HAGAKI_WIDTH_PT/2 - 24.0, 75.0), 48.0, 40.0);
  settings.sendto.extra[0].fontsize = 20.0;
  settings.sendto.extra[0].stretch = true;
#endif

  settings.sendto.extra[1].rect =
    rect_t(point_t(0.0, 0.0), point_t(0.0, 0.0));
  settings.sendto.extra[1].fontsize = 20.0;
  settings.sendto.extra[1].stretch = false;

  settings.sendfrom.name.rect =
    rect_t(point_t(0.0, 200.0), 32.0, 140.0);
  settings.sendfrom.name.fontsize = 32.0;
  settings.sendfrom.name.stretch = true;

  settings.sendfrom.addr.rect =
    rect_t(point_t(32.0, 160.0), 48.0, 180.0);
  settings.sendfrom.addr.fontsize = 20.0;
  settings.sendfrom.addr.stretch = false;

  settings.sendfrom.extra[0].rect =
    rect_t(point_t(0.0, 0.0), point_t(0.0, 0.0));
  settings.sendfrom.extra[0].fontsize = 20.0;
  settings.sendfrom.extra[0].stretch = false;

  settings.sendfrom.extra[1].rect =
    rect_t(point_t(0.0, 0.0), point_t(0.0, 0.0));
  settings.sendfrom.extra[1].fontsize = 20.0;
  settings.sendfrom.extra[1].stretch = false;

  static const char kSendfrom[] = "sashidashi.txt";
  settings.sendfrompath = kSendfrom;
  static const char kOutput[] = "./temp.pdf";
  static const char kFont[] = "/usr/share/fonts/truetype/takao-gothic/TakaoGothic.ttf";
//  static const char kFont[] = "resources/ipaexm.ttf";
//  static const char kFont[] = "resources/meiryob.ttc";
//  settings.outputpath = kOutput;
  settings.outputpath = "";
  settings.fontpath = kFont;

  static const char dlmt[] = ";";
  settings.sendto.dlmt = dlmt;
  settings.sendfrom.dlmt = dlmt;
#endif
#if 0
sendfrompath                   = "sashidashi.txt";
fontpath                       = "/usr/share/fonts/truetype/takao-gothic/TakaoGothic.ttf";
outputpath                     = "";
sendto.dlmt                    = ";";
sendto.zipfontsize             = 6.000000  ;
sendto.name.rect               = r(117.732283, 120.000000, 165.732283, 360.000000);
sendto.name.fontsize           = 32.000000 ;
sendto.name.stretch            = true ;
sendto.name.bottom             = false;
sendto.addr.rect               = r(219.464567,  80.000000, 283.464567, 330.000000);
sendto.addr.fontsize           = 20.000000 ;
sendto.addr.stretch            = false;
sendto.addr.bottom             = true ;
sendto.extra[0].rect           = r(117.732283,  75.000000, 165.732283, 115.000000);
sendto.extra[0].fontsize       = 20.000000 ;
sendto.extra[0].stretch        = true ;
sendto.extra[0].bottom         = true ;
sendto.extra[1].rect           = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendto.extra[1].fontsize       = 20.000000 ;
sendto.extra[1].stretch        = false;
sendto.extra[1].bottom         = true ;
sendto.extra[2].rect           = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendto.extra[2].fontsize       = 0.000000  ;
sendto.extra[2].stretch        = true ;
sendto.extra[2].bottom         = true ;
sendto.extra[3].rect           = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendto.extra[3].fontsize       = 0.000000  ;
sendto.extra[3].stretch        = true ;
sendto.extra[3].bottom         = true ;
sendto.extra[4].rect           = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendto.extra[4].fontsize       = 0.000000  ;
sendto.extra[4].stretch        = true ;
sendto.extra[4].bottom         = true ;
sendto.extra[5].rect           = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendto.extra[5].fontsize       = 0.000000  ;
sendto.extra[5].stretch        = true ;
sendto.extra[5].bottom         = true ;
sendto.drawzipframe            = true ;
sendfrom.dlmt                  = ";";
sendfrom.zipfontsize           = 6.000000  ;
sendfrom.name.rect             = r(  0.000000, 200.000000,  32.000000, 340.000000);
sendfrom.name.fontsize         = 32.000000 ;
sendfrom.name.stretch          = true ;
sendfrom.name.bottom           = false;
sendfrom.addr.rect             = r( 32.000000, 160.000000,  80.000000, 340.000000);
sendfrom.addr.fontsize         = 20.000000 ;
sendfrom.addr.stretch          = false;
sendfrom.addr.bottom           = true ;
sendfrom.extra[0].rect         = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendfrom.extra[0].fontsize     = 20.000000 ;
sendfrom.extra[0].stretch      = false;
sendfrom.extra[0].bottom       = true ;
sendfrom.extra[1].rect         = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendfrom.extra[1].fontsize     = 20.000000 ;
sendfrom.extra[1].stretch      = false;
sendfrom.extra[1].bottom       = true ;
sendfrom.extra[2].rect         = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendfrom.extra[2].fontsize     = 0.000000  ;
sendfrom.extra[2].stretch      = false;
sendfrom.extra[2].bottom       = false;
sendfrom.extra[3].rect         = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendfrom.extra[3].fontsize     = 0.000000  ;
sendfrom.extra[3].stretch      = true ;
sendfrom.extra[3].bottom       = false;
sendfrom.extra[4].rect         = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendfrom.extra[4].fontsize     = 0.000000  ;
sendfrom.extra[4].stretch      = true ;
sendfrom.extra[4].bottom       = true ;
sendfrom.extra[5].rect         = r(  0.000000,   0.000000,   0.000000,   0.000000);
sendfrom.extra[5].fontsize     = 0.000000  ;
sendfrom.extra[5].stretch      = true ;
sendfrom.extra[5].bottom       = true ;
sendfrom.drawzipframe          = true ;
#endif
//"[a-z\\.\\[\\]]+rect\\s*=\\s*r\\(\\s*([1-9\\.]+)s*\\,\\s*([1-9\\.]+)s*\\,\\s*([1-9\\.]+)s*\\,\\s*([1-9\\.]+)s*\\);"
//  std::regex regRect("(\\w+)");
//  std::smatch match;


//      if (std::regex_match(line, match, regRect)) {
//        size_t size =match.size();
//      }
//      char trash;
//      std::sscanf(line.c_str(), "%34s %c %c%c%lf%c%lf%c%lf%c%lf);", buff, &trash, &trash, &trash, &a, &trash, &b, &trash, &c, &trash, &d);
//    fprintf(stderr, "#####%s,\n%s,%f,%f,%f,%f######\n", line.c_str(), buff, a,b,c,d);

  static const char katsuragikatsushika[] =
#if 0
  {0xE8, 0x91, 0x9B, 0xF3, 0xA0, 0x84, 0x80,
   0xE5, 0x9F, 0x8E,
   0xE5, 0xB8, 0x82,
   0xE8, 0x91, 0x9B, 0xF3, 0xA0, 0x84, 0x81,
   0xE9, 0xA3, 0xBE, 
   0xE5, 0x8C, 0xBA,
   0};
  printf("\n");
  const char* ptr = katsushikakatsuragi;
  while( (*ptr) != 0){
    printf("%d, ", (*ptr));
    ptr++;
  }
  printf("\n");
#endif
  {-24, -111, -101, -13,  -96, -124, -128,  //葛 katsura (curved) U+845B U+E0100
   -27,  -97, -114,                         //城 gi               U+57CE
   -27,  -72, -126,                         //市 shi              U+5E02
   -24, -111, -101, -13,  -96, -124, -127,  //葛 katsu   (edged)  U+845B U+E0101
   -23,  -93,  -66,                         //飾 shika            U+98FE
   -27, -116,  -70,                         //区 ku               U+533A
   0};
#if 0
  if (length > MAX_GLYPH_LENGTH) {
     fprintf(stderr, "This line is too long: %s", aString);
     exit(-1);
  }
#endif


#ifndef NDEBUG
  printZipcode(aCS, "105-0011", zipRects, 6.0, true);
#else
  printZipcode(aCS, "105-0011", zipRects, 6.0, false);
#endif

// 楚人有㆘鬻㆓盾與㆒㆑矛者㆖
// 愛新覺羅 顯㺭様
// 夏目 漱石 様
// "国会議事堂様"
// "東京都千代田区永田町一丁目七番一号"
//"‐−-－"
//東京都墨田区押上一丁目一‐二
// "東京タワー様"
//　東京都港区芝公園四丁目二-八 

  printLines(aFTLib, aCS, "葛\xF3\xA0\x84\x80葛\xF3\xA0\x84\x81 "
                              "齋\xF3\xA0\x84\x82齋\xF3\xA0\x84\x83様",
             nameRect, 48.0, true);

// 東京都千代田区永田町1丁目7番1号
  printLines(aFTLib, aCS, "東京都港区芝公園四丁目二‐八\n東京タワー 十六階一号室B", addrRect, 20.0, false);
/*
flogf("atesaki.txt", "%s;%s;%s;","葛\xF3\xA0\x84\x80葛\xF3\xA0\x84\x81 "
                              "齋\xF3\xA0\x84\x82齋\xF3\xA0\x84\x83様","105-0011","東京都港区芝公園四丁目二‐八\n東京タワー 十六階一号室B");
*/

class OutputBuffer {
public:
  OutputBuffer() :size(0) {}
  ~OutputBuffer() {}
  void output() {
    std::vector<char>::const_iterator itr;
    std::vector<char>::const_iterator end = mBuffer.end();
    for (itr = mBuffer.begin(); itr != end; itr++) {
       std::cout << char(*itr);
     }
  }
  std::vector<char> mBuffer;
  uint32_t size;
};


cairo_status_t caStdout(void *aClosure,
                        const unsigned char *aData,
                        unsigned int aLength){
  OutputBuffer* stdoutbuff = static_cast<OutputBuffer*> (aClosure);
  std::vector<char>& buffer = stdoutbuff->mBuffer;
//  buffer.insert(buffer.end(), aData, aData + aLength);


  for (unsigned int i = 0; i < aLength; i++) {
    std::cout << char(*(aData+i));
  }
  stdoutbuff->size += aLength;



//  return CAIRO_STATUS_WRITE_ERROR;
  return CAIRO_STATUS_SUCCESS;
}


