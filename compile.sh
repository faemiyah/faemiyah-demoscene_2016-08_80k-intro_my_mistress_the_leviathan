#!/bin/sh

DNLOAD="../dnload/dnload.py"
if [ ! -f "${DNLOAD}" ] ; then
  DNLOAD="/usr/local/src/dnload/dnload.py"
  if [ ! -f "${DNLOAD}" ] ; then
    DNLOAD="/usr/local/src/faemiyah-demoscene/dnload/dnload.py"
    if [ ! -f "${DNLOAD}" ] ; then
      echo "${0}: could not find dnload.py"
      exit 1
    fi
  fi
fi

if [ ! -f "src/dnload.h" ] ; then
  touch src/dnload.h
fi

if [ -f "/opt/vc/lib/libbcm_host.so" ] ; then # VideoCore
  python "${DNLOAD}" -v -c src/intro.cpp -o src/my_mistress_the_leviathan -lc -lgcc -lm -lbcm_host -lEGL -lGLESv2 -lfreetype -lSDL2 -m vanilla $*
elif [ -d "/usr/lib/arm-linux-gnueabihf/mali-egl" ] ; then # Mali
  python "${DNLOAD}" -v -c src/intro.cpp -o src/my_mistress_the_leviathan --rpath "/usr/local/lib" -lc -lgcc -ldl -m dlfcn $*
else
  python "${DNLOAD}" -c src/intro.cpp -o src/my_mistress_the_leviathan -v -lc -lgcc -lm -lGL -lfreetype -lSDL2 -m hash $*
fi
if test $? -ne 0 ; then
  echo "${0}: compilation failed"
  exit 1
fi

exit 0
