

HEADERS += \
    $$JSONARANGO_HEADERS_DIR/arangodbusers.h \
    $$JSONARANGO_HEADERS_DIR/arangograph.h \
    $$JSONARANGO_HEADERS_DIR/arangorequests.h \
    $$JSONARANGO_HEADERS_DIR/connection_data.h \
    $$JSONARANGO_HEADERS_DIR/arangocollection.h

HEADERS += \
    $$JSONARANGO_DIR/jsondetail.h \
    $$JSONARANGO_DIR/arangocurl.h \


SOURCES += \
   $$JSONARANGO_DIR/arangocollection.cpp \
   $$JSONARANGO_DIR/arangodbusers.cpp \
   $$JSONARANGO_DIR/arangograph.cpp \
   $$JSONARANGO_DIR/arangorequests.cpp \
   $$JSONARANGO_DIR/connection_data.cpp \
   $$JSONARANGO_DIR/arangocurl.cpp



