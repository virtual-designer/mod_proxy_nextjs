#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_core.h"
#include "http_log.h"
#include "ap_config.h"
#include "apr_buckets.h"
#include "apr_strings.h"

#define MODULE_NAME     "mod_proxy_nextjs"
#define OUT_FILTER_NAME "nextjs"

typedef struct {
    apr_size_t bytes_read;
    bool enabled;
    char buffer[4096];
} proxy_nextjs_ctx_t;

static apr_status_t proxy_nextjs_output_filter(ap_filter_t *filter, apr_bucket_brigade *bb)
{ 
    request_rec *r = filter->r;
    proxy_nextjs_ctx_t *ctx = filter->ctx;

    if (!ctx)
    {
        ctx = apr_pcalloc(r->pool, sizeof (proxy_nextjs_ctx_t));
        ctx->enabled = true;
        ctx->bytes_read = 0;
        filter->ctx = ctx;

        if (strcasecmp(r->content_type, "text/html") != 0 &&
            strcasecmp(r->content_type, "application/xhtml+xml") != 0 &&
            strncasecmp(r->content_type, "text/html;", 10) != 0 &&
            strncasecmp(r->content_type, "application/xhtml+xml;", 22) != 0)
        {
            ctx->enabled = false;
        }
    }

    if (!ctx->enabled)
    {
        return ap_pass_brigade(filter->next, bb);
    }
    
    printf("Content-Type: %s\n", r->content_type);

    for (apr_bucket *b = APR_BRIGADE_FIRST(bb); b != APR_BRIGADE_SENTINEL(bb); b = APR_BUCKET_NEXT(b))
    {
        if (APR_BUCKET_IS_EOS(b))
            continue;

        if (!ctx->enabled)
            break;

        const char *data;
        apr_size_t len;
        apr_status_t rv = apr_bucket_read(b, &data, &len, APR_BLOCK_READ);

        if (rv != APR_SUCCESS)
        {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, "Failed to read bucket");
            return rv;
        }

        apr_size_t offset = ctx->bytes_read;
        apr_size_t max = 4095 - offset;

        if (len > max)
        {
            len = max;
            ctx->enabled = false;
        }
        
        memcpy(ctx->buffer + offset, data, len);
        ctx->bytes_read += len;
    }

    apr_size_t max = ctx->bytes_read >= 4095 ? 4095 : ctx->bytes_read;
    char start[] = "<meta name=\"nextjs:status\" content=\"";

    ctx->buffer[max] = 0;

    char *head_end = strstr(ctx->buffer, "</head>");
    char *meta_start = strstr(ctx->buffer, start);

    if (meta_start && head_end && meta_start < head_end)
    {
        char *end = ctx->buffer + max;
        char *status_start = meta_start + sizeof(start) - 1;

        if (status_start + 2 < end)
        {
            char status_code[4] = {0};

            memcpy(status_code, status_start, 3);
            
            char *endptr = NULL;
            apr_int64_t val = apr_strtoi64(status_code, &endptr, 10);

            if (endptr == status_code + 3)
            {
                r->status = val;
            }
        }
    }

    return ap_pass_brigade(filter->next, bb);
}

static void proxy_nextjs_register_hooks(apr_pool_t *p)
{
    ap_register_output_filter(OUT_FILTER_NAME, &proxy_nextjs_output_filter,
                              NULL, AP_FTYPE_CONTENT_SET);
}

module AP_MODULE_DECLARE_DATA proxy_nextjs_module = {
    STANDARD20_MODULE_STUFF,
    NULL, NULL, NULL, NULL, NULL,
    proxy_nextjs_register_hooks
};
