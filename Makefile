.PHONY: all clean distclean resethard q quick i2c_driver i2c_terminal_report fcgi_daemon update

all: i2c_driver i2c_terminal_report fcgi_daemon


i2c_driver:
	$(MAKE) -C i2c_driver

fcgi_daemon:
	$(MAKE) -C fcgi_daemon

i2c_terminal_report:
	$(MAKE) -C i2c_terminal_report


pull:
	git pull

resethard:
	git reset HEAD --hard

update:	clean pull resethard all

clean:
	$(MAKE) -C i2c_driver clean
	$(MAKE) -C i2c_terminal_report clean
	$(MAKE) -C fcgi_daemon clean

distclean: clean

