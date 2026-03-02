#
# NetSurf QuickJS source file inclusion
#

S_JAVASCRIPT += quickjs/quickjs.c quickjs/qjsky.c quickjs/xhr.c quickjs/qjs_utils.c

# Rule to invoke nsgenbind with the QuickJS backend.
# Note: This requires an updated version of nsgenbind that supports -t qjs_libdom.
# Prototype code for this backend is provided in docs/nsgenbind_prototype/
$(OBJROOT)/quickjs/binding.h $(OBJROOT)/quickjs/Makefile: content/handlers/javascript/duktape/netsurf.bnd $(BINDINGS)
	$(Q)$(MKDIR) -p $(OBJROOT)/quickjs
	$(VQ)echo " GENBIND (QJS): $<"
	$(Q)nsgenbind -t qjs_libdom $(GBFLAGS) -I content/handlers/javascript/WebIDL $< $(OBJROOT)/quickjs || ( \
		echo "ERROR: Your version of nsgenbind does not support the QuickJS backend (-t qjs_libdom)."; \
		echo "Please apply the prototype patch from docs/nsgenbind_prototype/ to your nsgenbind source."; \
		false; \
	)

# Link against quickjs.
# Use pkg-config if available, otherwise fallback to -lquickjs.
CFLAGS += $(shell pkg-config --cflags quickjs 2>/dev/null || echo "")
LDFLAGS += $(shell pkg-config --libs quickjs 2>/dev/null || echo "-lquickjs")
