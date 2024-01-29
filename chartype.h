/******************************************************************************

The MIT License (MIT)

Copyright (c) 2000-2016 Андрей Коваленко aka Keva
  keva@meta.ua
  keva@rambler.ru
  skype: big_keva
  phone: +7(495)648-4058, +7(916)015-5592

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

=============================================================================

Данная лицензия разрешает лицам, получившим копию данного программного обеспечения
и сопутствующей документации (в дальнейшем именуемыми «Программное Обеспечение»),
безвозмездно использовать Программное Обеспечение без ограничений, включая неограниченное
право на использование, копирование, изменение, слияние, публикацию, распространение,
сублицензирование и/или продажу копий Программного Обеспечения, а также лицам, которым
предоставляется данное Программное Обеспечение, при соблюдении следующих условий:

Указанное выше уведомление об авторском праве и данные условия должны быть включены во
все копии или значимые части данного Программного Обеспечения.

ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ,
ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ ПРИГОДНОСТИ,
СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ
ИМИ.

НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ,
ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ
СИТУАЦИИ, ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ
С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ.

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
  const unsigned char cat_t = 0x0e;
  const unsigned char cat_u = 0x0f;

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

  const unsigned char cat_Sm = cat_S + cat_m;

  class __impl_is_category_last__
  {
    public: bool  operator () ( unsigned char c ) const  {  (void)c;  return false;  }
  };

  template <unsigned uvalue, class acnext = __impl_is_category_last__>
  class is_category
  {
    public: bool  operator ()( unsigned char c ) const  {  return c == uvalue || acnext()( c );  }
  };

  template <class ... categories>
  inline  bool  __is_chartype_category__( unsigned char _typ, unsigned char _cat, categories... _set )
  {  return _typ == _cat || __is_chartype_category__( _typ, _set... );  }

  template <>
  inline  bool  __is_chartype_category__( unsigned char _typ, unsigned char _cat )
  {  return _typ == _cat;  }

  template <class ... categories>
  inline  bool  is_char_category( widechar _chr, unsigned char _cat, categories... _set )
  {  return __is_chartype_category__( charType[_chr], _cat, _set... );  }

  inline  bool  IsSpace( widechar c )
  {
    return is_category<codepages::cat_Cc,
           is_category<codepages::cat_Zs,
           is_category<codepages::cat_Zl,
           is_category<codepages::cat_Zp>>>>()( codepages::charType[c] );
  }

  inline  bool  IsEmpty( widechar c )
  {
    return is_category<codepages::cat_Cf>()( codepages::charType[c] );
  }

  inline  bool  IsBlank( widechar c )
  {
    return IsSpace( c ) || IsEmpty( c );
  }

}

#endif  // __codepages_chartype_h__
