/*
 *	ASCII to WAV file converter.
 *
 *	Copyright:Les Hatton, CISM, Kingston University, London.
 *			l.hatton@kingston.ac.uk, 2009.
 *			Released under the Gnu Public Licence.
 *
 *	Revision:	$Revision: 1.3 $
 *	Date:	$Date: 2009/02/16 11:22:35 $
 *
 *	Notes:	ISO C 9899:1990 conformant requiring getopt().
 *			tab stop = 5.
 *
 *	Make:	Linux> gcc -o ascii2wav ascii2wav.c
 *
 *	Use:		ascii2wav -i input.ascii 	! [R] numbers one per line
 *					-o output.wav		! [R] output 16-bit .wav file
 *					-r 				! [O] (44100) sample rate Hz
 *					-e 				! [O] (0) 1=big-endian, 0=little.
 *
 *	[R] denotes Required and [O] denotes optional with the default.
 */
/*------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
/*------------------------------------------------------------------
 *	We explicitly include the API to optarg to avoid dredging in any
 *	other weird stuff, (usually in unistd.h).
 */
int	getopt( int argc, char * const argv[], const char *optstring );
extern	char	*optarg;
extern	int	optind, opterr, optopt;
/*------------------------------------------------------------------*/
/*
 *	Endian stuff.
 */
#define	LITTLE		0
#define	BIG			1
/*
 *	Data structures to do byte reversals as clean of the underlying
 *	machine as possible.  You might need to change the typedefs to
 *	provide type of the appropriate size and signedness.
 */
typedef	unsigned long	uint32_t;
typedef	unsigned short	uint16_t;
typedef	signed long	sint32_t;
typedef	signed short	sint16_t;

static	char			sb;

static	union	tag_u32_swap
{
	char		bytes[4];
	uint32_t	word;
}	u_u32;

static	union	tag_u16_swap
{
	char		bytes[2];
	uint16_t	word;
}	u_u16;

static	union	tag_s16_swap
{
	char		bytes[2];
	sint16_t	word;
}	u_s16;
/*
 *	Three macros to do the little-endian conversion of unsigned 16
 *	and 32 bit numbers.  We separate signed and unsigned 16 bit numbers
 *	to avoid implementation-defined conversions.
 */
#define	SWAPBYTE(b1,b2)	sb=b1; b1=b2; b2=sb;
/*
 *	Take note here.  Note that half words are also reversed in
 *	full words hence the scheme for 32 bits below.
 */
#define	REVERSE_U32(w1)	u_u32.word=w1; \
					 	SWAPBYTE( u_u32.bytes[0], u_u32.bytes[1]) \
					 	SWAPBYTE( u_u32.bytes[2], u_u32.bytes[3]) \
					 	w1=u_u32.word;
/*
 *	Conventional byte swapping a 16 bit word.
 */
#define	REVERSE_U16(w1)	u_u16.word=w1; \
					 	SWAPBYTE( u_u16.bytes[0], u_u16.bytes[1]) \
					 	w1=u_u16.word;
#define	REVERSE_S16(w1)	u_s16.word=w1; \
					 	SWAPBYTE( u_s16.bytes[0], u_s16.bytes[1]) \
					 	w1=u_s16.word;
/*------------------------------------------------------------------*/
/*
 *	Canonical WAV file header offsets.
 *	(e.g. http://www.ringthis.com/dev/wave_format.htm)
 */
static	struct	tag_wave_header
{
	char		riff[4];
	uint32_t	filesize;
	char		wave[4];
	char		fmt[4];
	uint32_t	fmtsize;
	uint16_t	fmttag;
	uint16_t	nchan;
	uint32_t	sr;
	uint32_t	byte_sec;
	uint16_t	block;
	uint16_t	bits;
	char		data[4];
	uint32_t	datasize;
}	wav_header;

static	sint16_t		*numbers = NULL;
static	sint32_t		nsamples = 0;
static	sint32_t		count = 0;
static	sint32_t 		headersize = sizeof( wav_header );
/*------------------------------------------------------------------*/

int	main( int argc, char *argv[] )
{
	int		opt;
	int		titsup	= 0;
	char		inputfile[BUFSIZ];
	char		outputfile[BUFSIZ];
	char		buffer[BUFSIZ];
	long		sr		= 44100;
	int		endian	= LITTLE;

	FILE		*fin		= NULL;
	FILE		*fout	= NULL;

	double	value	= 0.0;
	double	maxvalue	= - DBL_MAX;
	double	minvalue	= + DBL_MAX;
	double	scale	= 1.0;

	int		have_i	= 0;
	int		have_o	= 0;
/*
 *	Some sanity checks.
 */
	if ( sizeof( uint16_t ) != 2 )	titsup	= 1;
	if ( sizeof( sint16_t ) != 2 )	titsup	= 1;
	if ( sizeof( uint32_t ) != 4 )	titsup	= 1;
	if ( sizeof( sint32_t ) != 4 )	titsup	= 1;

	if ( titsup == 1 )
	{
/*
 *		Fundamental assumption of type sizes is not true.
 */
		fprintf(stderr,
			"shorts not 2 byte OR longs not 4 byte.\n");
		exit(EXIT_FAILURE);
	}

	if ( argc == 1 )
	{
		fprintf(stderr,"Usage:\n"
					"ascii2wav\n"
					"  -i inputasciifile\n"
					"       File of C format numbers, 1 per line.\n"
					"       Lines beginning with # are skipped.\n"
					"  -o outputwavfile\n"
					"       Output uncompressed .wav file,\n"
					"       16 bit canonical format.\n"
					"  -e endian [1=big|0=little, default 0]\n"
					"       Endian nature of this machine.  Windows\n"
					"       and Linux Intel machines are little.\n"
					"  -r samplerate [number, default 44100]\n");
		exit(EXIT_FAILURE);
	}
/*
 *	Check the arguments.
 *	====================
 */
	while( (opt = getopt(argc, argv, "i:o:r:")) != -1 )
	{
		switch(opt)
		{
			case 'i':
				have_i	= 1;

				if ( strlen(optarg) >= BUFSIZ )
				{
					fprintf(stderr,"inputasciifile name too long\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					strcpy( inputfile, optarg );
				}
				break;

			case 'o':
				have_o	= 1;

				if ( strlen(optarg) >= BUFSIZ )
				{
					fprintf(stderr,"outputwavfile name too long\n");
					exit(EXIT_FAILURE);
				}
				else
				{
					strcpy( outputfile, optarg );
				}
				break;

			case 'e':
				endian	= atol( optarg );
				break;

			case 'r':
				sr		= atol( optarg );
				break;

			default:
				fprintf(stderr,"Usage: -i inputasciifile "
							"-o outputwavfile\n"
							"       -r samplerate [44100]\n");
				exit(EXIT_FAILURE);
		}
	}

	if ( ! (have_i && have_o) )
	{
		fprintf(stderr,"Usage: -i inputasciifile "
					"-o outputwavfile\n"
					"       -r samplerate [44100]\n");
		exit(EXIT_FAILURE);
	}
/*
 *	Try and open the files.
 */
	if ( (fin = fopen( inputfile, "r" )) == NULL )
	{
		fprintf(stderr,"Couldn't open %s for reading.\n",inputfile);
		exit(EXIT_FAILURE);
	}

	if ( (fout = fopen( outputfile, "wb+" )) == NULL )
	{
		fprintf(stderr,"Couldn't open %s for writing.\n",outputfile);
		exit(EXIT_FAILURE);
	}
/*
 *	Files are open and input arguments seem OK.
 *
 *	Read the input file to allocate resources.
 *	==========================================
 */
	while( fgets( buffer, BUFSIZ, fin ) )
	{
/*
 *		Skip comment.
 */
		if ( buffer[0] == '#' )	continue;
/*							=========			*/
		++nsamples;

		value	= strtod( buffer, NULL );

		if ( value < minvalue )		minvalue	= value;
		if ( value > maxvalue )		maxvalue	= value;
	}
/*
 *	Determine scaling factors to map from any real numbers to 16bit
 *	signed integer range -32768 - 32767.
 */
	if ( maxvalue + minvalue > 0.0 )
	{
/*
 *		Peak bigger than trough or all positive.
 */
		scale	= (32767.0 / maxvalue);
	}
	else
	{
/*
 *		Trough bigger than peak or all negative.
 */
		scale	= (-32768.0 / minvalue);
	}

	rewind( fin );
/*
 *	Prepare the header.
 *	===================
 */
	wav_header.riff[0]	= 'R';
	wav_header.riff[1]	= 'I';
	wav_header.riff[2]	= 'F';
	wav_header.riff[3]	= 'F';

	wav_header.wave[0]	= 'W';
	wav_header.wave[1]	= 'A';
	wav_header.wave[2]	= 'V';
	wav_header.wave[3]	= 'E';

	wav_header.fmt [0]	= 'f';
	wav_header.fmt [1]	= 'm';
	wav_header.fmt [2]	= 't';
	wav_header.fmt [3]	= ' ';

	wav_header.data[0]	= 'd';
	wav_header.data[1]	= 'a';
	wav_header.data[2]	= 't';
	wav_header.data[3]	= 'a';

	wav_header.fmtsize	= 0x00000010;	/* Length of the fmt data (16 bytes) */
	wav_header.fmttag	= 0x0001;		/* Format tag: 1 = PCM			*/
	wav_header.nchan	= 0x0001;		/* Mono						*/
	wav_header.sr		= sr;
	wav_header.bits	= 16;		/* For now 16 bits per sample		*/
	wav_header.block	= wav_header.nchan * (wav_header.bits/8);
	wav_header.byte_sec	= wav_header.sr * wav_header.block;
	wav_header.datasize	= nsamples;
	wav_header.filesize	= (headersize - 8) + (nsamples * wav_header.bits);
/*
 *	Before writing we must now make all the numeric fields little-endian.
 */
	if ( endian == BIG )
	{
		REVERSE_U32( wav_header.filesize );
		REVERSE_U32( wav_header.fmtsize );
		REVERSE_U16( wav_header.fmttag );
		REVERSE_U16( wav_header.nchan );
		REVERSE_U32( wav_header.sr );
		REVERSE_U32( wav_header.byte_sec );
		REVERSE_U16( wav_header.block );
		REVERSE_U16( wav_header.bits );
		REVERSE_U32( wav_header.datasize );
	}

	fwrite( &wav_header, 1, headersize, fout );

	numbers	= malloc( nsamples * sizeof( sint16_t ) );
/*
 *	Reread the input file, writing the output file.
 *	===============================================
 */
	count	= 0;
	while( fgets( buffer, BUFSIZ, fin ) )
	{
/*
 *		Skip comment.
 */
		if ( buffer[0] == '#' )	continue;
/*							=========			*/

		value	= strtod( buffer, NULL );
/*
 *		Scale the number.
 */
		numbers[count]	= (scale * value);	

		if ( endian == BIG )
		{
/*
 *			Make it little-endian.
 */
			REVERSE_U16( numbers[count] );
		}

		count++;
	}

	fwrite( numbers, sizeof( sint16_t ), count, fout );

	fclose( fout );
	fclose( fin );

	free( numbers );

	exit(EXIT_SUCCESS);
}
