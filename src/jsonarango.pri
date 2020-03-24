

HEADERS += \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangoexception.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangoquery.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangoconnect.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangorequests.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangocollection.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangodbusers.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangograph.h \
    $$JSONARANGO_HEADERS_DIR/jsonarango/arangocurl.h

HEADERS += \
    $$JSONARANGO_DIR/arangodetail.h


SOURCES += \
   $$JSONARANGO_DIR/arangocollection.cpp \
   $$JSONARANGO_DIR/arangodbusers.cpp \
   $$JSONARANGO_DIR/arangograph.cpp \
   $$JSONARANGO_DIR/arangorequests.cpp \
   $$JSONARANGO_DIR/arangoconnect.cpp \
   $$JSONARANGO_DIR/arangocurl.cpp \
   $$JSONARANGO_DIR/arangoquery.cpp



