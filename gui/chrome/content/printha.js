Components.utils.import("resource://gre/modules/FileUtils.jsm");
Components.utils.import("resource://gre/modules/NetUtil.jsm");
Components.utils.import("resource://printha/csvparser.js");

var gPrinthaSettings = {
  get "font" () {
    return document.getElementById("font-list").selectedItem.value;
  },

  get "zipfont" () {
    return document.getElementById("zipfont-list").selectedItem.value;
  },

  set addressData(aData) {
    this._addressData = aData;
    if (this._addressData.length > 0 && this._addressData[0].length > 3) {
      document.getElementById("sendto.fname").value = this._addressData[0][0];
      document.getElementById("sendto.sname").value = this._addressData[0][1];
      document.getElementById("sendto.zipcode").value = this._addressData[0][2];
      document.getElementById("sendto.address").value = this._addressData[0][3];
      if (this._addressData[0].length > 4 && this._addressData[0][4]) {
        document.getElementById("sendto.address").value += 
          "\n" + this._addressData[0][4];
      }
    }
    else {
      document.getElementById("sendto.fname").value = "";
      document.getElementById("sendto.sname").value = "";
      document.getElementById("sendto.zipcode").value = "";
      document.getElementById("sendto.address").value = "";
    }
  },

  set sendfromData(aData) {
    var data = aData.split(";")

    while(data.length < 3) {
      data.push("");
    }

    document.getElementById("sendfrom.name").value = data[0];
    document.getElementById("sendfrom.zipcode").value = data[1];
    document.getElementById("sendfrom.address").value = data[2];
  },

  get sendfromData() {
    return document.getElementById("sendfrom.name").value + ";" +
           document.getElementById("sendfrom.zipcode").value + ";" +
           document.getElementById("sendfrom.address").value;
  },

  get addressData() {
    var length = this._addressData.length;
    var data = this.formatLine(this._addressData[0]);
    for (var i = 1; i < length; i++) {
      var temp = this.formatLine(this._addressData[i]);
      if (temp) {
        data += "|" + temp;
      }
    }
    return data;
  },

  formatLine: function (aLine) {
    if (aLine.length < 4) {
      return "";
    }
    var data = aLine[0] + " " + aLine[1] + "様;" + aLine[2] + ";" + aLine[3];
    if (aLine.length > 4 && aLine[4]) {
      data += "\n" + aLine[4] + ";";
    }
    return data;
  },

  get configData() {
    var data = "";
    for (var i in this.configKeys) {
      var key = this.configKeys[i]; 
      data += key + " " + this[key] + "\n";
    }
    return data;
  },

  configKeys: ["sendfrompath", "outputpath", "font", "zipfont"],
  "sendfrompath" : "",
  "outputpath" : "",
  config : "",
  svgpath: "",
  isPreview: false,
  binpath: "",
  _addressData: [["","","",""]]
};

function selectCSV() {
  var fp = Components.classes["@mozilla.org/filepicker;1"]
                     .createInstance(Components.interfaces.nsIFilePicker);
  fp.init(window, "CSVファイルの選択",
          Components.interfaces.nsIFilePicker.modeOpen);
 
  fp.appendFilter("CSVファイル (*.csv)", "*.csv");

  var rv = fp.show();
  if (rv != Components.interfaces.nsIFilePicker.returnOK &&
      rv != Components.interfaces.nsIFilePicker.returnReplace) {
    return;
  }

  document.getElementById("csv-path").value = fp.file.path;

  function callback(aInputStream, aStatus) {
    if (!Components.isSuccessCode(aStatus)) {
      return;
    }

    const rc = Components.interfaces.nsIConverterInputStream
                                    .DEFAULT_REPLACEMENT_CHARACTER;
    const kOption = {
      charset: "utf-8",
      eplacement: Components.interfaces.nsIConverterInputStream
                            .DEFAULT_REPLACEMENT_CHARACTER
    };
    var data = NetUtil.readInputStreamToString(aInputStream,
                                               aInputStream.available(),
                                               kOption);
    gPrinthaSettings.addressData = parseCSV(data);
  }

  NetUtil.asyncFetch(fp.file, callback);
}
function loadSender() {
  var fp = Components.classes["@mozilla.org/filepicker;1"]
                     .createInstance(Components.interfaces.nsIFilePicker);
  fp.init(window, "テキストファイルの選択",
          Components.interfaces.nsIFilePicker.modeOpen);
  fp.appendFilters(Components.interfaces.nsIFilePicker.filterText);

  var rv = fp.show();
  if (rv != Components.interfaces.nsIFilePicker.returnOK &&
      rv != Components.interfaces.nsIFilePicker.returnReplace) {
    return;
  }
  loadSenderFile(fp.file)
}

function saveSender() {
  var fp = Components.classes["@mozilla.org/filepicker;1"]
                     .createInstance(Components.interfaces.nsIFilePicker);
  fp.init(window, "保存先の選択",
          Components.interfaces.nsIFilePicker.modeSave);
  fp.appendFilters(Components.interfaces.nsIFilePicker.filterText);
  fp.defaultString = "sendfrom.txt"
  var rv = fp.show();
  if (rv != Components.interfaces.nsIFilePicker.returnOK &&
      rv != Components.interfaces.nsIFilePicker.returnReplace) {
    return;
  }

  var suc = Components.classes["@mozilla.org/intl/scriptableunicodeconverter"]
                      .createInstance(Components.interfaces
                                                .nsIScriptableUnicodeConverter);
  suc.charset = "utf-8";
  var is = suc.convertToInputStream(gPrinthaSettings.sendfromData);
  var os = FileUtils.openSafeFileOutputStream(fp.file)

  NetUtil.asyncCopy(is, os, function (aResult) {});
}

function print(aIsPreview) {
  if (aIsPreview) {
    gPrinthaSettings.svgpath =
      FileUtils.getFile("TmpD", ["printha", "preview.svg"]).path;

    gPrinthaSettings["outputpath"] = gPrinthaSettings.svgpath;
  }
  else {
    var fp = Components.classes["@mozilla.org/filepicker;1"]
                       .createInstance(Components.interfaces.nsIFilePicker);
    fp.init(window, "保存先の選択",
            Components.interfaces.nsIFilePicker.modeSave);
    fp.appendFilter("PDFファイル (*.pdf)", "*.pdf");
    fp.defaultString = "printha.pdf"
    var rv = fp.show();
    if (rv != Components.interfaces.nsIFilePicker.returnOK &&
        rv != Components.interfaces.nsIFilePicker.returnReplace) {
      return;
    }

    gPrinthaSettings.svgpath = "about:blank";
    gPrinthaSettings["outputpath"] = fp.file.path;
  }
  gPrinthaSettings.isPreview = aIsPreview;
  var sendfromFile =
    FileUtils.getFile("TmpD", ["printha", "sendfrom.txt"]);

  gPrinthaSettings["sendfrompath"] = sendfromFile.path;

  var suc = Components.classes["@mozilla.org/intl/scriptableunicodeconverter"]
                      .createInstance(Components.interfaces
                                                .nsIScriptableUnicodeConverter);
  suc.charset = "utf-8";
  var is = suc.convertToInputStream(gPrinthaSettings.sendfromData);
  var os = FileUtils.openSafeFileOutputStream(sendfromFile)

  NetUtil.asyncCopy(is, os, print2);
}

function print2(aStatus) {
  try {
    var configFile =
      FileUtils.getFile("TmpD", ["printha", "config.txt"]);

    gPrinthaSettings.config = configFile.path;

    var suc = Components.classes["@mozilla.org/intl/scriptableunicodeconverter"]
                        .createInstance(Components.interfaces
                                                  .nsIScriptableUnicodeConverter);
    suc.charset = "utf-8";
    var is = suc.convertToInputStream(gPrinthaSettings.configData);
    var os = FileUtils.openSafeFileOutputStream(configFile)

    NetUtil.asyncCopy(is, os, print3);
  }
  catch(e) {
    alert(e);
  }
}

var previewObserver = {
  observe: function (aSubject, aTopic, aData) {
    var browser = document.getElementById("content");
    if (aTopic != "process-finished") {
      if (browser.currentURI.spec != "about:blank") {
        browser.loadURI("about:blank", null, null);
      }
    }
    var io = Components.classes['@mozilla.org/network/io-service;1']
                       .getService(Components.interfaces.nsIIOService);
    var file = Components.classes["@mozilla.org/file/local;1"]
                         .createInstance(Components.interfaces.nsILocalFile);
    file.initWithPath(gPrinthaSettings.svgpath);
    var fileURI = io.newFileURI(file);
    if (fileURI.equals(browser.currentURI)) {
      const flag =
        Components.interfaces.nsIWebNavigation.LOAD_FLAGS_BYPASS_HISTORY |
        Components.interfaces.nsIWebNavigation.LOAD_FLAGS_BYPASS_CACHE;
      browser.reload();
    }
    else {
      browser.loadURI(fileURI.spec);
    }
  },

  QueryInterface: function(aIID) {
    if(!aIID.equals(CI.nsISupports) && !aIID.equals(CI.nsIObserver))
      throw CR.NS_ERROR_NO_INTERFACE;
    return this;
  }
};

function print3(aStatus) {
  try {
    var process = Components.classes["@mozilla.org/process/util;1"]
                            .createInstance(Components.interfaces.nsIProcess);
    var file = Components.classes["@mozilla.org/file/local;1"]
                         .createInstance(Components.interfaces.nsILocalFile);
    file.initWithPath(gPrinthaSettings.binpath);
    process.init(file);

    var args = ["--import", gPrinthaSettings.config];
    if (gPrinthaSettings.isPreview) {
      args.push("--preview");
      args.push("--svg");
    }
    args.push(gPrinthaSettings.addressData);

    process.runwAsync(args, args.length, previewObserver, false);
  }
  catch(e) {
    alert(e);
  }
}

function loadSenderFile(aFile) {
  function callback(aInputStream, aStatus) {
    if (!Components.isSuccessCode(aStatus)) {
      return;
    }

    const rc = Components.interfaces.nsIConverterInputStream
                                    .DEFAULT_REPLACEMENT_CHARACTER;
    const kOption = {
      charset: "utf-8",
      eplacement: Components.interfaces.nsIConverterInputStream
                            .DEFAULT_REPLACEMENT_CHARACTER
    };
    gPrinthaSettings.sendfromData =
      NetUtil.readInputStreamToString(aInputStream, aInputStream.available(),
                                      kOption);
  }

  NetUtil.asyncFetch(aFile, callback);
}

function startup(aEvent) {
  var fl = Components.classes["@mozilla.org/gfx/fontenumerator;1"]
                     .createInstance(Components.interfaces.nsIFontEnumerator);
  var size = {};
  var names = fl.EnumerateAllFonts(size);

  var i;
  var fontlist = document.getElementById("font-list");
  for (i = 0; i < size.value; i++) {
    fontlist.appendItem(names[i], names[i], fl.getDefaultFont("ja", names[i]));
  }

  fontlist = document.getElementById("zipfont-list");
  for (i = 0; i < size.value; i++) {
    fontlist.appendItem(names[i], names[i], fl.getDefaultFont("ja", names[i]));
  }

  var cl = window.arguments[0]
                 .QueryInterface(Components.interfaces.nsICommandLine);

  gPrinthaSettings.binpath = cl.handleFlagWithParam("printha-bin", false);
  FileUtils.getDir("TmpD", ["printha"]);
}

function enddown(aEvent) {
  var file = FileUtils.getDir("TmpD", ["printha"]);
  if (file.exists())
    file.remove(true);
}

window.addEventListener("unload", enddown, false);
window.addEventListener("load", startup, false);
