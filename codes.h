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
# include <climits>
# include <cstring>
# include <cstdint>
# include <string>

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
  using charstring = std::string;
  using widestring = std::basic_string<widechar>;

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

  /*

    utf8 - работа с utf8-строками

  */
  namespace utf8
  {

    /*
      cbchar( uint32_t uc )
      Возвращает количество символов, которыми кодируется этот символ.
    */
    inline  size_t  cbchar( uint32_t n );

    /*
      cbchar( const widechar* wcs, size_t len )
      Возвращает количество байт, кодирующих строку
    */
    inline  size_t  cbchar( const widechar* wcs, size_t len );

    /*
      cbchar( utfstr )
      Возвращает количество байт, кодирующих первый символ строки.
    */
    inline  size_t  cbchar( const char* ptrtop, size_t  cchstr = (size_t)-1 );

    /*
      detect( utfstr )
      Возвращает признак того, что строка является utf8-строкой.
    */
    inline  bool    detect( const char* pszstr, size_t  cchstr = (size_t)-1 );

    /*
      strlen( utfstr )
      Возвращает количество символов, закодированных utf8-строкой.
    */
    inline  size_t  strlen( const char* pszstr, size_t  cchstr = (size_t)-1 );

    /*
      wcslen( utfstr )
      Возвращает количество uint16_t, необходимых для представления строки.
    */
    inline  size_t  wcslen( const char* pszstr, size_t  cchstr = (size_t)-1 );
  }

  namespace __impl__
  {

    // convertor templates

    struct cvt_null
    {
      static uint32_t translate ( uint32_t c )
        {  return c;  }
    };

    template <const widechar table[], class __cvt__ = cvt_null>
    struct cvt_wide
    {
      static widechar translate( uint32_t c )
        {  return c <= 0xffff ? table[__cvt__::translate( c )] : '?';  }
    };

    template <const uint8_t table[], class __cvt__ = cvt_null>
    struct cvt_byte
    {
      static uint8_t  translate( uint32_t c )
        {  return table[__cvt__::translate( c )];  }
    };

    template <class __cvt__ = cvt_null>
    struct utf_1251
    {
      static uint8_t  translate( uint32_t c )
        {
          auto  conv_c = __cvt__::translate( c );
        
          return conv_c <= 0xffff ? xlatUtf16ToWin[conv_c >> 8][conv_c & 0x00ff] : '?';
        }
    };

    namespace utf
    {

      /*
        character get/put
      */    
      namespace character
      {
      // get characters from source string

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
            auto ucl = one( *s++ );
            auto uch = two( *s++ );
              return ucl + uch;
          }

      // put characters to output string

        inline  uint16_t  upper_16( uint32_t u )  {  return 0xd800 | ((u - 0x10000) >> 10);  }
        inline  uint16_t  lower_16( uint32_t u )  {  return 0xdc00 | ((u & 0x03ff));  }

        inline  size_t    store_16( widechar* o, size_t l, uint32_t u )
          {
            return l > 0 ? (*o = u, 1) : (size_t)-1;
          }

        inline  size_t    store_21( widechar* o, size_t l, uint32_t u )
          {
            return l > 1 ? (*o++ = upper_16( u ), *o = lower_16( u ), 2) : (size_t)-1;
          }

        inline  size_t    put( widechar* o, size_t l, uint32_t u )
          {
            return u <= 0xffff ? store_16( o, l, u ) : store_21( o, l, u );
          }

        inline  size_t    put( char* o, size_t l, uint32_t u )
          {
            return l > 0 ? (*o = (char)(uint8_t)u, 1) : (size_t)-1;
          }

      }

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

      template <class O, class __cvt__ = __impl__::cvt_null>
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
          if ( (nchars = utf8::cbchar( pszstr, pszend - pszstr )) == 0 )
          {
            ++pszstr;
              continue;
          }
          if ( (ucchar = decodechar<__cvt__>( pszstr, nchars )) == (unsigned)-1 )
            continue;

          if ( (nstore = character::put( output, outend - output, ucchar )) == (size_t)-1 )
            return (size_t)-1;

          output += nstore;
          pszstr += nchars;
        }

        if ( output < outend )
          *output = 0;
        return output - outorg;
      }

      template <class __cvt__ = __impl__::cvt_null>
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

      template <class S, class __cvt__ = __impl__::cvt_null>
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
        O*        outorg;

        if ( srclen == (size_t)-1 )
          for ( srclen = 0; source[srclen] != 0; ++srclen ) (void)NULL;

        for ( outend = (outorg = output) + cchout, srcend = source + srclen; source < srcend && output != outend; )
          *output++ = (O)__cvt__::translate( character::get( source, srcend ) );
            
        if ( output != outend ) *output = 0;
          else return (size_t)-1;

        return output - outorg;
      }

    }   // end utf8 namespace

    template <const widechar xt[]>
    inline  char  chartocase( unsigned codepage, char ch )
      {
        switch ( codepage )
        {
          case codepage_koi8:
            return  cvt_byte<xlatWinToKoi,
                    utf_1251<
                    cvt_wide<xt,
                    cvt_wide<xlatWinToUtf16,
                    cvt_byte<xlatKoiToWin> > > > >().translate( (unsigned char)ch );
          case codepage_866:
            return  cvt_byte<xlatWinToDos,
                    utf_1251<
                    cvt_wide<xt,
                    cvt_wide<xlatWinToUtf16,
                    cvt_byte<xlatDosToWin> > > > >().translate( (unsigned char)ch );
          case codepage_iso:
            return  cvt_byte<xlatWinToIso,
                    utf_1251<
                    cvt_wide<xt,
                    cvt_wide<xlatWinToUtf16,
                    cvt_byte<xlatIsoToWin> > > > >().translate( (unsigned char)ch );
          case codepage_mac:
            return  cvt_byte<xlatWinToMac,
                    utf_1251<
                    cvt_wide<xt,
                    cvt_wide<xlatWinToUtf16,
                    cvt_byte<xlatMacToWin> > > > >().translate( (unsigned char)ch );
          case codepage_utf8:
            if ( (ch & 0x80) != 0 )
              return 0;
          case codepage_1251:
          case codepage_1252:
          case codepage_1254:
          default:
            return  (char)(unsigned char)
                    utf_1251<
                    cvt_wide<xt,
                    cvt_wide<xlatWinToUtf16> > >().translate( (unsigned char)ch );
        }
      }

    template <const widechar xt[]>
    inline  size_t  strtocase( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
      {
        switch ( codepage )
        {
          case codepage_koi8:
            return __impl__::utf::recodetext<
              cvt_byte<xlatWinToKoi,
              utf_1251<
              cvt_wide<xt,
              cvt_wide<xlatWinToUtf16,
              cvt_byte<xlatKoiToWin> > > > > >( o, l, (const unsigned char*)s, u );
          case codepage_866:
            return __impl__::utf::recodetext<
              cvt_byte<xlatWinToDos,
              utf_1251<
              cvt_wide<xt,
              cvt_wide<xlatWinToUtf16,
              cvt_byte<xlatDosToWin> > > > > >( o, l, (const unsigned char*)s, u );
          case codepage_iso:
            return __impl__::utf::recodetext<
              cvt_byte<xlatWinToIso,
              utf_1251<
              cvt_wide<xt,
              cvt_wide<xlatWinToUtf16,
              cvt_byte<xlatIsoToWin> > > > > >( o, l, (const unsigned char*)s, u );
          case codepage_mac:
            return __impl__::utf::recodetext<
              cvt_byte<xlatWinToMac,
              utf_1251<
              cvt_wide<xt,
              cvt_wide<xlatWinToUtf16,
              cvt_byte<xlatMacToWin> > > > > >( o, l, (const unsigned char*)s, u );
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

                if ( (n = utf8::cbchar( s, e - s )) != 0 )
                {
                  c = utf::decodechar<__impl__::cvt_wide<xt> >( s, n );
                    s += n - 1;
                  if ( (n = utf::encodechar<>( o, l, c )) == (size_t)-1 )
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
            return utf::recodetext<
              utf_1251<
              cvt_wide<xt,
              cvt_wide<xlatWinToUtf16> > > >( o, l, (const unsigned char*)s, u );
        }
      }

  }

  /*

    utf8 - работа с utf8-строками

  */
  namespace utf8
  {

    inline  size_t  decode( widechar* output, size_t  maxlen, const char* pszstr, size_t  cchstr = (size_t)-1 )
      {  return __impl__::utf::decodetext<>( output, maxlen, pszstr, cchstr );  }

    inline  size_t  encode( char* output, size_t  cchout, const widechar* pwsstr, size_t  cchstr = (size_t)-1 )
      {  return __impl__::utf::encodetext<>( output, cchout, pwsstr, cchstr );  }

    inline  size_t  encode( char* output, size_t  cchout, const uint32_t* pwsstr, size_t  cchstr = (size_t)-1 )
      {  return __impl__::utf::encodetext<>( output, cchout, pwsstr, cchstr );  }

    inline  size_t  cbchar( uint32_t n )
    {
      return  n <= 0x0000007f ? 1 :
              n <= 0x000007ff ? 2 :
              n <= 0x0000ffff ? 3 :
              n <= 0x001fffff ? 4 :
              n <= 0x03ffffff ? 5 :
              n <= 0x7fffffff ? 6 : 7;
    }

    inline  size_t  cbchar( const char* pszstr, size_t  cchstr )
    {
      unsigned char chnext;
      int           nleast;
      const char*   pszorg;
      const char*   pszend;

    // check for length
      if ( pszstr == nullptr )
        return 0;

      if ( cchstr == (size_t)-1 )
        for ( pszend = pszorg = pszstr; *pszend != '\0'; ++pszend )  (void)NULL;
      else
        pszend = (pszorg = pszstr) + cchstr;

    // check if 7-bit character
      if ( ((chnext = (uint8_t)*pszstr++) & 0x80) == 0 )
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
      if ( pszend - pszstr < nleast - 1 )
        return 0;

    // check least bits
      while ( nleast-- > 0 )
        if ( (*pszstr & 0xC0) != 0x80 ) return 0;
          else ++pszstr;

      return pszstr - pszorg;
    }

    inline  size_t  cbchar( const widechar* pwsstr, size_t  cchstr )
    {
      const widechar* pwsend;
      size_t          length;

    // check for length
      if ( pwsstr == nullptr )
        return 0;

      if ( cchstr == (size_t)-1 )
        for ( pwsend = pwsstr; *pwsend != 0; ++pwsend )  (void)NULL;
      else
        pwsend = pwsstr + cchstr;

    // get string character by character
      for ( length = 0; pwsstr != pwsend; )
        length += cbchar( __impl__::utf::character::get( pwsstr, pwsend ) );
      
      return length;
    }

    inline  bool  detect( const char* pszstr, size_t  cchstr )
    {
      const char* pszend;
      bool        uppers = false;

      if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
        else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

      while ( pszstr < pszend )
      {
        size_t  nchars;

        if ( (nchars = cbchar( pszstr, pszend - pszstr )) == 0 )
          return pszend - pszstr <= 1 ? uppers : false;
        uppers |= ((*pszstr & 0x80) != 0);
          pszstr += nchars;
      }
      return uppers;
    }

    inline  size_t  strlen( const char* pszstr, size_t  cchstr )
    {
      const char* pszend;
      size_t      cchlen;
      size_t      nchars;

      if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
        else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

      for ( cchlen = 0; pszstr < pszend; pszstr += (nchars ? nchars : 1), cchlen += (nchars ? 1 : 0) )
        nchars = cbchar( pszstr, pszend - pszstr );

      return cchlen;
    }

    inline  size_t  wcslen( const char* pszstr, size_t  cchstr )
    {
      const char* pszend;
      size_t      cchlen;
      size_t      nchars;

      if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
        else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

      for ( cchlen = 0; pszstr < pszend; pszstr += (nchars ? nchars : 1), cchlen += (nchars > 3 ? 2 : (nchars ? 1 : 0)) )
        nchars = cbchar( pszstr, pszend - pszstr );

      return cchlen;
    }

  }

  inline  uint32_t  chartolower( uint32_t ch )    { return ch <= 0xffff ? __impl__::cvt_wide<xlatUtf16Lower>().translate( ch ) : ch;  }
  inline  uint32_t  chartoupper( uint32_t ch )    { return ch <= 0xffff ? __impl__::cvt_wide<xlatUtf16Upper>().translate( ch ) : ch;  }

  inline  char      chartolower( unsigned codepage, char ch ) {  return __impl__::chartocase<xlatUtf16Lower>( codepage, ch );  }
  inline  char      chartoupper( unsigned codepage, char ch ) {  return __impl__::chartocase<xlatUtf16Upper>( codepage, ch );  }

  inline  size_t    strtolower( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
    {  return __impl__::utf::recodetext<__impl__::cvt_wide<xlatUtf16Lower> >( o, l, s, u );  }

  inline  size_t    strtoupper( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
    {  return __impl__::utf::recodetext<__impl__::cvt_wide<xlatUtf16Upper> >( o, l, s, u );  }

  inline  size_t  strtolower( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {  return __impl__::strtocase<xlatUtf16Lower>( codepage, o, l, s, u );  }

  inline  size_t  strtoupper( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {  return __impl__::strtocase<xlatUtf16Upper>( codepage, o, l, s, u );  }

  inline  widestring  strtolower( const widestring& s )
    {
      widestring  out( s.c_str(), s.length() );

      __impl__::utf::recodetext<__impl__::cvt_wide<xlatUtf16Lower> >( (widechar*)out.c_str(), out.length(),
        s.c_str(), s.length() );

      return out;
    }

  inline  widestring  strtoupper( const widestring& s )
    {
      widestring  out( s );

      __impl__::utf::recodetext<__impl__::cvt_wide<xlatUtf16Upper> >( (widechar*)out.c_str(), out.length(),
        s.c_str(), s.length() );

      return out;
    }

  inline  widechar  chartowide( unsigned codepage, char c )
    {
      switch ( codepage )
      {
        case codepage_koi8: return __impl__::cvt_wide<xlatWinToUtf16,
                                   __impl__::cvt_byte<xlatKoiToWin> >::translate( (unsigned char)c );
        case codepage_866:  return __impl__::cvt_wide<xlatWinToUtf16,
                                   __impl__::cvt_byte<xlatDosToWin> >::translate( (unsigned char)c );
        case codepage_iso:  return __impl__::cvt_wide<xlatWinToUtf16,
                                   __impl__::cvt_byte<xlatIsoToWin> >::translate( (unsigned char)c );
        case codepage_mac:  return __impl__::cvt_wide<xlatWinToUtf16,
                                   __impl__::cvt_byte<xlatMacToWin> >::translate( (unsigned char)c );
        case codepage_utf8: return (unsigned char)c;
        case codepage_1251:
        case codepage_1252:
        case codepage_1254: return __impl__::cvt_wide<xlatWinToUtf16>::translate( (unsigned char)c );
        default:            return (widechar)-1;
      }
    }

  inline  size_t    mbcstowide( unsigned codepage, widechar* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return __impl__::utf::recodetext<
            __impl__::cvt_wide<xlatWinToUtf16,
            __impl__::cvt_byte<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_866:
          return __impl__::utf::recodetext<
            __impl__::cvt_wide<xlatWinToUtf16,
            __impl__::cvt_byte<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_iso:
          return __impl__::utf::recodetext<
            __impl__::cvt_wide<xlatWinToUtf16,
            __impl__::cvt_byte<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_mac:
          return __impl__::utf::recodetext<
            __impl__::cvt_wide<xlatWinToUtf16,
            __impl__::cvt_byte<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
        case codepage_utf8:
          return utf8::decode( o, l, s, u );
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return __impl__::utf::recodetext<
            __impl__::cvt_wide<xlatWinToUtf16> >( o, l, (const unsigned char*)s, u );
      }
    }

  template <size_t N>
  inline  size_t  mbcstowide( unsigned codepage, widechar (&o)[N], const char* s, size_t l = (size_t)-1 )
    {  return mbcstowide( codepage, o, N, s, l );  }

  template <unsigned codepage, size_t N>
  inline  size_t  mbcstowide( widechar (&o)[N], const char* s, size_t l = (size_t)-1 )
    {  return mbcstowide( codepage, o, N, s, l );  }

  inline  size_t  widetombcs( unsigned codepage, char* o, size_t l, const widechar* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return __impl__::utf::recodetext<
            __impl__::cvt_byte<xlatWinToKoi,
            __impl__::utf_1251<> > >( o, l, s, u );
        case codepage_866:
          return __impl__::utf::recodetext<
            __impl__::cvt_byte<xlatWinToDos,
            __impl__::utf_1251<> > >( o, l, s, u );
        case codepage_iso:
          return __impl__::utf::recodetext<
            __impl__::cvt_byte<xlatWinToIso,
            __impl__::utf_1251<> > >( o, l, s, u );
        case codepage_mac:
          return __impl__::utf::recodetext<
            __impl__::cvt_byte<xlatWinToMac,
            __impl__::utf_1251<> > >( o, l, s, u );
        case codepage_utf8:
          return utf8::encode( o, l, s, u );
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return __impl__::utf::recodetext<
            __impl__::utf_1251<> >( o, l, s, u );
      }
    }

  template <size_t N>
  inline  size_t  widetombcs( unsigned codepage, char (&o)[N], const widechar* s, size_t u = (size_t)-1 )
    {  return widetombcs( codepage, o, N, s, u );  }

  template <unsigned codepage, size_t N>
  inline  size_t  widetombcs( char (&o)[N], const widechar* s, size_t u = (size_t)-1 )
    {  return widetombcs( codepage, o, N, s, u );  }

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
              return __impl__::utf::recodetext<__impl__::cvt_byte<xlatKoiToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl__::utf::recodetext<__impl__::cvt_byte<xlatDosToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl__::utf::recodetext<__impl__::cvt_byte<xlatIsoToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl__::utf::recodetext<__impl__::cvt_byte<xlatMacToWin> >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl__::utf::decodetext<char, __impl__::utf_1251<> >( o, l, s, u );
            default:
              return __impl__::utf::recodetext<__impl__::cvt_null>( o, l, (const unsigned char*)s, u );
          }
        case codepage_koi8:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl__::utf::recodetext<__impl__::cvt_byte<xlatWinToKoi> >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToKoi,
                __impl__::cvt_byte<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToKoi,
                __impl__::cvt_byte<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToKoi,
                __impl__::cvt_byte<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl__::utf::decodetext<char,
                __impl__::cvt_byte<xlatWinToKoi,
                __impl__::utf_1251<> > >( o, l, s, u );
            default:
              return __impl__::utf::recodetext<__impl__::cvt_null>( o, l, (const unsigned char*)s, u );
          }
        case codepage_866:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToDos> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToDos,
                __impl__::cvt_byte<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToDos,
                __impl__::cvt_byte<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToDos,
                __impl__::cvt_byte<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl__::utf::decodetext<char, __impl__::cvt_byte<xlatWinToDos,
                            __impl__::utf_1251<> > >( o, l, s, u );
            default:
              return __impl__::utf::recodetext<__impl__::cvt_null>( o, l, (const unsigned char*)s, u );
          }
        case codepage_iso:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToIso> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToIso,
                __impl__::cvt_byte<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToIso,
                __impl__::cvt_byte<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToIso,
                __impl__::cvt_byte<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl__::utf::decodetext<char, __impl__::cvt_byte<xlatWinToIso,
                            __impl__::utf_1251<> > >( o, l, s, u );
            default:
              return __impl__::utf::recodetext<__impl__::cvt_null>( o, l, (const unsigned char*)s, u );
          }
        case codepage_mac:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToMac> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToMac,
                __impl__::cvt_byte<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToMac,
                __impl__::cvt_byte<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl__::utf::recodetext<
                __impl__::cvt_byte<xlatWinToMac,
                __impl__::cvt_byte<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_utf8:
              return __impl__::utf::decodetext<char, __impl__::cvt_byte<xlatWinToMac,
                            __impl__::utf_1251<> > >( o, l, s, u );
            default:
              return __impl__::utf::recodetext<__impl__::cvt_null>( o, l, (const unsigned char*)s, u );
          }
        case codepage_utf8:
          switch ( srccps )
          {
            case codepage_1251:
            case codepage_1252:
            case codepage_1254:
              return __impl__::utf::encodetext<unsigned char,
                __impl__::cvt_wide<xlatWinToUtf16> >( o, l, (const unsigned char*)s, u );
            case codepage_koi8:
              return __impl__::utf::encodetext<unsigned char,
                __impl__::cvt_wide<xlatWinToUtf16,
                __impl__::cvt_byte<xlatKoiToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_iso:
              return __impl__::utf::encodetext<unsigned char,
                __impl__::cvt_wide<xlatWinToUtf16,
                __impl__::cvt_byte<xlatIsoToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_866:
              return __impl__::utf::encodetext<unsigned char,
                __impl__::cvt_wide<xlatWinToUtf16,
                __impl__::cvt_byte<xlatDosToWin> > >( o, l, (const unsigned char*)s, u );
            case codepage_mac:
              return __impl__::utf::encodetext<unsigned char,
                __impl__::cvt_wide<xlatWinToUtf16,
                __impl__::cvt_byte<xlatMacToWin> > >( o, l, (const unsigned char*)s, u );
            default:
              return __impl__::utf::recodetext<__impl__::cvt_null>( o, l, (const unsigned char*)s, u );
          }
        default:
          return (size_t)-1;
      }
    }

  // std::string wrappers

  inline  charstring  widetombcs( unsigned target_cp, const widechar* s, size_t l = (size_t)-1 )
    {
      std::string st;
      size_t      cc;

      if ( s == nullptr )
        return st;
      if ( l == (size_t)-1 )
        for ( l = 0; s[l] != 0; ++l ) (void)NULL;

      st.assign( cc = (target_cp == codepage_utf8 ? l * 6 : l) + 1, 0 );
        st.resize( widetombcs( target_cp, (char*)st.c_str(), cc, s, l ) );
      return st;
    }

  inline  widestring  mbcstowide( unsigned source_cp, const char* s, size_t l = (size_t)-1 )
    {
      widestring  ws;
      size_t      cc;

      if ( s == nullptr )
        return ws;
      if ( l == (size_t)-1 )
        for ( l = 0; s[l] != 0; ++l ) (void)NULL;

      ws.assign( cc = (source_cp == codepage_utf8 ? utf8::strlen( s, l ) : l) + 1, 0 );
        ws.resize( mbcstowide( source_cp, (widechar*)ws.c_str(), cc, s, l ) );
      return ws;
    }

  inline  charstring  mbcstombcs( unsigned target_cp, unsigned source_cp, const char* s, size_t l = (size_t)-1 )
    {
      std::string cs;
      size_t      cc;

      if ( l == (size_t)-1 )
        for ( l = 0; s[l] != 0; ++l ) (void)NULL;

      if ( target_cp == source_cp )
        return std::string( s, l );

      cs.assign( cc = (target_cp == codepages::codepage_utf8 ? l * 6 : l) + 1, 0 );
        cs.resize( mbcstombcs( target_cp, (char*)cs.c_str(), cc, source_cp, s, l ) );
      return cs;
    }

  inline  charstring  widetombcs( unsigned target_cp, const widestring& s )
    {  return widetombcs( target_cp, s.c_str(), s.length() );  }
  inline  widestring  mbcstowide( unsigned source_cp, const charstring& s )
    {  return mbcstowide( source_cp, s.c_str(), s.length() );  }
  inline  charstring  mbcstombcs( unsigned target_cp, unsigned source_cp, const charstring& s )
    {  return mbcstombcs( target_cp, source_cp, s.c_str(), s.length() );  }

} // codepages namespace

# endif  // __codes_h__
