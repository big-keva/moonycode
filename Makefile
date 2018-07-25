TARGET=LIB
OUTPUT=$(ProjectRoot)/lib/libcodes$(_LIB)
SOURCE=chartype.cpp  utf16_lo.cpp  utf16_up.cpp  utf16win.cpp  win_dos.cpp  win_iso.cpp  win_koi.cpp  win_mac.cpp

include build/build.gmake
