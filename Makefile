CPPFLAGS=$(shell pkg-config --cflags gtk+-2.0) -Wall -W
LDFLAGS=$(shell pkg-config --libs gtk+-2.0)
all: awmenu

clean:
	rm -f awmenu

test: awmenu
	./test.sh
install:
	install -d $(DESTDIR)/usr/bin
	install -D -m755 awmenu $(DESTDIR)/usr/bin/awmenu

uninstall:
	rm -rf $(DESTDIR)/usr/bin/awmenu
