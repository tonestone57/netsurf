#include <stdint.h>
#include <stddef.h>

/* These stubs are only needed if libdom is not available.
 * We use weak symbols so they don't conflict with a real libdom.
 * We do not include dom/dom.h here to avoid redefinition errors
 * with static inline functions in the real header.
 */

struct dom_string;

void dom_string_unref(struct dom_string *s) __attribute__((weak));
void dom_string_unref(struct dom_string *s)
{
}

int dom_string_create_interned(const uint8_t *data, size_t len, struct dom_string **s) __attribute__((weak));
int dom_string_create_interned(const uint8_t *data, size_t len, struct dom_string **s)
{
	*s = (struct dom_string *)1;
	return 0; // DOM_NO_ERR
}
