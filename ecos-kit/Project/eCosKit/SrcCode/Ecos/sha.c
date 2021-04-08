#include <stdio.h>
#include <stdlib.h>
#include "Type.h"


/**
 * \brief          SHA-1 context structure
 */
typedef struct
{
    UINT32 total[2];     /*!< number of bytes processed  */
    UINT32 state[5];     /*!< intermediate digest state  */
    UINT8  buffer[64];   /*!< data block being processed */
}
sha1_context;


#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    UINT32 *ptr32 = (UINT32*)((b)+(i));                 \
    (n) = __builtin_bswap32(*ptr32);                    \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    UINT32 *ptr32 = (UINT32*)((b)+(i));                 \
    ptr32[0] = __builtin_bswap32( (n) );                \
}
#endif

//SHA1 C model
/*
 * SHA-1 context setup
 */
void sha1_starts(sha1_context *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
}


void sha1_process(sha1_context *ctx, const UINT8 data[64])
{
    UINT32 temp, W[16], A, B, C, D, E;

    GET_ULONG_BE( W[ 0], data,  0 );
    GET_ULONG_BE( W[ 1], data,  4 );
    GET_ULONG_BE( W[ 2], data,  8 );
    GET_ULONG_BE( W[ 3], data, 12 );
    GET_ULONG_BE( W[ 4], data, 16 );
    GET_ULONG_BE( W[ 5], data, 20 );
    GET_ULONG_BE( W[ 6], data, 24 );
    GET_ULONG_BE( W[ 7], data, 28 );
    GET_ULONG_BE( W[ 8], data, 32 );
    GET_ULONG_BE( W[ 9], data, 36 );
    GET_ULONG_BE( W[10], data, 40 );
    GET_ULONG_BE( W[11], data, 44 );
    GET_ULONG_BE( W[12], data, 48 );
    GET_ULONG_BE( W[13], data, 52 );
    GET_ULONG_BE( W[14], data, 56 );
    GET_ULONG_BE( W[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R1(t)                                            \
(                                                       \
    temp = W[( t -  3 ) & 0x0F] ^ W[( t - 8 ) & 0x0F] ^ \
           W[( t - 14 ) & 0x0F] ^ W[  t       & 0x0F],  \
    ( W[t & 0x0F] = S(temp,1) )                         \
)

#define P1(a,b,c,d,e,x)                                  \
{                                                       \
    e += S(a,5) + F(b,c,d) + K + x; b = S(b,30);        \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5A827999

    P1( A, B, C, D, E, W[0]  );
    P1( E, A, B, C, D, W[1]  );
    P1( D, E, A, B, C, W[2]  );
    P1( C, D, E, A, B, W[3]  );
    P1( B, C, D, E, A, W[4]  );
    P1( A, B, C, D, E, W[5]  );
    P1( E, A, B, C, D, W[6]  );
    P1( D, E, A, B, C, W[7]  );
    P1( C, D, E, A, B, W[8]  );
    P1( B, C, D, E, A, W[9]  );
    P1( A, B, C, D, E, W[10] );
    P1( E, A, B, C, D, W[11] );
    P1( D, E, A, B, C, W[12] );
    P1( C, D, E, A, B, W[13] );
    P1( B, C, D, E, A, W[14] );
    P1( A, B, C, D, E, W[15] );
    P1( E, A, B, C, D, R1(16) );
    P1( D, E, A, B, C, R1(17) );
    P1( C, D, E, A, B, R1(18) );
    P1( B, C, D, E, A, R1(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    P1( A, B, C, D, E, R1(20) );
    P1( E, A, B, C, D, R1(21) );
    P1( D, E, A, B, C, R1(22) );
    P1( C, D, E, A, B, R1(23) );
    P1( B, C, D, E, A, R1(24) );
    P1( A, B, C, D, E, R1(25) );
    P1( E, A, B, C, D, R1(26) );
    P1( D, E, A, B, C, R1(27) );
    P1( C, D, E, A, B, R1(28) );
    P1( B, C, D, E, A, R1(29) );
    P1( A, B, C, D, E, R1(30) );
    P1( E, A, B, C, D, R1(31) );
    P1( D, E, A, B, C, R1(32) );
    P1( C, D, E, A, B, R1(33) );
    P1( B, C, D, E, A, R1(34) );
    P1( A, B, C, D, E, R1(35) );
    P1( E, A, B, C, D, R1(36) );
    P1( D, E, A, B, C, R1(37) );
    P1( C, D, E, A, B, R1(38) );
    P1( B, C, D, E, A, R1(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    P1( A, B, C, D, E, R1(40) );
    P1( E, A, B, C, D, R1(41) );
    P1( D, E, A, B, C, R1(42) );
    P1( C, D, E, A, B, R1(43) );
    P1( B, C, D, E, A, R1(44) );
    P1( A, B, C, D, E, R1(45) );
    P1( E, A, B, C, D, R1(46) );
    P1( D, E, A, B, C, R1(47) );
    P1( C, D, E, A, B, R1(48) );
    P1( B, C, D, E, A, R1(49) );
    P1( A, B, C, D, E, R1(50) );
    P1( E, A, B, C, D, R1(51) );
    P1( D, E, A, B, C, R1(52) );
    P1( C, D, E, A, B, R1(53) );
    P1( B, C, D, E, A, R1(54) );
    P1( A, B, C, D, E, R1(55) );
    P1( E, A, B, C, D, R1(56) );
    P1( D, E, A, B, C, R1(57) );
    P1( C, D, E, A, B, R1(58) );
    P1( B, C, D, E, A, R1(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    P1( A, B, C, D, E, R1(60) );
    P1( E, A, B, C, D, R1(61) );
    P1( D, E, A, B, C, R1(62) );
    P1( C, D, E, A, B, R1(63) );
    P1( B, C, D, E, A, R1(64) );
    P1( A, B, C, D, E, R1(65) );
    P1( E, A, B, C, D, R1(66) );
    P1( D, E, A, B, C, R1(67) );
    P1( C, D, E, A, B, R1(68) );
    P1( B, C, D, E, A, R1(69) );
    P1( A, B, C, D, E, R1(70) );
    P1( E, A, B, C, D, R1(71) );
    P1( D, E, A, B, C, R1(72) );
    P1( C, D, E, A, B, R1(73) );
    P1( B, C, D, E, A, R1(74) );
    P1( A, B, C, D, E, R1(75) );
    P1( E, A, B, C, D, R1(76) );
    P1( D, E, A, B, C, R1(77) );
    P1( C, D, E, A, B, R1(78) );
    P1( B, C, D, E, A, R1(79) );

#undef K
#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
}

/*
 * SHA-1 process buffer
 */
void sha1_update(sha1_context *ctx, const UINT8 *input, UINT32 ilen )
{
    UINT32 fill;
    UINT32 left;

    if( ilen == 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += (UINT32) ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (UINT32) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left), input, fill );
        sha1_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        sha1_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
        memcpy( (void *) (ctx->buffer + left), input, ilen );
}

static const UINT8 sha1_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-1 final digest
 */
void sha1_finish(sha1_context *ctx, UINT8 output[20] )
{
    UINT32 last, padn;
    UINT32 high, low;
    unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_ULONG_BE( high, msglen, 0 );
    PUT_ULONG_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha1_update( ctx, sha1_padding, padn );
    sha1_update( ctx, msglen, 8 );

    PUT_ULONG_BE( ctx->state[0], output,  0 );
    PUT_ULONG_BE( ctx->state[1], output,  4 );
    PUT_ULONG_BE( ctx->state[2], output,  8 );
    PUT_ULONG_BE( ctx->state[3], output, 12 );
    PUT_ULONG_BE( ctx->state[4], output, 16 );
}


/*
 * output = SHA-1( input buffer )
 */
void sha1( const UINT8 *input, UINT32 ilen, UINT8 output[20] )
{
    sha1_context ctx;

    memset( &ctx, 0, sizeof( sha1_context ) );
    sha1_starts( &ctx );
    sha1_update( &ctx, input, ilen );
    sha1_finish( &ctx, output );
}


