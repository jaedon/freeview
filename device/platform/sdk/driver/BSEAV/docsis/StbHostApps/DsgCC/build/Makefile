################    Compile Rules
################    Install Rules
install:
#	./build dsgcc slim pvnet applib cablecard
#temporary modify for release compile. 2012.3.24
	./build dsgcc pvnet applib cablecard
###	./build dsgcc slim pvnet applib cablecard rswdload
install_debug:
	./build dsgcc pvnet applib 

################    Clean Rules
clean:
	./build dsgcc slim clean

clean_debug:
	./build dsgcc clean

clean_all: clean clean_debug
	@rm -f compile.log
	@rm -f dsgcclib.a

distclean: clean_all

.PHONY: clean clean_all distclean install

