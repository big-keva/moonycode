/******************************************************************************

    libcodes - codepages conversion library.
    Copyright (C) 2005-2016 Andrew Kovalenko aka Keva

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    Contacts:
      email: keva@meta.ua, keva@rambler.ru
      Skype: big_keva
      Phone: +7(495)648-4058, +7(926)513-2991

******************************************************************************/
# if !defined( __codepages_chartype_h__ )
# define __codepages_chartype_h__
# include <stdint.h>

# if !defined( __widechar_defined__ )
# define  __widechar_defined__
#   if defined(WCHAR_MAX) && (WCHAR_MAX >> 16) == 0
    typedef wchar_t         widechar;
#   else
    typedef unsigned short  widechar;
#   endif  // size
# endif  // __widechar_defined__

namespace codepages
{
  extern unsigned char  charType[0x10000];

  const unsigned char cat_C = 0x10;
  const unsigned char cat_L = 0x20;
  const unsigned char cat_M = 0x30;
  const unsigned char cat_N = 0x40;
  const unsigned char cat_P = 0x50;
  const unsigned char cat_S = 0x60;
  const unsigned char cat_Z = 0x70;

  const unsigned char cat_a = 0x01;
  const unsigned char cat_c = 0x02;
  const unsigned char cat_d = 0x03;
  const unsigned char cat_e = 0x04;
  const unsigned char cat_f = 0x05;
  const unsigned char cat_i = 0x06;
  const unsigned char cat_k = 0x07;
  const unsigned char cat_l = 0x08;
  const unsigned char cat_m = 0x09;
  const unsigned char cat_n = 0x0a;
  const unsigned char cat_o = 0x0b;
  const unsigned char cat_p = 0x0c;
  const unsigned char cat_s = 0x0d;

  const unsigned char cat_Cc = cat_C + cat_c;
  const unsigned char cat_Zs = cat_Z + cat_s;
  const unsigned char cat_Zl = cat_Z + cat_l;
  const unsigned char cat_Zp = cat_Z + cat_p;

  const unsigned char cat_Cf = cat_C + cat_f;

  const unsigned char cat_Pd = cat_P + cat_d;
  const unsigned char cat_Pe = cat_P + cat_e;
  const unsigned char cat_Pf = cat_P + cat_f;
  const unsigned char cat_Pi = cat_P + cat_i;
  const unsigned char cat_Po = cat_P + cat_o;
  const unsigned char cat_Ps = cat_P + cat_s;

  const unsigned char cat_Nd = cat_N + cat_d;
}

#endif  // __codepages_chartype_h__
