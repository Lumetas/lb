# lb - simple browser
# See LICENSE file for copyright and license details.
.POSIX:

include config.mk

SRC = lb.c
WSRC = webext-lb.c
OBJ = $(SRC:.c=.o)
WOBJ = $(WSRC:.c=.o)
WLIB = $(WSRC:.c=.so)

all: options lb $(WLIB)

options:
	@echo lb build options:
	@echo "CC            = $(CC)"
	@echo "CFLAGS        = $(SURFCFLAGS) $(CFLAGS)"
	@echo "WEBEXTCFLAGS  = $(WEBEXTCFLAGS) $(CFLAGS)"
	@echo "LDFLAGS       = $(LDFLAGS)"

lb: $(OBJ)
	$(CC) $(SURFLDFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

$(OBJ) $(WOBJ): config.h common.h config.mk

buildTabs:
	cd tabs && make clean install

config.h:
	cp config.def.h $@

$(OBJ): $(SRC)
	$(CC) $(SURFCFLAGS) $(CFLAGS) -c $(SRC)

$(WLIB): $(WOBJ)
	$(CC) -shared -Wl,-soname,$@ $(LDFLAGS) -o $@ $? $(WEBEXTLIBS)

$(WOBJ): $(WSRC)
	$(CC) $(WEBEXTCFLAGS) $(CFLAGS) -c $(WSRC)

clean:
	rm -f lb $(OBJ)
	rm -f $(WLIB) $(WOBJ)
	rm -f ~/.local/share/applications/lb.desktop

distclean: clean
	rm -f config.h lb-$(VERSION).tar.gz

.PHONY: buildJs

buildJs:
	@echo "Объединение JavaScript файлов..."
	@mkdir -p js
	@echo "// Объединенный файл от $$(date)" > js/build.js
	@find js -name "*.js" -not -path "js/build.js" -exec sh -c '\
		echo "// Файл: {}" >> js/build.js; \
		cat {} >> js/build.js; \
		echo "" >> js/build.js' \;
	@echo "Готово! Создан js/build.js"


dist: distclean
	mkdir -p lb-$(VERSION)
	cp -R LICENSE Makefile config.mk config.def.h README \
	    lb-open.sh arg.h TODO.md lb.png \
	    lb.1 common.h $(SRC) $(WSRC) lb-$(VERSION)
	tar -cf lb-$(VERSION).tar lb-$(VERSION)
	gzip lb-$(VERSION).tar
	rm -rf lb-$(VERSION)

install: all buildJs buildTabs
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f lb $(DESTDIR)$(PREFIX)/bin
	cp -f lb.desktop ~/.local/share/applications
	chmod 755 $(DESTDIR)$(PREFIX)/bin/lb
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp -f $(WLIB) $(DESTDIR)$(LIBDIR)
	for wlib in $(WLIB); do \
	    chmod 644 $(DESTDIR)$(LIBDIR)/$$wlib; \
	done
	mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	sed "s/VERSION/$(VERSION)/g" < lb.1 > $(DESTDIR)$(MANPREFIX)/man1/lb.1
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/lb.1

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/lb
	rm -f $(DESTDIR)$(MANPREFIX)/man1/lb.1
	for wlib in $(WLIB); do \
	    rm -f $(DESTDIR)$(LIBDIR)/$$wlib; \
	done
	- rmdir $(DESTDIR)$(LIBDIR)

.PHONY: all options distclean clean dist install uninstall
