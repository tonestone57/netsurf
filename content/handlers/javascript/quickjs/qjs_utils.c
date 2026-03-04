#include "content/handlers/javascript/quickjs/qjs_utils.h"
#include "utils/log.h"

void qjs_log_exception(JSContext *ctx, const char *prefix)
{
	JSValue exception = JS_GetException(ctx);
	const char *str = JS_ToCString(ctx, exception);
	if (str) {
		NSLOG(jserrors, ERROR, "%s: %s", prefix ? prefix : "JS Error", str);
		JS_FreeCString(ctx, str);
	}
	JS_FreeValue(ctx, exception);
}

void qjs_run_jobs(JSContext *ctx)
{
	JSContext *ctx1;
	int ret;
	while ((ret = JS_ExecutePendingJob(JS_GetRuntime(ctx), &ctx1)) != 0) {
		if (ret < 0) {
			qjs_log_exception(ctx1, "Promise job error");
		}
	}
}
