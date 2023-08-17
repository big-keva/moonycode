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

  template <unsigned N>
  unsigned  findtrig(
    const unsigned char* tree,
    const unsigned char* trig,
    const unsigned char* xlat )
  {
    for ( auto  size = *tree++; size-- > 0; )
    {
      auto  chtree = *tree++;
      auto  chnext = xlat[*trig];

      if ( chtree <= chnext )
      {
        unsigned  uvalue;

        if ( chtree < chnext )
        {
          tree = GetUint( tree, uvalue );
          tree += uvalue;
        }
          else
        return findtrig<N - 1>( GetUint( tree, uvalue ), trig + 1, xlat );
      }
        else
      break;
    }
    return 0;
  }

  template <>
  unsigned  findtrig<0>( const unsigned char* tree, const unsigned char*, const unsigned char* )
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
        auto  rate = findtrig<3>( rus_trigraph, (const unsigned char*)text, next.xlat );

        if ( rate != 0 )
        {
          next.stat += rate;
          next.meet += 1;
          foundTrig = true;
        }
        test += foundTrig ? 1 : 0;
      }
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
    return findtrig<3>( rus_trigraph, (const unsigned char*)trig, xlat );
  }

  auto  detect::trigraph( const std::string& trig, const unsigned char* xlat ) -> unsigned
  {
    return trig.length() >= 3 ? trigraph( trig.c_str(), xlat ) : false;
  }

  bool  detect::coverage( const char* text, size_t size, const unsigned char* xlat )
  {
    if ( text == nullptr )
      return false;

    if ( size == (size_t)-1 )
      for ( size = 0; text[size] != 0; ++size ) (void)NULL;

    for ( auto stop = text + size - 2; text < stop; ++text )
      if ( !findtrig<3>( rus_trigraph, (const unsigned char*)text, xlat ) )
        return false;

    return true;
  }

  bool  detect::coverage( const std::string& text, const unsigned char* xlat )
  {
    return coverage( text.c_str(), text.length(), xlat );
  }

}
