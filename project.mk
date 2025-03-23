APP_NAME = json
VERSION = 0.1.4
INSTALL_PATH = /opt
# CFLAGS +=
PACKAGE_CONTENTS := ${SRC_PATH}/${APP_NAME}.h lib${APP_NAME}.a

PTREE_PROJECT_ID = 61120549
DLINKED_LIST_PROJECT_ID = 18027338
UNITEST_PROJECT_ID = 11728468

DEPENDENCIES += gitlab/${PTREE_PROJECT_ID}/0.0.9
DEPENDENCIES += gitlab/${DLINKED_LIST_PROJECT_ID}/0.1.2
DEPENDENCIES += gitlab/${UNITEST_PROJECT_ID}/0.2.0

STATIC_DEP = ptree dlinked_list
SHARED_DEP = m

LDFLAGS += ${STATIC_DEP:%=-l%} ${SHARED_DEP:%=-l%}

