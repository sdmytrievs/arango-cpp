TEMPLATE = app
CONFIG += thread console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++2a

DEFINES += TestRemoteServer

!win32 {
  DEFINES += TestLocalServer
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
  #CONFIG += staticlib
  #QMAKE_CXXFLAGS_DEBUG += -MTd
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
}

# Define the directory where source code is located
ARANGO_DIR =  $$PWD/src
ARANGO_HEADERS_DIR =  $$ARANGO_DIR/../include
TESTS_DIR =  $$PWD/tests

DEPENDPATH   += $$ARANGO_DIR
DEPENDPATH   += $$ARANGO_HEADERS_DIR
DEPENDPATH   += $$TESTS_DIR

INCLUDEPATH   += $$ARANGO_DIR
INCLUDEPATH   += $$ARANGO_HEADERS_DIR
INCLUDEPATH   += $$TESTS_DIR

LIBS +=   -lvelocypack
win32:LIBS +=   -llibcurl
!win32:LIBS +=   -lcurl

OBJECTS_DIR   = obj

include($$TESTS_DIR/gtest_dependency.pri)
include($$ARANGO_DIR/arango-cpp.pri)

HEADERS += \
        $$TESTS_DIR/init_tests.h \
        $$TESTS_DIR/tst_arango.h \
        $$TESTS_DIR/tst_collection_api.h \
        $$TESTS_DIR/tst_query_api.h \
        $$TESTS_DIR/tst_root_api.h \
        $$TESTS_DIR/tst_graph_api.h

SOURCES += \
        $$TESTS_DIR/main.cpp

DISTFILES += \
        Resources/docs/source.md
