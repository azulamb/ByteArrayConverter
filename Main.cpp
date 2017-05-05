#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum
{
	NO_ERR = 0,
	ERR_NO_FILE,
	ERR_OPEN_READ,
	ERR_OPEN_WRITE,
};

char * OutputFile( const char *src )
{
	size_t len;
	char *path;

	len = strlen( src );

	if (len <= 0) { return nullptr; }

	while (0 < --len)
	{
		if (src[ len ] == '.') { break; }
	}
	if (len <= 0) { len = strlen( src ); }

	path = (char*)calloc( sizeof( char ), len + 3 );
	if (!path) { return nullptr; }
	path[ len + 1 ] = 'c';
	path[ len ] = '.';
	while (0 < len--) { path[ len ] = src[ len ]; }
	return path;
}

int Convert( const char *src, const char *dst )
{
	FILE *read, *write;
	size_t counter = 0;
	fpos_t size;
	int byte;

	if (fopen_s( &read, src, "rb" ) != 0) { return ERR_OPEN_READ; }
	if (fopen_s( &write, dst, "wb" ) != 0) { return ERR_OPEN_WRITE; }
	fseek( read, 0, SEEK_END );
	fgetpos( read, &size );
	fseek( read, 0, 0 );

	fprintf( write, "unsigned char data[ %lld ] = {\n", size );
	while (!feof( read ))
	{
		//fputc( fgetc( read ), write );
		byte = fgetc( read );
		if (byte < 0) { break; }

		if (counter % 16 == 0) { fprintf( write, "\t" ); }
		fprintf( write, "0x%02x,", byte );
		if (( ++counter ) % 16 == 0) { fprintf( write, "\n" ); }
	}
	fprintf( write, "%s};", counter % 16 == 0 ? "" : "\n" );

	fclose( write );
	fclose( read );

	return NO_ERR;
}

int main( int argc, const char **argv )
{
	char *dest;

	if (argc < 2) { return 0; }
	dest = OutputFile( argv[ 1 ] );

	Convert( argv[ 1 ], dest );

	free( dest );

	return 0;
}
