main:
  gcc compile.c -o vfc

install: vfc
  mv vfc /usr/bin/vfc
