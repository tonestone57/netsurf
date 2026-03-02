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
		echo "NOTE: Your nsgenbind tool does not yet support the experimental QuickJS backend (-t qjs_libdom)."; \
		echo "This is required ONLY if you are explicitly building with NETSURF_USE_QUICKJS=YES."; \
		echo "To proceed with QuickJS development, please apply the prototype patch provided in docs/nsgenbind_prototype/."; \
		false; \
	)

# Generator for the C include representing the generics.js
$(OBJROOT)/quickjs/generics.js.inc: content/handlers/javascript/duktape/generics.js $(TOOLROOT)/xxd
	$(Q)$(MKDIR) -p $(OBJROOT)/quickjs
	$(VQ)echo "     XXD: $<"
	$(Q)$(TOOLROOT)/xxd -i $< $@.tmp
	$(Q)sed -e 's/content_handlers_javascript_duktape_generics_js/generics_js/' $@.tmp > $@

# Generator for the C include representing the polyfill.js
$(OBJROOT)/quickjs/polyfill.js.inc: content/handlers/javascript/duktape/polyfill.js $(TOOLROOT)/xxd
	$(Q)$(MKDIR) -p $(OBJROOT)/quickjs
	$(VQ)echo "     XXD: $<"
	$(Q)$(TOOLROOT)/xxd -i $< $@.tmp
	$(Q)sed -e 's/content_handlers_javascript_duktape_polyfill_js/polyfill_js/' $@.tmp > $@

# Link against quickjs.
# Use pkg-config if available, otherwise fallback to standard paths.
QJS_CFLAGS := $(shell pkg-config --cflags quickjs 2>/dev/null)
ifeq ($(QJS_CFLAGS),)
  QJS_CFLAGS := -I/usr/include/quickjs
endif
CFLAGS += $(QJS_CFLAGS)

QJS_LDFLAGS := $(shell pkg-config --libs quickjs 2>/dev/null)
ifeq ($(QJS_LDFLAGS),)
  QJS_LDFLAGS := -L/usr/lib/quickjs -lquickjs
endif
LDFLAGS += $(QJS_LDFLAGS)

# Add dependencies for objects
$(OBJROOT)/content_handlers_javascript_quickjs_qjsky.o: $(OBJROOT)/quickjs/generics.js.inc $(OBJROOT)/quickjs/polyfill.js.inc
$(OBJROOT)/content_handlers_javascript_quickjs_quickjs.o: $(OBJROOT)/quickjs/generics.js.inc $(OBJROOT)/quickjs/polyfill.js.inc
