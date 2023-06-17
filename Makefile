##
# cfetch
#
# @file
# @version 0.1

all:
	mkdir -p build
	gcc -o build/cfetch src/cfetch.c

clean:
	rm -rf build

install:
	mkdir -p ${DESTDIR}/usr/bin
	cp build/cfetch ${DESTDIR}/usr/bin

uninstall:
	rm -rf ${DESTDIR}/usr/bin/cfetch

.PHONY: all clean install uninstall

# end
