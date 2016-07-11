# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <errno.h>
# include <cassert>

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

  for ( memset( matrix, 0, sizeof(matrix) ); fgets( sznext, sizeof(sznext), source ) != nullptr;
    ++lineid )     // 0 == undefined
  {
    char*     strtop = sznext;
    char*     strend;
    unsigned  ulower;
    unsigned  uupper;
    char*     sztype;
    unsigned  cctype;
    unsigned  tvalue = 0;

  // trim string
    while ( *strtop != '\0' && (unsigned char)*strtop <= 0x20 )
      ++strtop;
    if ( *strtop == '#' )
      continue;
    for ( strend = strtop; *strend != '\0'; ++strend )  (void)NULL;
      while ( strend > strtop && (unsigned char)strend[-1] <= 0x20 )  *--strend = '\0';
    if ( *strtop == '\0' )
      continue;

  // get interval
    if ( (uupper = ulower = strtoul( strtop, &strend, 0x10 )) == 0 )
      continue;
    if ( *(strtop = strend) == '.' )
    {
      do ++strtop;
        while ( *strtop == '.' );
      if ( (uupper = strtoul( strtop, &strend, 0x10 )) == 0 )
        {  fprintf( stderr, "Invalid declaration '%s', line %d!\n", sznext, lineid );  continue;  }
    }

    if ( uupper >= 0xffff )
      uupper = 0xffff;
    if ( ulower >  0xffff )
      continue;

  // skip until the type string
    for ( strtop = strend; *strtop != '\0' && (unsigned char)*strtop <= 0x20; ++strtop )
      (void)NULL;

  // check for semicolon
    if ( *strtop++ != ';' )
      {  fprintf( stderr, "';' expected, line %d!\n", lineid );  continue;  }
    else
      while ( *strtop != '\0' && (unsigned char)*strtop <= 0x20 ) ++strtop;

  // get type string
    for ( sztype = strtop; *strtop != '\0' && (unsigned char)*strtop > 0x20; ++strtop )
      (void)NULL;
    if ( (cctype = (unsigned)(strtop - sztype)) == 0 )
      {  fprintf( stderr, "Invalid (zero size) type string, line %d!\n", lineid );  continue;  }
    else
      while ( *strtop != '\0' && (unsigned char)*strtop <= 0x20 ) *strtop++ = '\0';

  // check '#'
    if ( *strtop++ != '#' )
      {  fprintf( stderr, "'#' expected, line %d!\n", lineid );  continue;  }
    else
      while ( *strtop != '\0' && (unsigned char)*strtop <= 0x20 ) ++strtop;

  // map two characters to the type value
    switch ( *strtop++ )
    {
      case 'C': tvalue |= 0x0010;  break;
      case 'L': tvalue |= 0x0020;  break;
      case 'M': tvalue |= 0x0030;  break;
      case 'N': tvalue |= 0x0040;  break;
      case 'P': tvalue |= 0x0050;  break;
      case 'S': tvalue |= 0x0060;  break;
      case 'Z': tvalue |= 0x0070;  break;
      default:
        fprintf( stderr, "Invalid character type ->'%s', line %d\n", strtop - 1, lineid );
        continue;
    }
    switch ( *strtop++ )
    {
      case '&': tvalue |= 0x0001;  break;
      case 'c': tvalue |= 0x0002;  break;
      case 'd': tvalue |= 0x0003;  break;
      case 'e': tvalue |= 0x0004;  break;
      case 'f': tvalue |= 0x0005;  break;
      case 'i': tvalue |= 0x0006;  break;
      case 'k': tvalue |= 0x0007;  break;
      case 'l': tvalue |= 0x0008;  break;
      case 'm': tvalue |= 0x0009;  break;
      case 'n': tvalue |= 0x000a;  break;
      case 'o': tvalue |= 0x000b;  break;
      case 'p': tvalue |= 0x000c;  break;
      case 's': tvalue |= 0x000d;  break;
      default:
        fprintf( stderr, "Invalid character type ->'%s', line %d\n", strtop - 2, lineid );
        continue;
    }

    while ( ulower <= uupper )
      matrix[ulower++] = (unsigned char)tvalue;
  }

  return 0;
}

const char about[] = "umap - create the unicode character type map from it's text representation.\n"
                     "usage: umap source [varname=charType] [namespace]\n";

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
