.PHONY: all
all:
	cd src; $(MAKE) $(MFLAGS) all

.PHONY: clean
clean:
	cd src; $(MAKE) $(MFLAGS) clean

.PHONY: install
install: all
	install src/sms /usr/local/bin

.PHONY: uninstall
uninstall: all
	rm -f /usr/local/bin/sms

