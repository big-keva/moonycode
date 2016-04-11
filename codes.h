# if !defined( __codes_h__ )
# define  __codes_h__
# include <limits.h>
# include <stdint.h>
# include <string.h>

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
  extern  const widechar  xlatWinToUtf16[];
  extern  const char*     xlatUtf16ToWin[];
  extern  const widechar  xlatUtf16Lower[];
  extern  const widechar  xlatUtf16Upper[];

  extern  const widechar  xlatWinToKoi[];
  extern  const widechar  xlatWinToDos[];
  extern  const widechar  xlatWinToIso[];
  extern  const widechar  xlatWinToMac[];
  extern  const widechar  xlatKoiToWin[];
  extern  const widechar  xlatDosToWin[];
  extern  const widechar  xlatIsoToWin[];
  extern  const widechar  xlatMacToWin[];

  inline  size_t  utf8cbchar( const char* ptrtop, const char* ptrend )
  {
    unsigned char chnext;
    int           nleast;
    const char*   ptrorg;

  // check if nothing to scan
    if ( (ptrorg = ptrtop) >= ptrend )
      return 0;
    else chnext = (unsigned char)*ptrtop++;

  // check if 7-bit character
    if ( (chnext & 0x80) == 0 )
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
      if ( (*ptrtop & 0xC0) != 0x80 )
        return 0;
      else ++ptrtop;
    return ptrtop - ptrorg;
  }

  inline  widechar  utf8decode( const char* ptrtop, size_t chsize )
  {
    unsigned  ucchar;

  // on non-utf strings, return -1 as non-utf string error
    switch ( chsize-- )
    {
      case 1:
        return (unsigned char)*ptrtop;
      case 2:
        ucchar = (unsigned)(unsigned char)(*ptrtop++ & 0x1f);
        break;
      case 3:
        ucchar = (unsigned)(unsigned char)(*ptrtop++ & 0x0f);
        break;
      case 4:
        ucchar = (unsigned)(unsigned char)(*ptrtop++ & 0x07);
        break;
      case 5:
        ucchar = (unsigned)(unsigned char)(*ptrtop++ & 0x03);
        break;
      case 6:
        ucchar = (unsigned)(unsigned char)(*ptrtop++ & 0x01);
        break;
      default:
        return (widechar)-1;
    }
    while ( chsize-- > 0 )
      ucchar = (ucchar << 6) | (unsigned char)(*ptrtop++ & 0x3f);
    return ucchar;
  }

  inline  size_t  utf8encode( char* output, size_t  cchout, widechar chnext )
  {
    char* outorg = output;
    char*           outend = output + cchout;

    if ( (chnext & ~0x007f) == 0 )
    {
      if ( output < outend )  *output++ = (char)(unsigned char)chnext;
        else return -1;
    }
      else
    if ( (chnext & ~0x07ff) == 0 )
    {
      if ( output >= outend - 1 )
        return -1;
      *output++ = (char)(0xC0 | (unsigned char)(chnext >> 6));
      *output++ = (char)(0x80 | (unsigned char)(chnext & 0x3f));
    }
      else
    {
      if ( output >= outend - 2 )
        return -1;
      *output++ = (char)(0xE0 | (unsigned char)(chnext >> 12));
      *output++ = (char)(0x80 | (unsigned char)((chnext >> 6) & 0x3F));
      *output++ = (char)(0x80 | (unsigned char)(chnext & 0x3F));
    }
    if ( output < outend )
      *output = '\0';
    return output - outorg;
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

  inline  size_t  utf8decode( widechar* output, size_t  maxlen, const char* pszstr, size_t  cchstr = (unsigned)-1 )
  {
    widechar*   outorg = output;
    widechar*   outend = output + maxlen;
    const char* pszend;

    if ( cchstr != (size_t)-1 ) pszend = pszstr + cchstr;
      else for ( pszend = pszstr; *pszend != 0; ++pszend ) (void)NULL;

    while ( pszstr < pszend )
    {
      widechar  ucchar;
      size_t    nchars;

    // on non-utf strings, return -1 as non-utf string error
      if ( (nchars = utf8cbchar( pszstr, pszend )) == 0 )
      {
        ++pszstr;
          continue;
      }
      if ( (ucchar = utf8decode( pszstr, nchars )) == (widechar)-1 )
        continue;

      if ( output < outend )
        *output++ = ucchar;

      pszstr += nchars;
    }
    if ( output < outend )
      *output = 0;
    return output - outorg;
  }

  inline  size_t  utf8encode( char* output, size_t  cchout, const widechar* pwsstr, size_t  cchstr = (size_t)-1 )
  {
    char*           outorg = output;
    char*           outend = output + cchout;
    const widechar* pwsend;

    if ( cchstr != (size_t)-1 ) pwsend = pwsstr + cchstr;
      else for ( pwsend = pwsstr; *pwsend != 0; ++pwsend )  (void)NULL;

    while ( pwsstr < pwsend )
    {
      widechar  chnext = *pwsstr++;
      size_t    cchenc;

      if ( (cchenc = utf8encode( output, cchout, *pwsstr++ )) <= 0 )  return cchenc;
        else  output += cchenc;
    }
    if ( output < outend )
      *output = '\0';
    return output - outorg;
  }

  template <class __cvt__, class O, class S>
  inline  size_t  encode( O*  output, size_t  cchout, const S*  source, size_t srclen = (size_t)-1 )
  {
    const S*  srcend;
    O*        outend;

    if ( srclen == (size_t)-1 )
      for ( srclen = 0; source[srclen] != 0; ++srclen ) (void)NULL;

    if ( srclen > cchout )
      return (size_t)-1;
 
    for ( outend = output + cchout, srcend = source + srclen; source < srcend; )
      *output++ = (O)__cvt__::translate( *source++ );

    if ( output < outend )
      *output = 0;

    return srclen;
  }

  struct __cvt_null__
  {
    static const widechar  translate ( unsigned c )
      {  return c;  }
  };

  template <const widechar table[], class __cvt__>
  struct __cvt_char__
  {
    static const widechar  translate( unsigned c )
      {  return table[__cvt__::translate( c )];  }
  };

  template <class __cvt__>
  struct __utf_1251__
  {
    static const widechar translate( unsigned c )
      {  return xlatUtf16ToWin[__cvt__::translate( c ) >> 8][__cvt__::translate( c & 0x00ff )];  }
  };

  inline  size_t  strtolower( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
    {  return encode<__cvt_char__<xlatUtf16Lower, __cvt_null__> >( o, l, s, u );  }

  inline  size_t  strtoupper( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
    {  return encode<__cvt_char__<xlatUtf16Upper, __cvt_null__> >( o, l, s, u );  }

  template <const widechar xt[]>
  inline  size_t  __impl__strtocase__( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return encode<__cvt_char__<xlatWinToKoi,
                        __utf_1251__<
                        __cvt_char__<xt,
                        __cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatKoiToWin, __cvt_null__> > > > > >( o, l, s, u );
        case codepage_866:
          return encode<__cvt_char__<xlatWinToDos,
                        __utf_1251__<
                        __cvt_char__<xt,
                        __cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatDosToWin, __cvt_null__> > > > > >( o, l, s, u );
        case codepage_iso:
          return encode<__cvt_char__<xlatWinToIso,
                        __utf_1251__<
                        __cvt_char__<xt,
                        __cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatIsoToWin, __cvt_null__> > > > > >( o, l, s, u );
        case codepage_mac:
          return encode<__cvt_char__<xlatWinToMac,
                        __utf_1251__<
                        __cvt_char__<xt,
                        __cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatMacToWin, __cvt_null__> > > > > >( o, l, s, u );
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
                c = __cvt_char__<xt, __cvt_null__>::translate( utf8decode( s, n ) );
                  s += n - 1;
                if ( (n = utf8encode( o, l, c )) < 0 )
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
          return encode<__utf_1251__<
                        __cvt_char__<xt,
                        __cvt_char__<xlatWinToUtf16, __cvt_null__> > > >( o, l, s, u );
      }
    }

  inline  size_t  strtolower( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      return __impl__strtocase__<xlatUtf16Lower>( codepage, o, l, s, u );
    }

  inline  size_t  strtoupper( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      return __impl__strtocase__<xlatUtf16Upper>( codepage, o, l, s, u );
    }

  inline  size_t  mbcstowide( unsigned codepage, widechar* o, size_t l, const char* s, size_t u = (size_t)-1 )
    {
      switch ( codepage )
      {
        case codepage_koi8:
          return encode<__cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatKoiToWin,
                        __cvt_null__> > >( o, l, s, u );
        case codepage_866:
          return encode<__cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatDosToWin,
                        __cvt_null__> > >( o, l, s, u );
        case codepage_iso:
          return encode<__cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatIsoToWin,
                        __cvt_null__> > >( o, l, s, u );
        case codepage_mac:
          return encode<__cvt_char__<xlatWinToUtf16,
                        __cvt_char__<xlatMacToWin,
                        __cvt_null__> > >( o, l, s, u );
        case codepage_utf8:
          return utf8decode( o, l, s, u );
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return encode<__cvt_char__<xlatWinToUtf16,
                        __cvt_null__> >( o, l, s, u );
      }
    }

  inline  size_t  widetombcs( unsigned codepage, char* o, size_t l, const widechar* s, size_t u = (size_t)-1 )
  {
      switch ( codepage )
      {
        case codepage_koi8:
          return encode<__cvt_char__<xlatWinToKoi,
                        __utf_1251__<
                        __cvt_null__> > >( o, l, s, u );
        case codepage_866:
          return encode<__cvt_char__<xlatWinToDos,
                        __utf_1251__<
                        __cvt_null__> > >( o, l, s, u );
        case codepage_iso:
          return encode<__cvt_char__<xlatWinToIso,
                        __utf_1251__<
                        __cvt_null__> > >( o, l, s, u );
        case codepage_mac:
          return encode<__cvt_char__<xlatWinToMac,
                        __utf_1251__<
                        __cvt_null__> > >( o, l, s, u );
        case codepage_utf8:
          return utf8encode( o, l, s, u );
        case codepage_1251:
        case codepage_1252:
        case codepage_1254:
        default:
          return encode<__utf_1251__<__cvt_null__> >( o, l, s, u );
      }
  }

} // codepages namespace

# endif  // __codes_h__
