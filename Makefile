.PHONY: all clean distclean resethard q quick i2c_driver i2c_terminal_report fcgi_daemon update congatec_cgosdrv congatec_cgoslib

all: congatec_cgoslib congatec_cgosdrv i2c_driver i2c_terminal_report fcgi_daemon


i2c_driver:
	$(MAKE) -C i2c_driver

fcgi_daemon:
	$(MAKE) -C fcgi_daemon

i2c_terminal_report:
	$(MAKE) -C i2c_terminal_report

congatec_cgosdrv:
	$(MAKE) -C congatec_cgosdrv/Lx

congatec_cgoslib:
	$(MAKE) -C congatec_cgoslib/Lx


pull:
	git pull

resethard:
	git reset HEAD --hard

update:	clean pull resethard all

clean:
	$(MAKE) -C i2c_driver clean
	$(MAKE) -C i2c_terminal_report clean
	$(MAKE) -C fcgi_daemon clean
	$(MAKE) -C congatec_cgosdrv/Lx clean
	$(MAKE) -C congatec_cgoslib/Lx clean

distclean: clean

