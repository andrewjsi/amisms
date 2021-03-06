.PHONY: all
all:
ifeq ($(wildcard libamievent/ami.h),)
	@echo "* "
	@echo "* libamievent submodule not found!"
	@echo "* To download this project with all submodules included, run:"
	@echo "* "
	@echo "* git clone --recursive git://github.com/andrewjsi/amisms"
	@echo "* "
	@exit 1
endif

	cd src; $(MAKE) $(MFLAGS) all

.PHONY: test
test: all
	cd t; $(MAKE) $(MFLAGS) all

.PHONY: clean
clean:
	cd src; $(MAKE) $(MFLAGS) clean
	cd t; $(MAKE) $(MFLAGS) clean

.PHONY: install
install: all
	install src/sms /usr/local/bin
	install src/phone_number_validator /usr/local/bin/phone_number_validator

.PHONY: uninstall
uninstall: all
	rm -f /usr/local/bin/sms
	rm -f /usr/local/bin/phone_number_validator

