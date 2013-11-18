printha
=======

This application helps you to print name/address on Japanese postcards (Hagaki).

インストールする前に
-----------------

このソフトウェアは、テキストで提供される宛名と住所のデータをハガキサイズのPDFファイルへと
変換するためのものです。そして、ここでいうテキストとはUTF-8でエンコードされた文字列のこと
です。入力データがUTF-8にさえなっていれば、大きな問題は起きないと思いますが、システムの環
境をShift-JISやEUC-JPなどに設定している場合は、その分、要求される知識が増えることを念頭
に置いてから導入を決定してください。


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

Fedoraでは、Harfbuzzパッケージが配布されているので、ビルドの必要はありません。

    sudo yum install gcc gcc-c++ freetype-devel cairo-devel harfbuzz-devel cmake

でprinthaをビルドする準備が整います。

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

を実行して、「foo.pdf」の中身を確認してください。

###printhaの使い方の例

元々、想定している使途は、以下のように、lprの直前にパイプ('|')で標準入力・出力を繋ぐ方式です。

    echo -e "徳川 家康様;0000000;東京都千代田区千代田町一丁目\n江戸城本丸天守最上階" | printha | lpr

もし、住所録のデータが

    姓　,名　,郵便番号,住所(番地)　　　　　　　　　,住所(建物名)
    徳川,家康, 0000000,東京都千代田区千代田町一丁目,江戸城本丸天守最上階,…
    織田,信長, 0000000,滋賀県蒲生郡安土町一丁目　　,安土城本丸天守最上階,…

のようになっていれば、printhaに渡す文字列を、下記のようにprintfで整形するだけで済みます。

    printf "%s %s様;%07d;%s\n%s" "徳川" "家康" 0000000 "東京都千代田区千代田町一丁目" "江戸城本丸天守最上階" | printha | lpr

実際にはこの順番になっているとは限らないので、表計算ソフトで列を入れ替えて列の順番を上記の
形式と揃えてから、CSVに出力すると便利です。

* CSVファイルからPDFへの変換例
    * [入力ファイル(csv)](https://github.com/Torisugari/printha/blob/master/demo/addressbook2.csv)
    * [CSVパーサー](https://github.com/Torisugari/printha/blob/master/demo/csvparser.py)と[コマンドライン](https://github.com/Torisugari/printha/blob/master/demo/demo4.sh)
    * [出力ファイル(pdf)](https://github.com/Torisugari/printha/blob/master/demo/4/csv.pdf)

###printhaの設定を変更する

printhaはカレントディレクトリにあるprintha.config.txtで設定された通りに動作します。
printha.config.txtの中身を書き換えると以下のような機能を使うことができます。

* 住所や氏名が書き込まれる位置を変更する
* 住所と氏名以外の事柄、例えば組織名、役職または電話番号を書き込む 
    * [組織名及び役職名(pdf)](https://github.com/Torisugari/printha/blob/master/demo/1/president.pdf)
    * [電話番号(pdf)](https://github.com/Torisugari/printha/blob/master/demo/1/telephone.pdf)
* フォント及びそのサイズを変更する
* 官製はがきでないはがきの郵便番号欄に、赤い枠を書き込む
* 年賀はがきでないはがきを年賀状として使用するために「年賀」の文字を朱筆する
    * [郵便番号の赤枠と「年賀」を朱筆(pdf)](https://github.com/Torisugari/printha/blob/master/demo/1/nenga.pdf)
* 入力データの各種区切り文字を変更する
   * [入力データ](https://github.com/Torisugari/printha/blob/master/demo/addressbook.csv)
     [設定ファイル](https://github.com/Torisugari/printha/blob/master/demo/csvoid.txt)
     [出力(pdf)](https://github.com/Torisugari/printha/blob/master/demo/2/addressbook.pdf)

詳しくはsettingsディレクトリにある[config.txt](https://github.com/Torisugari/printha/blob/master/settings/config.txt)を参考にしてください。


郵便局により細かく場所が決められている宛先の郵便番号欄とは違って、
差出人の郵便番号欄には定位置がありません。
各種の「お年玉付き年賀はがき」の差出人の郵便番号欄は、大体決まった位置にあるので、
printhaは、特に指定しない限り、「お年玉付き年賀はがき」に合わせた位置(
[白紙のはがき画像](https://github.com/Torisugari/printha/blob/master/resources/bg.png)
の赤い点線の部分)で郵便番号を書き込みます。しかし、それ以外の官製はがきでは
[白紙のはがき画像](https://github.com/Torisugari/printha/blob/master/resources/bg.png)
の青い部分に郵便番号欄があるので、設定を変更して下さい。

###printhaのコマンドラインオプション
上記の方法の他に、"--import <ファイルパス>"オプションで、設定を一時的に変更することができます。

"--ps"と"--svg"のコマンドラインオプションをつけると、
PDFの代わりにPostScriptファイル、またはSVGファイルを出力します。

"--preview"のコマンドラインオプションを指定すると、
[白紙のはがき画像](https://github.com/Torisugari/printha/blob/master/resources/bg.png)
と重ね合わせて出力されます。この場合、そのまま印刷してしまわないように注意して下さい。

###printhaで出来ないこと

現在、printhaは宛名を連名で出す機能を実装していません。ただ、宛名でも改行は有効なので、
全角空白で字数を合わせれば、それらしく見せかけることができます。
[入力データ](https://github.com/Torisugari/printha/blob/master/demo/demo3.sh)
[出力データ](https://github.com/Torisugari/printha/blob/master/demo/3/renmei.pdf)

アンインストール
--------------

ビルドしたディレクトリにuninstall.shというファイルが作成されるので、

    sudo ./uninstall.sh

を実行してください。

Harfbuzzを上記の説明にしたがってビルドしてからインストールした場合は、
harfbuzzディレクトリで、

    sudo make uninstall

を実行してください。


その他の注意事項
--------------

resourcesディレクトリに収録されているIPAex明朝フォントとdemoに収録されている各PDFファイル
のフォントについては、同梱のIPAフォントライセンスv1.0が適用されます。
配布元: [IPAフォント/IPAexフォント　ダウンロードトップページ](http://ipafont.ipa.go.jp/)

resourcesディレクトリに収録されているOCRB_aizuフォントについては、SIL Open Font License
1.1が適用されます。
配布元: [発生事例:OCRBフォントが使用できない 会津若松市](http://www.city.aizuwakamatsu.fukushima.jp/docs/2008021400265/)

このソフトウェアがフォントを同梱している理由は、ユーザーがフォントのパスを自力で
設定する困難を慮ってのものなので、ソフトウェア自体が特定のフォントの設定に基づいて
実装されているわけではありません。

フォント以外のソフトウェアはMITライセンスで、設定や住所のファイルはパブリックドメイン
とします。

