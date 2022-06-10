main:
	gcc compiler.c -o vfc

install: vfc
	mv vfc /usr/bin/vfc
