APP_NAME = json
VERSION = 0.0.2
INSTALL_PATH = /opt
# CFLAGS +=

DEPENDENCIES = Kick1911/Hash_Table/0.1.0/libhash_table.a

STATIC_DEP = hash_table
SHARED_DEP =

LDFLAGS += ${STATIC_DEP:%=-l%} ${SHARED_DEP:%=-l%}

