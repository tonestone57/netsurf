#
# NetSurf QuickJS source file inclusion
#

S_JAVASCRIPT += quickjs/quickjs.c quickjs/qjsky.c

# TODO: Add nsgenbind rules for QuickJS when the backend is ready

# Link against quickjs
# This assumes quickjs is installed in a system-wide or project-local include/lib path
CFLAGS += $(shell pkg-config --cflags quickjs 2>/dev/null || echo "")
LDFLAGS += $(shell pkg-config --libs quickjs 2>/dev/null || echo "-lquickjs")
