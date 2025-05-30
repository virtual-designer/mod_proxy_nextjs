# mod_proxy_nextjs

This is an Apache HTTP server module that aims to solve the issue of not being able to set a custom response status code from Server Components, in Next.js app router.
This module can be combined with a `mod_proxy`, to proxy requests to the Next.js server and then filter the responses and set status codes accordingly.

### Apache Configuration

Simply enable the `nextjs` filter:

```conf
SetOutputFilter nextjs # The filter will only apply to HTML and XHTML response types
```

If you plan to use compression and/or the upstream Next.js server triggers compression based on `Accept-Encoding` headers, you should instead set one of these:

```conf
# Enables both deflate and brotli compression support on the proxy server
SetOutputFilter INFLATE;proxy-html;nextjs;DEFLATE;BROTLI_COMPRESS

# Enables only deflate
SetOutputFilter INFLATE;proxy-html;nextjs;DEFLATE

# Enables only brotli
SetOutputFilter INFLATE;proxy-html;nextjs;BROTLI_COMPRESS
```

Then configure `mod_proxy`:

```conf
ProxyRequests Off
ProxyPreserveHost On
ProxyPass / http://localhost:3000/         # Change as needed
ProxyPassReverse / http://localhost:3000/  # Change as needed
```

### Next.js

#### Configuration

Disable compression in `next.config.ts` or `next.config.js`:

```ts
const nextConfig = {
    compress: false  
};

export default nextConfig;
```

#### Example page

This page will return `429 Too Many Requests`:

```ts
import type { Metadata } from "next";

export async function generateMetadata(): Promise<Metadata> {
    return {
        other: { "nextjs:status": "429" }
    };
}

export default function Page() {
    return (
        <h1>Too many requests</h1>
    );
}
```
