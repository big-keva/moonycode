# include "../chartype.h"
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <errno.h>

unsigned char matrix[0x10000];

void  DumpCharType( FILE* output, const char* sshift )
{
  for ( auto i = 0; i < 0x10000 / 16; ++i )
  {
    const char* prefix = sshift;
    const char* suffix = ",";

    for ( auto j = 0; j < 16; ++j, prefix = " ", suffix = i < (0x10000 / 16 - 1) || j < 15 ? "," : "" )
      fprintf( output, "%s0x%02x%s", prefix, matrix[i * 16 + j], suffix );

    fprintf( output, "\n" );
  }
}

int   LoadCharType( FILE* source )
{
  char  sznext[0x100];
  int   lineid = 1;

  memset( matrix, 0, sizeof(matrix) );

  for ( ; fgets( sznext, sizeof(sznext), source ) != nullptr; ++lineid )     // 0 == undefined
  {
    char*     strtop = sznext;
    char*     strend;
    unsigned  uvalue;
    unsigned  tvalue = 0;

  // trim string
    while ( *strtop != '\0' && (unsigned char)*strtop <= 0x20 )
      ++strtop;

    for ( strend = strtop; *strend++ != 0; )
      (void)NULL;

    while ( strend > strtop && (unsigned char)strend[-1] <= 0x20 )
      --strend;

    *strend = '\0';

  // get character code
    if ( (uvalue = strtoul( strtop, &strend, 0x10 )) == 0 && strend - strtop != 4 )
      return fprintf( stderr, "Invalid character code declaration '%s', line %d!\n", sznext, lineid ), EINVAL;

    if ( *(strtop = strend) != ';' )
      return fprintf( stderr, "';' expected in '%s' after character code, line %d!\n", sznext, lineid ), EINVAL;

    for ( ++strtop; *strtop != ';' && *strtop != '\0'; ++strtop )
      (void)NULL;

    if ( *strtop++ != ';' )
      return fprintf( stderr, "';' expected in '%s' after character category name, line %d!\n", sznext, lineid ), EINVAL;

  // check if category
    if ( (unsigned char)strtop[0] <= 0x20 || (unsigned char)strtop[1] <= 0x20 || strtop[2] != ';' )
      return fprintf( stderr, "Character category expected in '%s', line %d!\n", sznext, lineid ), EINVAL;

  // map two characters to the type value
    switch ( *strtop++ )
    {
      case 'C': tvalue |= codepages::cat_C;  break;
      case 'L': tvalue |= codepages::cat_L;  break;
      case 'M': tvalue |= codepages::cat_M;  break;
      case 'N': tvalue |= codepages::cat_N;  break;
      case 'P': tvalue |= codepages::cat_P;  break;
      case 'S': tvalue |= codepages::cat_S;  break;
      case 'Z': tvalue |= codepages::cat_Z;  break;
      default:
        return fprintf( stderr, "Invalid character type ->'%s', line %d\n", strtop - 1, lineid ), EINVAL;
    }
    switch ( *strtop++ )
    {
      case 'c': tvalue |= codepages::cat_c;  break;
      case 'd': tvalue |= codepages::cat_d;  break;
      case 'e': tvalue |= codepages::cat_e;  break;
      case 'f': tvalue |= codepages::cat_f;  break;
      case 'i': tvalue |= codepages::cat_i;  break;
      case 'k': tvalue |= codepages::cat_k;  break;
      case 'l': tvalue |= codepages::cat_l;  break;
      case 'm': tvalue |= codepages::cat_m;  break;
      case 'n': tvalue |= codepages::cat_n;  break;
      case 'o': tvalue |= codepages::cat_o;  break;
      case 'p': tvalue |= codepages::cat_p;  break;
      case 's': tvalue |= codepages::cat_s;  break;
      case 't': tvalue |= codepages::cat_t;  break;
      case 'u': tvalue |= codepages::cat_u;  break;
      default:
        return fprintf( stderr, "Invalid character type ->'%s', line %d\n", strtop - 2, lineid ), EINVAL;
    }

    if ( uvalue < 0x10000 )
      matrix[uvalue] = (unsigned char)tvalue;
  }

  return 0;
}

const char about[] = "mktypes - create the unicode character type map from it's text representation.\n"
                     "usage: mktypes source [varname=charType] [namespace]\n";

int   main( int argc, char* argv[] )
{
  const char* pszsrc = nullptr;
  const char* vaname = nullptr;
  const char* nspace = nullptr;
  FILE*       source;
  int         nerror;

// parse command line
  for ( auto i = 1; i < argc; ++i )
    if ( pszsrc == nullptr )  pszsrc = argv[i];
      else
    if ( vaname == nullptr )  vaname = argv[i];
      else
    if ( nspace == nullptr )  nspace = argv[i];

  if ( pszsrc == nullptr )  {  fprintf( stderr, about );  return -1;  }
    else  vaname = vaname != nullptr ? vaname : "charType";

// open source file
  if ( (source = fopen( pszsrc, "rt" )) == nullptr )
    {  fprintf( stderr, "Could not open file \'%s\'!\n", pszsrc );  return ENOENT;  }

  nerror = LoadCharType( source );
    fclose( source );

// check for load
  if ( nerror != 0 )
    return nerror;

// dump
  if ( nspace != nullptr )
    fprintf( stdout, "namespace %s\n"
                     "{\n", nspace );

  fprintf( stdout, "%sunsigned char %s[0x10000] =\n"
                   "%s{\n", nspace != nullptr ? "  " : "", vaname,
                            nspace != nullptr ? "  " : "" );

  DumpCharType( stdout, nspace != nullptr ? "    " : "  " );

  fprintf( stdout, "%s};\n", nspace != nullptr ? "  " : "" );

  if ( nspace != nullptr )
    fprintf( stdout, "}\n" );

  return nerror;
}
