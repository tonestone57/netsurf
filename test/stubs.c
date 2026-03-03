#include <dom/dom.h>
#include <stddef.h>

void dom_string_unref(struct dom_string *s) { }
dom_exception dom_string_create_interned(const uint8_t *data, size_t len, struct dom_string **s) {
    *s = (struct dom_string *)1; // dummy pointer
    return DOM_NO_ERR;
}
