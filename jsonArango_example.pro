TEMPLATE = app
CONFIG += thread console c++14
CONFIG -= app_bundle
CONFIG -= qt

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
  INCLUDEPATH   += "C:\usr\local\include"
  DEPENDPATH   += "C:\usr\local\include"
  LIBPATH += "C:\usr\local\lib"
}

# Define the directory where jsonArango source code is located
JSONARANGO_DIR =  $$PWD/src
JSONARANGO_HEADERS_DIR =  $$JSONARANGO_DIR/../include/jsonarango

DEPENDPATH   += $$JSONARANGO_DIR
DEPENDPATH   += $$JSONARANGO_HEADERS_DIR

INCLUDEPATH   += $$JSONARANGO_DIR
INCLUDEPATH   += $$JSONARANGO_HEADERS_DIR

LIBS +=  -lcurl  -lvelocypack

OBJECTS_DIR   = obj

include($$JSONARANGO_DIR/jsonarango.pri)

SOURCES += \
       main.cpp
#      examples/create_example.cpp

DISTFILES += \
    Resources/docs/ArangoDB.md \
    Resources/docs/source.md
