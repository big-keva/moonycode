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
# include "../codes.h"
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>
# include <stdio.h>

using namespace codepages;

/*
  size_t    utf8cbchar( const char* ptrtop, const char* ptrend )
  widechar  utf8decode( const char* ptrtop, size_t chsize )
  size_t    utf8encode( char* output, size_t  cchout, widechar chnext )
  bool      utf8detect( const char* pszstr, size_t  cchstr = (size_t)-1 )
  size_t    utf8strlen( const char* pszstr, size_t  cchstr = (size_t)-1 )
  size_t    utf8decode( widechar* output, size_t  maxlen, const char* pszstr, size_t  cchstr = (unsigned)-1 )
  size_t    utf8encode( char* output, size_t  cchout, const widechar* pwsstr, size_t  cchstr = (size_t)-1 )
  size_t    strtolower( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
  size_t    strtoupper( widechar* o, size_t l, const widechar* s, size_t  u = (size_t)-1 )
  size_t    strtolower( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
  size_t    strtoupper( unsigned codepage, char* o, size_t l, const char* s, size_t u = (size_t)-1 )
  size_t    mbcstowide( unsigned codepage, widechar* o, size_t l, const char* s, size_t u = (size_t)-1 )
  size_t    widetombcs( unsigned codepage, char* o, size_t l, const widechar* s, size_t u = (size_t)-1 )
*/

template <class C>
C*  GetContent( const char* szname )
{
  FILE* lpfile;
  long  fisize;
  C*    loaded;

  if ( (lpfile = fopen( szname, "rb" )) == NULL )
    return NULL;

  fseek( lpfile, 0, SEEK_END );
    fisize = ftell( lpfile );
  fseek( lpfile, 0, SEEK_SET );

  if ( (loaded = (C*)malloc( fisize + sizeof(C) )) == NULL )
    return NULL;

  if ( fread( loaded, 1, fisize, lpfile ) != fisize )
  {
    fclose( lpfile );
    free( loaded );
    return NULL;
  }
  loaded[fisize / sizeof(C)] = (C)0;
    fclose( lpfile );
  return loaded;
}

template <class C>
inline  size_t  w_strlen( const C* p )
{
  const C* s;

  for ( s = p; *p != 0; ++p ) (void)NULL;
  return p - s;
}

template <class C>
inline  int     w_strcmp( const C* s, const C* m )
{
  int rc;

  while ( (rc = *s++ - *m++) == 0 && *s != (C)0 )
    (void)NULL;
  return rc;
}

int   _error( int code, const char* msg, ... )
{
  va_list argset;

  va_start( argset, msg );
    vfprintf( stderr, msg, argset );
     fprintf( stderr, "\n" );
  va_end( argset );
  return code;
}

int   main()
{
  widechar* wcsstr;
  widechar* wcs_lo;
  widechar* wcs_up;
  char*     utfstr;
  char*     winstr;
  char*     dosstr;
  char*     koistr;
  char*     isostr;
  char*     macstr;
  char*     tmpbuf;
  size_t    cchwcs;
  size_t    cchtmp;
  size_t    cchres;

  struct
  {
    const char*     szname;
    const unsigned  pageid;
    char*&          thestr;
  } codepageList[] =
  {
    { "1251", codepage_1251,  winstr },
    { "1252", codepage_1252,  winstr },
    { "1254", codepage_1254,  winstr },
    { "KOI8", codepage_koi8,  koistr },
    { "866",  codepage_866,   dosstr },
    { "ISO",  codepage_iso,   isostr },
    { "mac",  codepage_mac,   macstr },
    { "utf8", codepage_utf8,  utfstr }
  };

// load source data
  if ( (wcsstr = GetContent<widechar>( "test.utf16"     )) == NULL
    || (wcs_lo = GetContent<widechar>( "test.utf16.lo"  )) == NULL
    || (wcs_up = GetContent<widechar>( "test.utf16.up"  )) == NULL
    || (utfstr = GetContent<char>    ( "test.utf8"      )) == NULL
    || (winstr = GetContent<char>    ( "test.win"       )) == NULL
    || (dosstr = GetContent<char>    ( "test.dos"       )) == NULL
    || (koistr = GetContent<char>    ( "test.koi8"      )) == NULL
    || (isostr = GetContent<char>    ( "test.iso"       )) == NULL
    || (macstr = GetContent<char>    ( "test.mac"       )) == NULL )
  return _error( ENOENT, "Could not load source test data!" );

  if ( (tmpbuf = (char*)malloc( cchtmp = ((cchwcs = w_strlen( wcsstr )) + 1) * 6 )) == NULL )
    return ENOMEM;

// check utf16->utf8 in different modes
  if ( (cchres = utf8encode( tmpbuf, cchtmp, wcsstr, cchwcs )) != w_strlen( utfstr ) )
    return _error( EINVAL, "utf8encode: invalid return code!" );
  if ( tmpbuf[cchres] != 0 )
    return _error( EINVAL, "utf8encode: no final zero!" );
  if ( strcmp( tmpbuf, utfstr ) != 0 )
    return _error( EINVAL, "utf8encode: strcmp do not match!" );

  if ( (cchres = utf8encode( tmpbuf, cchtmp, wcsstr )) != w_strlen( utfstr ) )
    return _error( EINVAL, "utf8encode: invalid return code!" );
  if ( tmpbuf[cchres] != 0 )
    return _error( EINVAL, "utf8encode: no final zero!" );
  if ( strcmp( tmpbuf, utfstr ) != 0 )
    return _error( EINVAL, "utf8encode: strcmp do not match!" );

// check utf8strlen
  if ( utf8strlen( utfstr ) != w_strlen( wcsstr ) )
    return _error( EINVAL, "utf8strlen: string lengths do not match!" );

// check utf8->utf16
  if ( (cchres = utf8decode( (widechar*)tmpbuf, cchtmp / sizeof(widechar), utfstr )) != cchwcs )
    return _error( EINVAL, "utf8decode: invalid return code!" );
  if ( ((widechar*)tmpbuf)[cchres] != 0 )
    return _error( EINVAL, "utf8decode: no final zero!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcsstr ) != 0 )
    return _error( EINVAL, "utf8decode: string do not match!" );

// check strlower/strupper
  if ( (cchres = strtolower( (widechar*)tmpbuf, cchtmp / sizeof(widechar), wcsstr )) != cchwcs )
    return _error( EINVAL, "strtolower: the number of characters do not match!" );
  if ( ((widechar*)tmpbuf)[cchres] != 0 )
    return _error( EINVAL, "strtolower: no final zero!" );

  if ( w_strcmp( (widechar*)tmpbuf, wcs_lo ) != 0 )
    return _error( EINVAL, "strtolower: strcmp do not match!" );

  if ( (cchres = strtoupper( (widechar*)tmpbuf, cchtmp / sizeof(widechar), wcsstr )) != cchwcs )
    return _error( EINVAL, "strtoupper: the number of characters do not match!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcs_up ) != 0 )
    return _error( EINVAL, "strtupper: strcmp do not match!" );

// widechar to mbcs
  if ( (cchres = widetombcs( codepage_1251, tmpbuf, cchtmp, wcsstr )) != cchwcs )
    return _error( EINVAL, "widetombcs: invalid character count!" );
  if ( strcmp( tmpbuf, winstr ) != 0 )
    return _error( EINVAL, "widetombcs: 1251 codepage error!" );

  if ( (cchres = widetombcs( codepage_866, tmpbuf, cchtmp, wcsstr )) != cchwcs )
    return _error( EINVAL, "widetombcs: invalid character count!" );
  if ( strcmp( tmpbuf, dosstr ) != 0 )
    return _error( EINVAL, "widetombcs: 866 codepage error!" );

  if ( (cchres = widetombcs( codepage_koi8, tmpbuf, cchtmp, wcsstr )) != cchwcs )
    return _error( EINVAL, "widetombcs: invalid character count!" );
  if ( strcmp( tmpbuf, koistr ) != 0 )
    return _error( EINVAL, "widetombcs: koi8 codepage error!" );

  if ( (cchres = widetombcs( codepage_iso, tmpbuf, cchtmp, wcsstr )) != cchwcs )
    return _error( EINVAL, "widetombcs: invalid character count!" );
  if ( strcmp( tmpbuf, isostr ) != 0 )
    return _error( EINVAL, "widetombcs: iso codepage error!" );

  if ( (cchres = widetombcs( codepage_mac, tmpbuf, cchtmp, wcsstr )) != cchwcs )
    return _error( EINVAL, "widetombcs: invalid character count!" );
  if ( strcmp( tmpbuf, macstr ) != 0 )
    return _error( EINVAL, "widetombcs: mac codepage error!" );

// mbcs to widechar
  if ( (cchres = mbcstowide( codepage_1251, (widechar*)tmpbuf, cchtmp / sizeof(widechar), winstr )) != cchwcs )
    return _error( EINVAL, "mbcstowide: invalid character count!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcsstr ) != 0 )
    return _error( EINVAL, "widetombcs: 1251 codepage error!" );

  if ( (cchres = mbcstowide( codepage_866, (widechar*)tmpbuf, cchtmp, dosstr )) != cchwcs )
    return _error( EINVAL, "mbcstowide: invalid character count!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcsstr ) != 0 )
    return _error( EINVAL, "mbcstowide: 866 codepage error!" );

  if ( (cchres = mbcstowide( codepage_koi8, (widechar*)tmpbuf, cchtmp, koistr )) != cchwcs )
    return _error( EINVAL, "mbcstowide: invalid character count!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcsstr ) != 0 )
    return _error( EINVAL, "mbcstowide: 866 codepage error!" );

  if ( (cchres = mbcstowide( codepage_iso, (widechar*)tmpbuf, cchtmp, isostr )) != cchwcs )
    return _error( EINVAL, "mbcstowide: invalid character count!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcsstr ) != 0 )
    return _error( EINVAL, "mbcstowide: 866 codepage error!" );

  if ( (cchres = mbcstowide( codepage_mac, (widechar*)tmpbuf, cchtmp, macstr )) != cchwcs )
    return _error( EINVAL, "mbcstowide: invalid character count!" );
  if ( w_strcmp( (widechar*)tmpbuf, wcsstr ) != 0 )
    return _error( EINVAL, "mbcstowide: 866 codepage error!" );

// codepage transformations
  for ( int i = 0; i < sizeof(codepageList) / sizeof(codepageList[0]); ++i )
    for ( int j = 0; j < sizeof(codepageList) / sizeof(codepageList[0]); ++j )
    {
      if ( (cchres = mbcstombcs( codepageList[i].pageid, tmpbuf, cchtmp, codepageList[j].pageid, codepageList[j].thestr )) == (size_t)-1 )
        return _error( EINVAL, "mbcstombcs: length error!" );
      if ( strcmp( tmpbuf, codepageList[i].thestr ) != 0 )
        return _error( EINVAL, "mbcstombcs: %s <- %s string error!", codepageList[i].szname, codepageList[j].szname );
    }

  return 0;
}
