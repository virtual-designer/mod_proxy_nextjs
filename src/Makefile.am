APXS = @APXS@
APACHE_CFLAGS = @APACHE_CFLAGS@
APACHE_INCLUDES = @APACHE_INCLUDES@
APACHE_LDFLAGS = @APACHE_LDFLAGS@

all:
	$(APXS) -c mod_proxy_nextjs.c

install-exec-hook:
	$(APXS) -i -a mod_proxy_nextjs.la
