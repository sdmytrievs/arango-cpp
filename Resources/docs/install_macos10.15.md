## Main information ##

[Qt for macOS](https://doc.qt.io/qt-5/macos.html)

* To download and install Qt for macOS, follow the instructions on the [Getting Started with Qt](https://doc.qt.io/qt-5/gettingstarted.html) page.

The following macOS versions are supported.

| Target Platform |	Architecture|	Build Environment|
| macOS  10.15	| x86_64 |	Xcode 11 (10.15 SDK) |

* Xcode can be downloaded from Apple's developer website (including older versions of Xcode). Once installed, choosing an Xcode installation is done using the xcode-select tool.

```

$ sudo xcode-select --switch /Applications/Xcode.app

```

You can inspect the globally selected Xcode installation using the same tool.

```

$ xcode-select -print-path
/Applications/Xcode.app/Contents/Developer

```


* If you have problems with compile googletests execute:

```
$ sudo rm -r /usr/local/include/gtest/
$ sudo rm -r /usr/local/include/gmock/

```
