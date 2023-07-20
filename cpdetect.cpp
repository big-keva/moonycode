# include "moonycode/codes.h"
# include <algorithm>

namespace codepages
{
  extern unsigned char rus_trigraph[];

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
  unsigned  findtrig<1>( const unsigned char* tree, const unsigned char*, const unsigned char* )
  {
    unsigned  uvalue;

    return GetUint( tree, uvalue ), uvalue;
  }

  unsigned  findtrig( const char* trig, const unsigned char* xlat )
  {
    return findtrig<3>( rus_trigraph, (const unsigned char*)trig, xlat );
  }

  auto  detect( const char* text, size_t size ) -> unsigned
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

    for ( auto stop = text + size; text + 2 < stop; ++text )
    {
      bool  foundTrig = false;

      for ( auto& next: list )
      {
        auto  rate = findtrig( text, next.xlat );

        if ( rate != 0 )
        {
          next.stat += rate;
          next.meet += 1;
          foundTrig = true;
        }
        test += foundTrig ? 1 : 0;
      }
    }

    std::sort( std::begin( list ), std::end( list ) );

    if ( test != 0 && list[0].meet > test / 2.0 )
      return list[0].page;

    return 0;
  }

}
