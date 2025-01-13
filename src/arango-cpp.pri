

HEADERS += \
    $$ARANGO_HEADERS_DIR/arango-cpp/arangoexception.h \
    $$ARANGO_HEADERS_DIR/arango-cpp/arangoquery.h \
    $$ARANGO_HEADERS_DIR/arango-cpp/arangoconnect.h \
    $$ARANGO_HEADERS_DIR/arango-cpp/arangocollection.h \
    $$ARANGO_HEADERS_DIR/arango-cpp/arangodbusers.h \
    $$ARANGO_HEADERS_DIR/arango-cpp/arangograph.h

HEADERS += \
    $$ARANGO_DIR/arangorequests.h \
    $$ARANGO_DIR/arangodetail.h \
    $$ARANGO_DIR/arangocurl.h \
    $$ARANGO_DIR/curlobjects_pool.h


SOURCES += \
   $$ARANGO_DIR/arangocollection.cpp \
   $$ARANGO_DIR/arangodbusers.cpp \
   $$ARANGO_DIR/arangograph.cpp \
   $$ARANGO_DIR/arangorequests.cpp \
   $$ARANGO_DIR/arangoconnect.cpp \
   $$ARANGO_DIR/arangocurl.cpp \
   $$ARANGO_DIR/arangoquery.cpp



