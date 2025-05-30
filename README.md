# mod_proxy_nextjs

This is an Apache HTTP server module that aims to solve the issue of not being able to set a custom response status code from Server Components, in Next.js app router.
This module can be combined with a `mod_proxy`, to proxy requests to the Next.js server and then filter the responses and set status codes accordingly.

### Apache Configuration

Simply enable the `nextjs_proxy_out` filter for HTML responses:

```conf
AddOutputFilterByType proxy_nextjs_out text/html
```

Then configure `mod_proxy`:

```conf
RequestHeader unset Accept-Encoding
ProxyPreserveHost On
ProxyPass / http://localhost:3000/         # Change as needed
ProxyPassReverse / http://localhost:3000/  # Change as needed
```

### Next.js Server Component

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
