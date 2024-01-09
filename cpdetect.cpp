# include "./codes.h"
# include <algorithm>

namespace codepages
{
  const unsigned char* GetUint( const unsigned char* src, unsigned& val )
  {
    unsigned  ushift = 0;

    for ( val = 0; ; ushift += 7 )
    {
      val |= ((*src & 0x7f) << ushift);

      if ( (*src++ & 0x80) == 0 )
        break;
    }

    return src;
  }

  template <class ... trigraph>
  unsigned  findtrig( const unsigned char*  tree, trigraph ... );

  template <class ... trigraph>
  unsigned  findtrig( const unsigned char*  tree, unsigned char next, trigraph... list )
  {
    for ( auto  size = *tree++; size-- > 0; )
    {
      auto  chtree = *tree++;

      if ( chtree <= next )
      {
        unsigned  uvalue;

        if ( chtree < next )
        {
          tree = GetUint( tree, uvalue );
          tree += uvalue;
        }
          else
        return findtrig( GetUint( tree, uvalue ), list... );
      }
        else
      break;
    }
    return 0;
  }

  template <>
  unsigned  findtrig( const unsigned char* tree )
  {
    unsigned  uvalue;

    return GetUint( tree, uvalue ), uvalue;
  }

  // detect methods

  auto  detect::codepage( const char* text, size_t size ) -> unsigned
  {
    struct  pagestat
    {
      unsigned              page;
      const unsigned char*  xlat;
      unsigned              stat = 0;
      unsigned              meet = 0;

    public:
      bool  operator < ( const pagestat& cp ) const
        {  return meet > cp.meet || (meet == cp.meet && stat > cp.stat);  }
    };

    pagestat  list[5] =
    {
      { codepage_1251, xlatOneToOne },
      { codepage_866,  xlatDosToWin },
      { codepage_iso,  xlatIsoToWin },
      { codepage_mac,  xlatMacToWin },
      { codepage_koi8, xlatKoiToWin }
    };

    unsigned  test = 0;

    if ( size == (size_t)-1 )
      for ( size = 0; text[size] != 0; ++size ) (void)NULL;

    // check for utf8
    if ( utf8::detect( text, size ) )
      return codepage_utf8;

    // build codepages stat
    for ( auto stop = text + size; text + 2 < stop; ++text )
    {
      bool  foundTrig = false;

      for ( auto& next: list )
      {
        auto  rate = findtrig( rus_trigraph,
          next.xlat[(unsigned char)text[0]],
          next.xlat[(unsigned char)text[1]],
          next.xlat[(unsigned char)text[2]]);

        if ( rate != 0 )
        {
          next.stat += rate;
          next.meet += 1;
          foundTrig = true;
        }
      }
      test += foundTrig ? 1 : 0;
    }

    // get best codepage
    std::sort( std::begin( list ), std::end( list ) );

    if ( test != 0 && list[0].meet > test / 2.0 )
      return list[0].page;

    return 0;
  }

  auto  detect::codepage( const std::string& s ) -> unsigned
  {
    return codepage( s.c_str(), s.length() );
  }

  auto  detect::trigraph( const char* trig, const unsigned char* xlat ) -> unsigned
  {
    return findtrig( rus_trigraph,
      xlat[(unsigned char)trig[0]],
      xlat[(unsigned char)trig[1]],
      xlat[(unsigned char)trig[2]] );
  }

  auto  detect::trigraph( const std::string& trig, const unsigned char* xlat ) -> unsigned
  {
    return trig.length() >= 3 ? trigraph( trig.c_str(), xlat ) : false;
  }

  auto  detect::trigraph( const widechar* trig ) -> unsigned int
  {
    return findtrig( rus_trigraph,
      (unsigned char)__impl__::utf_1251<>::translate( trig[0] ),
      (unsigned char)__impl__::utf_1251<>::translate( trig[1] ),
      (unsigned char)__impl__::utf_1251<>::translate( trig[2] ) );
  }

  bool  detect::coverage( const char* text, size_t size, const unsigned char* xlat )
  {
    if ( text == nullptr )
      return false;

    if ( size == (size_t)-1 )
      for ( size = 0; text[size] != 0; ++size ) (void)NULL;

    for ( auto stop = text + size - 2; text < stop; ++text )
      if ( !findtrig( rus_trigraph,
        xlat[(unsigned char)text[0]],
        xlat[(unsigned char)text[1]],
        xlat[(unsigned char)text[2]] ) )
      return false;

    return true;
  }

  bool  detect::coverage( const std::string& text, const unsigned char* xlat )
  {
    return coverage( text.c_str(), text.length(), xlat );
  }

  bool  detect::coverage( const widechar* text, size_t size )
  {
    if ( text == nullptr )
      return false;

    if ( size == (size_t)-1 )
      for ( size = 0; text[size] != 0; ++size ) (void)NULL;

    for ( auto stop = text + size - 2; text < stop; ++text )
      if ( !findtrig( rus_trigraph,
        (unsigned char)__impl__::utf_1251<>::translate( text[0] ),
        (unsigned char)__impl__::utf_1251<>::translate( text[1] ),
        (unsigned char)__impl__::utf_1251<>::translate( text[2] ) ) )
      return false;

    return true;
  }

}
