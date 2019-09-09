

HEADERS += \
    $$JSONARANGO_HEADERS_DIR/arangoexception.h \
    $$JSONARANGO_HEADERS_DIR/query.h \
    $$JSONARANGO_HEADERS_DIR/arangoconnect.h \
    $$JSONARANGO_HEADERS_DIR/arangorequests.h \
    $$JSONARANGO_HEADERS_DIR/arangocollection.h \
    $$JSONARANGO_HEADERS_DIR/arangodbusers.h \
    $$JSONARANGO_HEADERS_DIR/arangograph.h \

HEADERS += \
    $$JSONARANGO_DIR/arangodetail.h \
    $$JSONARANGO_DIR/arangocurl.h \


SOURCES += \
   $$JSONARANGO_DIR/arangocollection.cpp \
   $$JSONARANGO_DIR/arangodbusers.cpp \
   $$JSONARANGO_DIR/arangograph.cpp \
   $$JSONARANGO_DIR/arangorequests.cpp \
   $$JSONARANGO_DIR/arangoconnect.cpp \
   $$JSONARANGO_DIR/arangocurl.cpp \
   $$JSONARANGO_DIR/query.cpp



