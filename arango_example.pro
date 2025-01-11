TEMPLATE = app
CONFIG += thread console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++2a
#CONFIG += sanitizer sanitize_thread

!win32 {
  DEFINES += __unix
#QMAKE_CFLAGS += pedantic -Wall -Wextra -Wwrite-strings -Werror
#QMAKE_CXXFLAGS += -ansi -pedantic -Wall -Wextra -Weffc++
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

# Define the directory where jsonArango source code is located
ARANGO_DIR =  $$PWD/src
ARANGO_HEADERS_DIR =  $$ARANGO_DIR/../include

DEPENDPATH   += $$ARANGO_DIR
DEPENDPATH   += $$ARANGO_HEADERS_DIR

INCLUDEPATH   += $$ARANGO_DIR
INCLUDEPATH   += $$ARANGO_HEADERS_DIR

LIBS +=   -lvelocypack
win32:LIBS +=   -llibcurl
!win32:LIBS +=   -lcurl


OBJECTS_DIR   = obj

include($$ARANGO_DIR/arango-cpp.pri)

SOURCES += \
#       main.cpp
       examples/exampleCRUD.cpp
#       examples/exampleOptions.cpp
#       examples/exampleSelection.cpp
#       examples/exampleQuery.cpp
#       examples/time_test.cpp
#       examples/exampleCollections.cpp
#       examples/exampleRoot.cpp
#       examples/exampleGraphCRUD.cpp

DISTFILES += \
    Resources/docs/ArangoDB.md \
    Resources/docs/source.md

