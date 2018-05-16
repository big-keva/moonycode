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
# if !defined( __codes_h__ )
# define  __codes_h__
# include <limits.h>
# include <string.h>
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
/* private codepage identifiers */
  const unsigned codepage_1251 = 1;
  const unsigned codepage_1252 = 2;
  const unsigned codepage_1254 = 7;
  const unsigned codepage_koi8 = 3;
  const unsigned codepage_866  = 4;
  const unsigned codepage_iso  = 5;
  const unsigned codepage_mac  = 6;

  const unsigned codepage_utf8 = 0x10;

/* tyranslation tables          */
  extern  const widechar        xlatWinToUtf16[];
  extern  const unsigned char*  xlatUtf16ToWin[];
  extern  const widechar        xlatUtf16Lower[];
  extern  const widechar        xlatUtf16Upper[];

  extern  const unsigned char   xlatWinToKoi[];
  extern  const unsigned char   xlatWinToDos[];
  extern  const unsigned char   xlatWinToIso[];
  extern  const unsigned char   xlatWinToMac[];
  extern  const unsigned char   xlatKoiToWin[];
  extern  const unsigned char   xlatDosToWin[];
  extern  const unsigned char   xlatIsoToWin[];
  extern  const unsigned char   xlatMacToWin[];

  // utf-8 functions

  inline  size_t  utf8cbchar( unsigned n )
  {
    return  n <= 0x0000007f ? 1 :
            n <= 0x000007ff ? 2 :
            n <= 0x0000ffff ? 3 :
            n <= 0x001fffff ? 4 :
            n <= 0x03ffffff ? 5 :
            n <= 0x7fffffff ? 6 : 7;
  }

  inline  size_t  utf8cbchar( const char* ptrtop, const char* ptrend = nullptr )
  {
    unsigned char chnext;
    int           nleast;
    const char*   ptrorg;

  // check for length
    if ( ptrtop == nullptr )
      return 0;

    if ( ptrend == nullptr )
      for ( ptrend = ptrtop; *ptrend != '\0'; ++ptrend )  (void)NULL;

  // check if nothing to scan
    if ( (ptrorg = ptrtop) >= ptrend )
      return 0;

  // check if 7-bit character
    if ( ((chnext = (unsigned char)*ptrtop++) & 0x80) == 0 )
      return 1;

  // check the number of bytes in the byte sequence if utf symbol
    if ( (chnext & 0xe0) == 0xc0 )  nleast = 1;
      else
    if ( (chnext & 0xf0) == 0xe0 )  nleast = 2;
      else
    if ( (chnext & 0xf8) == 0xf0 )  nleast = 3;
      else
    if ( (chnext & 0xfc) == 0xf8 )  nleast = 4;
      else
    if ( (chnext & 0xfe) == 0xfc )  nleast = 5;
      else
    return 1;

  // check the length of a least sequence
    if ( ptrend - ptrtop < nleast - 1 )
      return 0;

  // check least bits
    while ( nleast-- > 0 )
      if ( (*ptrtop & 0xC0) != 0x80 ) return 0;
        else ++ptrtop;

    return ptrtop - ptrorg;
  }

  inline  bool  utf8detect( const char* pszstr, size_t  cchstr = (size_t)-1 )
  {
    const char* pszend;
    bool        uppers = false;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    while ( pszstr < pszend )
    {
      size_t  nchars;

      if ( (nchars = utf8cbchar( pszstr, pszend )) == 0 )
        return pszend - pszstr <= 1 ? uppers : false;
      uppers |= ((*pszstr & 0x80) != 0);
        pszstr += nchars;
    }
    return uppers;
  }

  inline  size_t  utf8strlen( const char* pszstr, size_t  cchstr = (size_t)-1 )
  {
    const char* pszend;
    unsigned    cchlen;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    for ( cchlen = 0; pszstr < pszend; )
    {
      size_t  nchars;

      if ( (nchars = utf8cbchar( pszstr, pszend )) == 0 ) ++pszstr;
        else 
      {
        pszstr += nchars;
          ++cchlen;
      }
    }
    return cchlen;
  }

  // convertor templates

  struct __cvt_null__
  {
    static uint32_t translate ( uint32_t c )
      {  return c;  }
  };

  template <const widechar table[], class __cvt__ = __cvt_null__>
  struct __cvt_wide__
  {
    static widechar translate( uint32_t c )
      {  return c <= 0xffff ? table[__cvt__::translate( c )] : '?';  }
  };

  template <const uint8_t table[], class __cvt__ = __cvt_null__>
  struct __cvt_byte__
  {
    static uint8_t  translate( uint32_t c )
      {  return table[__cvt__::translate( c )];  }
  };

  template <class __cvt__ = __cvt_null__>
  struct __utf_1251__
  {
    static uint8_t  translate( uint32_t c )
      {
        auto  conv_c = __cvt__::translate( c );
        
        return conv_c <= 0xffff ? xlatUtf16ToWin[conv_c >> 8][conv_c & 0x00ff] : '?';
      }
  };

  //
  // utf8 helpers in separate namespace
  //
  namespace __impl_utf8__
  {

    template <class __cvt__>
    inline  uint32_t  decodechar( const char* ptrtop, size_t chsize )
    {
      uint32_t  ucchar;

    // on non-utf strings, return -1 as non-utf string error
      switch ( chsize-- )
      {
        case 0:
          return 0;
        case 1:
          return __cvt__::translate( (unsigned char)*ptrtop );
        case 2:
          ucchar = (unsigned char)(*ptrtop++ & 0x1f);
          break;
        case 3:
          ucchar = (unsigned char)(*ptrtop++ & 0x0f);
          break;
        case 4:
          ucchar = (unsigned char)(*ptrtop++ & 0x07);
          break;
        case 5:
          ucchar = (unsigned char)(*ptrtop++ & 0x03);
          break;
        case 6:
          ucchar = (unsigned char)(*ptrtop++ & 0x01);
          break;
        default:
          return (uint32_t)-1;
      }

      while ( chsize-- > 0 )
        ucchar = (ucchar << 6) | (unsigned char)(*ptrtop++ & 0x3f);

      return __cvt__::translate( ucchar );
    }

    /*
      output helpers: store uint32_t (32-bit unicode character) as utf32, utf16 and utf8 sequence
    */
    inline  size_t  serialchar( widechar* output, size_t maxlen, uint16_t source )
    {
      if ( maxlen == 0 )
        return (size_t)-1;
      *output = source;
        return 1;
    }

    inline  size_t  serialchar( widechar* output, size_t maxlen, uint32_t source )
    {
      if ( maxlen < 2 )
        return (size_t)-1;
      *output++ = 0xd800 | ((source - 0x10000) >> 10);
      *output   = 0xdc00 | ((source & 0x03ff));
        return 2;
    }

    inline  size_t  outputchar( char* output, size_t maxlen, uint32_t source )
    {
      if ( maxlen < 1 )
        return (size_t)-1;
      *output = (char)(unsigned char)source;
        return 1;
    }

    inline  size_t  outputchar( widechar* output, size_t maxlen, uint32_t source )
    {
      return source < 0x010000 ? serialchar( output, maxlen, (uint16_t)source ) :
             source < 0x110000 ? serialchar( output, maxlen, (uint32_t)source ) : (size_t)-1;
    }

    template <class O, class __cvt__ = __cvt_null__>
    inline  size_t  decodetext( O* output, size_t  maxlen, const char* pszstr, size_t  cchstr )
    {
      O*          outorg = output;
      O*          outend = output + maxlen;
      const char* pszend;

      if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
        else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

      while ( pszstr < pszend )
      {
        unsigned  ucchar;
        size_t    nchars;
        size_t    nstore;

      // on non-utf strings, return -1 as non-utf string error
        if ( (nchars = utf8cbchar( pszstr, pszend )) == 0 )
        {
          ++pszstr;
            continue;
        }
        if ( (ucchar = decodechar<__cvt__>( pszstr, nchars )) == (unsigned)-1 )
          continue;

        if ( (nstore = outputchar( output, outend - output, ucchar )) == (size_t)-1 )
          return (size_t)-1;

        output += nstore;
        pszstr += nchars;
      }

      if ( output < outend )
        *output = 0;
      return output - outorg;
    }

    template <class __cvt__ = __cvt_null__>
    inline  size_t  encodechar( char* output, size_t  cchout, uint32_t chnext )
    {
      char* outorg = output;
      char* outend = output + cchout;

      chnext = __cvt__::translate( chnext );

      if ( (chnext & ~0x007f) == 0 )
      {
        if ( output < outend )  *output++ = (char)(unsigned char)chnext;
          else return (size_t)-1;
      }
        else
      if ( (chnext & ~0x07ff) == 0 )
      {
        if ( output >= outend - 1 )
          return (size_t)-1;
        *output++ = (char)(0xC0 | (unsigned char)((chnext >> 0x06) & 0x3f));
        *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x00) & 0x3f));
      }
        else
      if ( (chnext & ~0x0ffff) == 0 )
      {
        if ( output >= outend - 2 )
          return (size_t)-1;
        *output++ = (char)(0xE0 | (unsigned char)((chnext >> 0x0c) & 0x0f));
        *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x06) & 0x3F));
        *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x00) & 0x3F));
      }
        else
      if ( chnext <= 0x10ffff )
      {
        if ( output >= outend - 3 )
          return (size_t)-1;
        *output++ = (char)(0xf0 | (unsigned char)((chnext >> 0x12) & 0x07));
        *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x0c) & 0x3F));
        *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x06) & 0x3F));
        *output++ = (char)(0x80 | (unsigned char)((chnext >> 0x00) & 0x3F));
      }
        else
      return (size_t)-1;

      if ( output < outend )
        *output = '\0';
        
      return output - outorg;
    }

    namespace character
    {
      inline  uint32_t  map( char     c ) {  return (uint32_t)(uint8_t)c;  }
      inline  uint32_t  map( uint8_t  c ) {  return (uint32_t)c;  }
      inline  uint32_t  map( uint32_t u ) {  return u;  }

      template <class S>
      inline  uint32_t  get( S& s, S e )  {  return s < e ? map( *s++ ) : 0;  }

      inline  uint32_t  one( widechar c ) {  return 0x10000 + ((c & 0x07ff) << 10);  }
      inline  uint32_t  two( widechar c ) {  return c & 0x3ff;  }

      inline  uint32_t  get( const widechar*& s, const widechar* e )
      {
        if ( s >= e )
          return 0;
        if ( *s < 0xd800 || *s > 0xdfff || s >= e - 1 )
          return *s++;
        auto hi = one( *s++ );
        auto lo = two( *s++ );
          return hi + lo;
      }

    }

    template <class S, class __cvt__ = __cvt_null__>
    inline  size_t  encodetext( char* output, size_t  cchout, const S* pwsstr, size_t  cchstr )
    {
      char*     outorg = output;
      char*     outend = output + cchout;
      const S*  pwsend;

      if ( cchstr != (size_t)-1 ) pwsend = pwsstr + cchstr;
        else for ( pwsend = pwsstr; *pwsend != 0; ++pwsend )  (void)NULL;

      while ( pwsstr < pwsend )
      {
        size_t  cchenc;

        if ( (cchenc = encodechar<__cvt__>( output, cchout, character::get( pwsstr, pwsend ) )) != (size_t)-1 )  output += cchenc;
          else return cchenc;
      }

      if ( output < outend )
        *output = '\0';

      return output - outorg;
    }

    template <class __cvt__, class O, class S>
    inline  size_t    recodetext( O*  output, size_t  cchout, const S*  source, size_t srclen = (size_t)-1 )
    {
      const S*  srcend;
      O*        outend;

      if ( srclen == (size_t)-1 )
        for ( srclen = 0; source[srclen] != 0; ++srclen ) (void)NULL;

      if ( srclen > cchout )
        return (size_t)-1;
 
      for ( outend = output + cchout, srcend = source + srclen; source < srcend; )
        *output++ = (O)__cvt__::translate( character::get( source, srcend ) );

      if ( output < outend )
        *output = 0;

      return srclen;
    }

  }

  inline  size_t  utf8decode( widechar* output, size_t  maxlen, const char* pszstr, size_t  cchstr = (size_t)-1 )
    {  return __impl_utf8__::decodetext<>( output, maxlen, pszstr, cchstr );  }

  inline  size_t  utf8encode( char* output, size_t  cchout, const widechar* pwsstr, size_t  cchstr = (size_t)-1 )
    {  return __impl_utf8__::encodetext<>( output, cchout, pwsstr, cchstr );  }

  inline  size_t  utf8encode( char* output, size_t  cchout, const uint32_t* pwsstr, size_t  cchstr = (size_t)-1 )
    {  return __impl_utf8__::encodetext<>( output, cchout, pwsstr, cchstr );  }

  template <const widechar xt[]>
  inline  char  __impl__chartocase__( unsigned codepage, char ch )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return  __cvt_byte__<xlatWinToKoi,
                  __utf_1251__<
                  __cvt_wide__<xt,
                  __cvt_wide__<xlatWinToUtf16,
                  __cvt_byte__<xlatKoiToWin> > > > >().translate( (unsigned char)ch );
        case codepage_866:
          return  __cvt_byte__<xlatWinToDos,
                  __utf_1251__<
                  __cvt_wide__<xt,
                  __cvt_wide__<xlatWinToUtf16,
                  __cvt_byte__<xlatDosToWin> > > > >().translate( (unsigned char)ch );
        case codepage_iso:
          return  __cvt_byte__<xlatWinToIso,
                  __utf_1251__<
                  __cvt_wide__<xt,
                  __cvt_wide__<xlatWinToUtf16,
                  __cvt_byte__<xlatIsoToWin> > > > >().translate( (unsigned char)ch );
        case codepage_mac:
          return  __cvt_byte__<xlatWinToMac,
                  __utf_1251__<
                  __cvt_wide__<xt,
                  __cvt_wide__<xlatWinToUtf16,
                  __cvt_byte__<xlatMacToWin> > > > >().translate( (unsigned char)ch );
        case codepage_utf8:
          if ( (ch & 0x80) != 0 )
            return 0;
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return  (char)(unsigned char)
                  __utf_1251__<
                  __cvt_wide__<xt,
                  __cvt_wide__<xlatWinToUtf16> > >().translate( (unsigned char)ch );
      }
    }

  inline  widechar  chartolower( widechar ch )
    { return __cvt_wide__<xlatUtf16Lower>().translate( ch );  }
  inline  widechar  chartoupper( widechar ch )
    { return __cvt_wide__<xlatUtf16Upper>().translate( ch );  }

  inline  char  chartolower( unsigned codepage, char ch )
    {  return __impl__chartocase__<xlatUtf16Lower>( codepage, ch );  }
  inline  char  chartoupper( unsigned codepage, char ch )
    {  return __impl__chartocase__<xlatUtf16Upper>( codepage, ch );  }

  inline  size_t  strtolower( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
    {  return __impl_utf8__::recodetext<__cvt_wide__<xlatUtf16Lower> >( o, l, s, u );  }
  inline  size_t  strtoupper( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
    {  return __impl_utf8__::recodetext<__cvt_wide__<xlatUtf16Upper> >( o, l, s, u );  }

  template <const widechar xt[]>
  inline  size_t  __impl__strtocase__( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToKoi,
            __utf_1251__<
            __cvt_wide__<xt,
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatKoiToWin> > > > > >( o, l, (const unsigned char*)s, u );
        case codepage_866:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToDos,
            __utf_1251__<
            __cvt_wide__<xt,
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatDosToWin> > > > > >( o, l, (const unsigned char*)s, u );
        case codepage_iso:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToIso,
            __utf_1251__<
            __cvt_wide__<xt,
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatIsoToWin> > > > > >( o, l, (const unsigned char*)s, u );
        case codepage_mac:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToMac,
            __utf_1251__<
            __cvt_wide__<xt,
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatMacToWin> > > > > >( o, l, (const unsigned char*)s, u );
        case codepage_utf8:
          {
            const char* e;
            char*       b;

            if ( u == (size_t)-1 )  for ( e = s; *e != 0; ++e ) (void)NULL;
              else e = s + u;

            for ( b = o; s < e; ++s )
            {
              widechar  c;
              size_t    n;

              if ( (n = utf8cbchar( s, e )) != 0 )
              {
                c = __impl_utf8__::decodechar<__cvt_wide__<xt> >( s, n );
                  s += n - 1;
                if ( (n = __impl_utf8__::encodechar<>( o, l, c )) == (size_t)-1 )
                  return n;
                o += n;
                l -= n;
              }
            }
            if ( l > 0 )
              *o = '\0';
            return o - b;
          }
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return __impl_utf8__::recodetext<
            __utf_1251__<
            __cvt_wide__<xt,
            __cvt_wide__<xlatWinToUtf16> > > >( o, l, (const unsigned char*)s, u );
      }
    }

  inline  size_t  strtolower( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {  return __impl__strtocase__<xlatUtf16Lower>( codepage, o, l, s, u );  }
  inline  size_t  strtoupper( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {  return __impl__strtocase__<xlatUtf16Upper>( codepage, o, l, s, u );  }

  inline  widechar  chartowide( unsigned codepage, char c )
    {
      switch ( codepage )
      {
        case codepage_koi8: return __cvt_wide__<xlatWinToUtf16,
                                   __cvt_byte__<xlatKoiToWin> >::translate( (unsigned char)c );
        case codepage_866:  return __cvt_wide__<xlatWinToUtf16,
                                   __cvt_byte__<xlatDosToWin> >::translate( (unsigned char)c );
        case codepage_iso:  return __cvt_wide__<xlatWinToUtf16,
                                   __cvt_byte__<xlatIsoToWin> >::translate( (unsigned char)c );
        case codepage_mac:  return __cvt_wide__<xlatWinToUtf16,
                                   __cvt_byte__<xlatMacToWin> >::translate( (unsigned char)c );
        case codepage_utf8: return (unsigned char)c;
        case codepage_1251:
        case codepage_1252:
        case codepage_1254: return __cvt_wide__<xlatWinToUtf16>::translate( (unsigned char)c );
        default:            return (widechar)-1;
      }
    }

  inline  size_t    mbcstowide( unsigned codepage, widechar* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return __impl_utf8__::recodetext<
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_866:
          return __impl_utf8__::recodetext<
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_iso:
          return __impl_utf8__::recodetext<
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_mac:
          return __impl_utf8__::recodetext<
            __cvt_wide__<xlatWinToUtf16,
            __cvt_byte__<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_utf8:
          return utf8decode( o, l, s, u );
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return __impl_utf8__::recodetext<
            __cvt_wide__<xlatWinToUtf16> >( o, l, (const unsigned char*)s, u );
      }
    }

  template <unsigned codepage, size_t N>
  inline  size_t  mbcstowide( widechar (&o)[N], const char* s, size_t l = (size_t)-1 )
    {
      return mbcstowide( codepage, o, N, s, l );
    }

  inline  size_t  widetombcs( unsigned codepage, char* o, size_t l, const widechar* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToKoi,
            __utf_1251__<> > >( o, l, s, u );
        case codepage_866:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToDos,
            __utf_1251__<> > >( o, l, s, u );
        case codepage_iso:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToIso,
            __utf_1251__<> > >( o, l, s, u );
        case codepage_mac:
          return __impl_utf8__::recodetext<
            __cvt_byte__<xlatWinToMac,
            __utf_1251__<> > >( o, l, s, u );
        case codepage_utf8:
          return utf8encode( o, l, s, u );
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return __impl_utf8__::recodetext<
            __utf_1251__<> >( o, l, s, u );
      }
    }

  inline  size_t  mbcstombcs( unsigned dstcps, char* o, size_t l, unsigned srccps, const char* s, size_t u = (size_t)-1 )
    {
      switch ( dstcps )
      {
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
          switch ( srccps )
          {
            case codepage_koi8:
              return __impl_utf8__::recodetext<__cvt_byte__<xlatKoiToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl_utf8__::recodetext<__cvt_byte__<xlatDosToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl_utf8__::recodetext<__cvt_byte__<xlatIsoToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl_utf8__::recodetext<__cvt_byte__<xlatMacToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl_utf8__::decodetext<char, __utf_1251__<> >( o, l, s, u );
            default:
              return __impl_utf8__::recodetext<__cvt_null__>( o, l, (const unsigned char*)s, u );
          }
        case codepage_koi8:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl_utf8__::recodetext<__cvt_byte__<xlatWinToKoi> >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToKoi,
                __cvt_byte__<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToKoi,
                __cvt_byte__<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToKoi,
                __cvt_byte__<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl_utf8__::decodetext<char,
                __cvt_byte__<xlatWinToKoi,
                __utf_1251__<> > >( o, l, s, u );
            default:
              return __impl_utf8__::recodetext<__cvt_null__>( o, l, (const unsigned char*)s, u );
          }
        case codepage_866:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToDos> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToDos,
                __cvt_byte__<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToDos,
                __cvt_byte__<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToDos,
                __cvt_byte__<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl_utf8__::decodetext<char, __cvt_byte__<xlatWinToDos,
                            __utf_1251__<> > >( o, l, s, u );
            default:
              return __impl_utf8__::recodetext<__cvt_null__>( o, l, (const unsigned char*)s, u );
          }
        case codepage_iso:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToIso> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToIso,
                __cvt_byte__<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToIso,
                __cvt_byte__<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToIso,
                __cvt_byte__<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl_utf8__::decodetext<char, __cvt_byte__<xlatWinToIso,
                            __utf_1251__<> > >( o, l, s, u );
            default:
              return __impl_utf8__::recodetext<__cvt_null__>( o, l, (const unsigned char*)s, u );
          }
        case codepage_mac:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToMac> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToMac,
                __cvt_byte__<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToMac,
                __cvt_byte__<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl_utf8__::recodetext<
                __cvt_byte__<xlatWinToMac,
                __cvt_byte__<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl_utf8__::decodetext<char, __cvt_byte__<xlatWinToMac,
                            __utf_1251__<> > >( o, l, s, u );
            default:
              return __impl_utf8__::recodetext<__cvt_null__>( o, l, (const unsigned char*)s, u );
          }
        case codepage_utf8:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl_utf8__::encodetext<unsigned char,
                __cvt_wide__<xlatWinToUtf16> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl_utf8__::encodetext<unsigned char,
                __cvt_wide__<xlatWinToUtf16,
                __cvt_byte__<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl_utf8__::encodetext<unsigned char,
                __cvt_wide__<xlatWinToUtf16,
                __cvt_byte__<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl_utf8__::encodetext<unsigned char,
                __cvt_wide__<xlatWinToUtf16,
                __cvt_byte__<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl_utf8__::encodetext<unsigned char,
                __cvt_wide__<xlatWinToUtf16,
                __cvt_byte__<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            default:
              return __impl_utf8__::recodetext<__cvt_null__>( o, l, (const unsigned char*)s, u );
          }
        default:
          return (size_t)-1;
      }
    }

} // codepages namespace

# endif  // __codes_h__
