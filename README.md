printha
=======

This application helps you to print name/address on Japanese postcards (Hagaki).

インストールまで
--------------

printhaのビルドには、Cairo、Freetype、Harfbuzz及びそれらの開発環境が必要です。

###Harfbuzzのインストール

ところが、2013年現在、Harfbuzzがバイナリでは提供されていない環境がたくさんあります。その場
合、まず、Harfbuzzをビルドしてインストールする必要があります。HarfbuzzのビルドにはCairoと
Freetypeが必要なので、Harfbuzzがビルド完了後の環境では、CairoとFreetypeは揃っていること
になります。

Harfbuzz本体の導入手順は、
[http://www.freedesktop.org/wiki/Software/HarfBuzz/](http://www.freedesktop.org/wiki/Software/HarfBuzz/)
に書いてある通りなのですが、printhaはUnicodeの解釈にICUを利用しているので、Harfbuzzのサブ
パッケージであるHarfbuzz-ICUにも依存しています。そのため、Harfbuzzをビルドする前にICUの開
発環境もインストールする必要があります。

つまるところ、UbuntuなどのDebian系のディストリビューションの場合、

    sudo apt-get install gcc g++ libfreetype6-dev libglib2.0-dev libcairo2-dev autoconf automake libtool pkg-config ragel gtk-doc-tools libicu-dev cmake

で環境を整えた後、適当なディレクトリで、

    git clone git://github.com/behdad/harfbuzz.git
    cd harfbuzz
    ./autogen.sh
    make
    sudo make install

を実行してください。

###printhaのインストール

適当なディレクトリで、

    git clone git://github.com/Torisugari/printha.git
    cd printha
    mkdir build
    cd build
    cmake ..
    make release
    make
    sudo make install

を実行してください。実行ファイルへのパスが把握できているなら、最後のsudo make installは省
略しても構いません。

インストールした後
----------------

###printhaの使い方を練習する
適当なディレクトリで、

    printha --init

を実行すると、そのディレクトリにprintha.config.txt、sendfrom.txt、sendto.txtの3つの
テキストファイルが作成されます。そこで、

    printha < sendto.txt > foo.pdf

と入力すると、差出人に「sendfrom.txt」、宛先に「sendto.txt」の内容が反映されたPDFファイル
が作成されます。それを確かめた後、「sendfrom.txt」の中身を自分の住所氏名で上書きしてから、
もう一度、

    printha < sendto.txt > foo.pdf

を実行して、「hoge.pdf」の中身を確認してください。

###設定を変更する
settingsディレクトリにあるconfig.txtと、demoディレクトリのスクリプトを参考にしてください。

