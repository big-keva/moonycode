# include "../chartype.h"
# include <algorithm>
# include <vector>
# include <memory>
# include <cstring>
# include <cstdio>

/*
 * short ::Serialize()
 */
template <class O, class T>
O*      Serialize( O* o, const T& );

FILE*   Serialize( FILE* o, const void* p, size_t l )
{
  return fwrite( p, l, 1, o ) == l ? o : nullptr;
}

FILE*   Serialize( FILE* o, char c )          {  return Serialize( o, &c, 1 );  }
FILE*   Serialize( FILE* o, unsigned char c ) {  return Serialize( o, &c, 1 );  }

size_t  GetIntLen( const unsigned& u )
{
  auto  size = 1;

  for ( auto uval = u; (uval & ~0x07f) != 0; uval >>= 7, ++size )
    (void)NULL;
  return size;
}

template <class T>
FILE* Serialize( FILE* o, const T& u )
{
  auto  uval = u;

  do
  {
    auto  uput = (unsigned char)(uval & 0x7f);
      uval >>= 7;

    if ( (o = Serialize( o, (unsigned char)(uput | (uval != 0 ? 0x80 : 0x00) )) ) == nullptr )
      return nullptr;
  } while ( uval != 0 );
  return o;
}

namespace mtc
{

  template <class O>
  class DumpSource
  {
    O*        output = nullptr;
    unsigned  ushift = 2;
    unsigned  nbytes = 12;
    unsigned  stored = 0;

    template <class To>
    friend  auto  ::Serialize( DumpSource<To>*, const void*, size_t ) -> DumpSource<To>*;
    template <class To>
    friend  auto  MakeDumpSource( To* ) -> DumpSource<To>;

  protected:
    DumpSource( O* o ): output( o ) {}

  public:
    auto  set_count( unsigned u ) -> DumpSource&  {  return nbytes = u, *this;  }
    auto  set_shift( unsigned u ) -> DumpSource&  {  return ushift = u, *this;  }

  public:
    auto  begin() const -> DumpSource<O>*
    {  return (DumpSource<O>*)this;  }

  protected:
    bool  put( const void* p, size_t l )
    {
      char  next[6] = { '0', 'x' };

      for ( auto top = (const unsigned char*)p, end = top + l; top != end && output != nullptr; ++top, ++stored )
      {
        auto  nshift = ushift;

        if ( (next[2] = '0' + (*top >> 0x4)) > '9' )
          next[2] += 'a' - '9' - 1;
        if ( (next[3] = '0' + (*top & 0x0f)) > '9' )
          next[3] += 'a' - '9' - 1;

        if ( stored != 0 )
        {
          output = ::Serialize( output, ',' );

          if ( (stored % nbytes) == 0 ) output = ::Serialize( output, '\n' );
            else nshift = 1;
        }

        for ( unsigned i = 0; i != nshift && output != nullptr; ++i )
          output = ::Serialize( output, ' ' );

        output = ::Serialize( output, next, 4 );
      }
      return output;
    }
  };

  template <class To>
  auto  MakeDumpSource( To* to ) -> DumpSource<To>
  {
    return DumpSource<To>( to );
  }

}

template <class O>
auto  Serialize( mtc::DumpSource<O>* d, const void* p, size_t l ) -> mtc::DumpSource<O>*
{
  return d != nullptr && d->put( p, l ) ? d : nullptr;
}

inline  bool  is_letter( unsigned ch )  {  return (codepages::charType[ch] & 0xf0) == codepages::cat_L;  }

struct trigraph {  char  _[3];  };

template <class C, unsigned L>  class trigraphtree;

template <class C>
class trigraphtree<C, 0>
{
  template <class Ch, unsigned Cc>
  friend class trigraphtree;

  C         chnode;
  unsigned  ucount;

public:
  trigraphtree( C c ): chnode( c ), ucount( 0 ) {}

public:
  auto  add_trigraph( const C* ) -> unsigned&
    {  return ++ucount;  }
  void  put_to_array( std::vector<std::pair<trigraph, unsigned>>& to, trigraph& tg )
    {  to.emplace_back( std::make_pair( tg, ucount ) );  }

public:
  auto  GetBufLen() const -> size_t
    {  return ::GetIntLen( ucount );  }
  template <class O>
  auto  Serialize( O* o ) const -> O*
    {  return ::Serialize( o, ucount );  }
};


template <class C, unsigned L>
class trigraphtree: public std::vector<trigraphtree<C, L - 1>>
{
  template <class Ch, unsigned Cc>
  friend class trigraphtree;

  C         chnode = 0;

public:
  trigraphtree() = default;
  trigraphtree( C ch ): chnode( ch )  {}

public:
  auto  add_trigraph( const C* seq ) -> unsigned&
  {
    auto  it = this->begin();

    while ( it != this->end() && it->chnode < *seq )
      ++it;

    if ( it == this->end() || it->chnode != *seq )
      it = this->insert( it, trigraphtree<C, L - 1>( *seq ) );

    return it->add_trigraph( seq + 1 );
  }

public:     // serialization
  auto  GetBufLen() const -> size_t
  {
    auto  size = ::GetIntLen( this->size() );

    for ( auto& it: *this )
    {
      auto  itemSize = it.GetBufLen();

      size += 1 + itemSize + (L != 0 ? ::GetIntLen( itemSize ) : 0);
    }

    return size;
  }

  template <class O>
  auto  Serialize( O* o ) const
  {
    o = ::Serialize( o, this->size() );

    for ( auto& it: *this )
    {
      o = ::Serialize( o, &it.chnode, 1 );

      if ( L != 0 ) o = it.Serialize( ::Serialize( o, it.GetBufLen() ) );
        else o = it.Serialize( o );
    }
    return o;
  }

  void  put_to_array( std::vector<std::pair<trigraph, unsigned>>& to )
  {
    trigraph  tg;

    put_to_array( to, tg );
  }

protected:
  void  put_to_array( std::vector<std::pair<trigraph, unsigned>>& to, trigraph& tg )
  {
    for ( auto& it: *this )
    {
      tg._[3 - L] = it.chnode;

      it.put_to_array( to, tg );
    }
  }
};

template <class C>
void  add_sequence( trigraphtree<C, 3>& res, const C* seq, size_t len )
{
  for ( auto end = seq + len - 3; seq <= end; ++seq )
    res.add_trigraph( seq );
}

template <class C>
void  add_charbuff( trigraphtree<C, 3>& res, const C* seq, size_t len = (size_t)-1 )
{
  auto  end = seq + len;

  if ( len == (size_t)-1 )
    for ( end = seq; *end != 0; ++end ) (void)NULL;

  while ( seq != end )
  {
    auto  top = seq;

    while ( top != end && !is_letter( *top ) )
      ++top;

    for ( seq = top; seq != end && is_letter( *seq ); ++seq )
      (void)NULL;

    add_sequence( res, top, seq - top );
  }
}

int   main( int argc, char* argv[] )
{
  auto  buffer = std::unique_ptr<char>( new char[0x1000] );
  auto  cptree = trigraphtree<unsigned char, 3>();
  auto  tosort = std::vector<std::pair<trigraph, unsigned>>();
  auto  summed = 0.0;
  auto  infile = stdin;

  if ( argc > 1 && strcmp( argv[1], "-" ) != 0 )
    if ( (infile = fopen( argv[1], "rt" )) == nullptr )
      return fprintf( stderr, "could not open file '%s'\n", argv[1] );

  while ( fgets( buffer.get(), 0x1000 - 1, infile ) != nullptr )
    add_charbuff( cptree, (const unsigned char*)buffer.get() );

  cptree.put_to_array( tosort );

  std::sort( tosort.begin(), tosort.end(), [](
    const std::pair<trigraph, unsigned>& t1,
    const std::pair<trigraph, unsigned>& t2 ){  return t1.second > t2.second;  } );

  for ( auto& tr: tosort )
    summed += tr.second;

  summed = 0.98 * summed;

  cptree.clear();

  for ( auto it = tosort.begin(); it != tosort.end() && summed > 0 ; summed -= (it++)->second )
    cptree.add_trigraph( (const unsigned char*)it->first._ ) = it->second;

  cptree.Serialize( stdout );

  if ( infile != stdin )
    fclose( infile );

  return 0;
}
