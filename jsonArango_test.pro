TEMPLATE = app
CONFIG += thread console c++14
CONFIG -= app_bundle
CONFIG -= qt

!win32 {
  DEFINES += __unix
}

macx-g++ {
  DEFINES += __APPLE__
}

macx-clang {
  DEFINES += __APPLE__
  INCLUDEPATH   += "/usr/local/include"
  DEPENDPATH   += "/usr/local/include"
  LIBPATH += "/usr/local/lib/"
}

win32 {
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
}

# Define the directory where jsonio14 source code is located
JSONARANGO_DIR =  $$PWD/src
JSONARANGO_HEADERS_DIR =  $$JSONARANGO_DIR/../include
TESTS_DIR =  $$PWD/tests

DEPENDPATH   += $$JSONARANGO_DIR
DEPENDPATH   += $$JSONARANGO_HEADERS_DIR
DEPENDPATH   += $$TESTS_DIR

INCLUDEPATH   += $$JSONARANGO_DIR
INCLUDEPATH   += $$JSONARANGO_HEADERS_DIR
INCLUDEPATH   += $$TESTS_DIR

LIBS +=  -lcurl  -lvelocypack

OBJECTS_DIR   = obj

include($$TESTS_DIR/gtest_dependency.pri)
include($$JSONARANGO_DIR/jsonarango.pri)

HEADERS += \
        $$TESTS_DIR/init_tests.h \
        $$TESTS_DIR/tst_arango.h \
        $$TESTS_DIR/tst_collection_api.h \
        $$TESTS_DIR/tst_query_api.h \
        $$TESTS_DIR/tst_root_api.h

SOURCES += \
        $$TESTS_DIR/main.cpp

DISTFILES += \
        Resources/docs/source.md
