/*
   BLAKE2 reference source code package - reference C implementations
   Copyright 2012, Samuel Neves <sneves@dei.uc.pt>.  You may use this under the
   terms of the CC0, the OpenSSL Licence, or the Apache Public License 2.0, at
   your option.  The terms of these licenses can be found at:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
   - OpenSSL license   : https://www.openssl.org/source/license.html
   - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0
   More information about the BLAKE2 hash function can be found at
   https://blake2.net.
*/

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "blake2s_ref.h"
#include "blake2s-impl_ref.h"

#define IV0 0x6A09E667UL
#define IV1 0xBB67AE85UL
#define IV2 0x3C6EF372UL
#define IV3 0xA54FF53AUL
#define IV4 0x510E527FUL
#define IV5 0x9B05688CUL
#define IV6 0x1F83D9ABUL
#define IV7 0x5BE0CD19UL

#define IV(r) IV ## r

#define Z00   0
#define Z01   1
#define Z02   2
#define Z03   3
#define Z04   4
#define Z05   5
#define Z06   6
#define Z07   7
#define Z08   8
#define Z09   9
#define Z0A   A
#define Z0B   B
#define Z0C   C
#define Z0D   D
#define Z0E   E
#define Z0F   F

#define Z10   E
#define Z11   A
#define Z12   4
#define Z13   8
#define Z14   9
#define Z15   F
#define Z16   D
#define Z17   6
#define Z18   1
#define Z19   C
#define Z1A   0
#define Z1B   2
#define Z1C   B
#define Z1D   7
#define Z1E   5
#define Z1F   3

#define Z20   B
#define Z21   8
#define Z22   C
#define Z23   0
#define Z24   5
#define Z25   2
#define Z26   F
#define Z27   D
#define Z28   A
#define Z29   E
#define Z2A   3
#define Z2B   6
#define Z2C   7
#define Z2D   1
#define Z2E   9
#define Z2F   4

#define Z30   7
#define Z31   9
#define Z32   3
#define Z33   1
#define Z34   D
#define Z35   C
#define Z36   B
#define Z37   E
#define Z38   2
#define Z39   6
#define Z3A   5
#define Z3B   A
#define Z3C   4
#define Z3D   0
#define Z3E   F
#define Z3F   8

#define Z40   9
#define Z41   0
#define Z42   5
#define Z43   7
#define Z44   2
#define Z45   4
#define Z46   A
#define Z47   F
#define Z48   E
#define Z49   1
#define Z4A   B
#define Z4B   C
#define Z4C   6
#define Z4D   8
#define Z4E   3
#define Z4F   D

#define Z50   2
#define Z51   C
#define Z52   6
#define Z53   A
#define Z54   0
#define Z55   B
#define Z56   8
#define Z57   3
#define Z58   4
#define Z59   D
#define Z5A   7
#define Z5B   5
#define Z5C   F
#define Z5D   E
#define Z5E   1
#define Z5F   9

#define Z60   C
#define Z61   5
#define Z62   1
#define Z63   F
#define Z64   E
#define Z65   D
#define Z66   4
#define Z67   A
#define Z68   0
#define Z69   7
#define Z6A   6
#define Z6B   3
#define Z6C   9
#define Z6D   2
#define Z6E   8
#define Z6F   B

#define Z70   D
#define Z71   B
#define Z72   7
#define Z73   E
#define Z74   C
#define Z75   1
#define Z76   3
#define Z77   9
#define Z78   5
#define Z79   0
#define Z7A   F
#define Z7B   4
#define Z7C   8
#define Z7D   6
#define Z7E   2
#define Z7F   A

#define Z80   6
#define Z81   F
#define Z82   E
#define Z83   9
#define Z84   B
#define Z85   3
#define Z86   0
#define Z87   8
#define Z88   C
#define Z89   2
#define Z8A   D
#define Z8B   7
#define Z8C   1
#define Z8D   4
#define Z8E   A
#define Z8F   5

#define Z90   A
#define Z91   2
#define Z92   8
#define Z93   4
#define Z94   7
#define Z95   6
#define Z96   1
#define Z97   5
#define Z98   F
#define Z99   B
#define Z9A   9
#define Z9B   E
#define Z9C   3
#define Z9D   C
#define Z9E   D
#define Z9F   0

#define Mx(r, i)    Mx_(Z ## r ## i)
#define Mx_(n)      Mx__(n)
#define Mx__(n)     M ## n

#define SWAP32(n) (rotate(n & 0x00FF00FF, 24U)|(rotate(n, 8U) & 0x00FF00FF)

#if NATIVE_LITTLE_ENDIAN
# define LOAD32(p) (*((int32_t*) p))
#else
# define LOAD32(p) load32(p)
#endif

static void blake2s_set_lastnode( blake2s_state *S )
{
  S->f[1] = (uint32_t)-1;
}

/* Some helper functions, not necessarily useful */
static int blake2s_is_lastblock( const blake2s_state *S )
{
  return S->f[0] != 0;
}

static void blake2s_set_lastblock( blake2s_state *S )
{
  if( S->last_node ) blake2s_set_lastnode( S );

  S->f[0] = (uint32_t)-1;
}

static void blake2s_increment_counter( blake2s_state *S, const uint32_t inc )
{
  S->t[0] += inc;
  S->t[1] += ( S->t[0] < inc );
}

static void blake2s_init0( blake2s_state *S )
{
  size_t i;
  memset( S, 0, sizeof( blake2s_state ) );

  S->h[0] = IV(0);
  S->h[1] = IV(1);
  S->h[2] = IV(2);
  S->h[3] = IV(3);
  S->h[4] = IV(4);
  S->h[5] = IV(5);
  S->h[6] = IV(6);
  S->h[7] = IV(7);
}

/* init2 xors IV with input parameter block */
int blake2s_init_param( blake2s_state *S, const blake2s_param *P )
{
  const unsigned char *p = ( const unsigned char * )( P );
  size_t i;

  blake2s_init0( S );

  /* IV XOR ParamBlock */
  for( i = 0; i < 8; ++i )
    S->h[i] ^= load32( &p[i * 4] );

  S->outlen = P->digest_length;
  return 0;
}


/* Sequential blake2s initialization */
int blake2s_init( blake2s_state *S, size_t outlen )
{
  blake2s_param P[1];

  /* Move interval verification here? */
  if ( ( !outlen ) || ( outlen > BLAKE2S_OUTBYTES ) ) return -1;

  P->digest_length = (uint8_t)outlen;
  P->key_length    = 0;
  P->fanout        = 1;
  P->depth         = 1;
  store32( &P->leaf_length, 0 );
  store32( &P->node_offset, 0 );
  store16( &P->xof_length, 0 );
  P->node_depth    = 0;
  P->inner_length  = 0;
  /* memset(P->reserved, 0, sizeof(P->reserved) ); */
  memset( P->salt,     0, sizeof( P->salt ) );
  memset( P->personal, 0, sizeof( P->personal ) );
  return blake2s_init_param( S, P );
}

int blake2s_init_key( blake2s_state *S, size_t outlen, const void *key, size_t keylen )
{
  blake2s_param P[1];

  if ( ( !outlen ) || ( outlen > BLAKE2S_OUTBYTES ) ) return -1;

  if ( !key || !keylen || keylen > BLAKE2S_KEYBYTES ) return -1;

  P->digest_length = (uint8_t)outlen;
  P->key_length    = (uint8_t)keylen;
  P->fanout        = 1;
  P->depth         = 1;
  store32( &P->leaf_length, 0 );
  store32( &P->node_offset, 0 );
  store16( &P->xof_length, 0 );
  P->node_depth    = 0;
  P->inner_length  = 0;
  /* memset(P->reserved, 0, sizeof(P->reserved) ); */
  memset( P->salt,     0, sizeof( P->salt ) );
  memset( P->personal, 0, sizeof( P->personal ) );

  if( blake2s_init_param( S, P ) < 0 ) return -1;

  {
    uint8_t block[BLAKE2S_BLOCKBYTES];
    memset( block, 0, BLAKE2S_BLOCKBYTES );
    memcpy( block, key, keylen );
    blake2s_update( S, block, BLAKE2S_BLOCKBYTES );
    secure_zero_memory( block, BLAKE2S_BLOCKBYTES ); /* Burn the key from stack */
  }
  return 0;
}

#define G(m0, m1, a,b,c,d)                      \
  do {                                      \
    a = a + b + m0; \
    d = rotr32(d ^ a, 16);                  \
    c = c + d;                              \
    b = rotr32(b ^ c, 12);                  \
    a = a + b + m1; \
    d = rotr32(d ^ a, 8);                   \
    c = c + d;                              \
    b = rotr32(b ^ c, 7);                   \
  } while(0)

#define ROUND(r)   do { \
    G(Mx(r, 0), Mx(r, 1), V0, V4, V8, VC); \
    G(Mx(r, 2), Mx(r, 3), V1, V5, V9, VD); \
    G(Mx(r, 4), Mx(r, 5), V2, V6, VA, VE); \
    G(Mx(r, 6), Mx(r, 7), V3, V7, VB, VF); \
    G(Mx(r, 8), Mx(r, 9), V0, V5, VA, VF); \
    G(Mx(r, A), Mx(r, B), V1, V6, VB, VC); \
    G(Mx(r, C), Mx(r, D), V2, V7, V8, VD); \
    G(Mx(r, E), Mx(r, F), V3, V4, V9, VE); \
  } while (0)

#define DO_COMPRESS(m0, m1, block) do { \
    M0 = m0; \
    M1 = m1; \
    M2 = LOAD32(block +   8-8); \
    M3 = LOAD32(block +  12-8); \
    M4 = LOAD32(block +  16-8); \
    M5 = LOAD32(block +  20-8); \
    M6 = LOAD32(block +  24-8); \
    M7 = LOAD32(block +  28-8); \
    M8 = LOAD32(block +  32-8); \
    M9 = LOAD32(block +  36-8); \
    MA = LOAD32(block +  40-8); \
    MB = LOAD32(block +  44-8); \
    MC = LOAD32(block +  48-8); \
    MD = LOAD32(block +  52-8); \
    ME = LOAD32(block +  56-8); \
    MF = LOAD32(block +  60-8); \
    V0 = H0; V1 = H1; V2 = H2; V3 = H3; \
    V4 = H4; V5 = H5; V6 = H6; V7 = H7; \
    V8 = IV0; V9 = IV1; VA = IV2; VB = IV3; \
    VC = T0 ^ IV4; VD = T1 ^ IV5; VE = F0 ^ IV6; VF = F1 ^ IV7; \
    ROUND(0); \
    ROUND(1); \
    ROUND(2); \
    ROUND(3); \
    ROUND(4); \
    ROUND(5); \
    ROUND(6); \
    ROUND(7); \
    ROUND(8); \
    ROUND(9); \
    H0 = H0 ^ V0 ^ V8;\
    H1 = H1 ^ V1 ^ V9;\
    H2 = H2 ^ V2 ^ VA;\
    H3 = H3 ^ V3 ^ VB;\
    H4 = H4 ^ V4 ^ VC;\
    H5 = H5 ^ V5 ^ VD;\
    H6 = H6 ^ V6 ^ VE;\
    H7 = H7 ^ V7 ^ VF;\
  } while (0)

#define DO_COMPRESS_SIMPLE(buf) DO_COMPRESS(LOAD32(buf+0), LOAD32(buf+4), buf+8)

static void blake2s_compress( blake2s_state *S, const uint8_t block[BLAKE2S_BLOCKBYTES] )
{
  #define H0 S->h[0]
  #define H1 S->h[1]
  #define H2 S->h[2]
  #define H3 S->h[3]
  #define H4 S->h[4]
  #define H5 S->h[5]
  #define H6 S->h[6]
  #define H7 S->h[7]
  #define T0 S->t[0]
  #define T1 S->t[1]
  #define F0 S->f[0]
  #define F1 S->f[1]

  uint32_t M0, M1, M2, M3, M4, M5, M6, M7;
  uint32_t M8, M9, MA, MB, MC, MD, ME, MF;
  uint32_t V0, V1, V2, V3, V4, V5, V6, V7;
  uint32_t V8, V9, VA, VB, VC, VD, VE, VF;

  DO_COMPRESS(LOAD32(block + 0), LOAD32(block + 4), block+8);

  #undef H0
  #undef H1
  #undef H2
  #undef H3
  #undef H4
  #undef H5
  #undef H6
  #undef H7
  #undef T0
  #undef T1
  #undef F0
  #undef F1
}

// #undef G
// #undef ROUND

int blake2s_update( blake2s_state *S, const void *pin, size_t inlen )
{
  const unsigned char * in = (const unsigned char *)pin;
  if( inlen > 0 )
  {
    size_t left = S->buflen;
    size_t fill = BLAKE2S_BLOCKBYTES - left;
    if( inlen > fill )
    {
      S->buflen = 0;
      memcpy( S->buf + left, in, fill ); /* Fill buffer */
      blake2s_increment_counter( S, BLAKE2S_BLOCKBYTES );
      blake2s_compress( S, S->buf ); /* Compress */
      in += fill; inlen -= fill;
      while(inlen > BLAKE2S_BLOCKBYTES) {
        blake2s_increment_counter(S, BLAKE2S_BLOCKBYTES);
        blake2s_compress( S, in );
        in += BLAKE2S_BLOCKBYTES;
        inlen -= BLAKE2S_BLOCKBYTES;
      }
    }
    memcpy( S->buf + S->buflen, in, inlen );
    S->buflen += inlen;
  }
  return 0;
}

int blake2s_final( blake2s_state *S, void *out, size_t outlen )
{
  uint8_t buffer[BLAKE2S_OUTBYTES] = {0};
  size_t i;

  if( out == NULL || outlen < S->outlen )
    return -1;

  if( blake2s_is_lastblock( S ) )
    return -1;

  blake2s_increment_counter( S, ( uint32_t )S->buflen );
  blake2s_set_lastblock( S );
  memset( S->buf + S->buflen, 0, BLAKE2S_BLOCKBYTES - S->buflen ); /* Padding */
  blake2s_compress( S, S->buf );

  for( i = 0; i < 8; ++i ) /* Output full hash to temp buffer */
    store32( buffer + sizeof( S->h[i] ) * i, S->h[i] );

  memcpy( out, buffer, outlen );
  secure_zero_memory(buffer, sizeof(buffer));
  return 0;
}

#include <assert.h>
#include <stdio.h>

void blake2s_run320(uint64_t nonce, uint8_t buf[320-8], uint8_t hash[32]) {
    uint32_t H0, H1, H2, H3, H4, H5, H6, H7;

    H0 = 0x6b08e647UL;
    H1 = IV(1);
    H2 = IV(2);
    H3 = IV(3);
    H4 = IV(4);
    H5 = IV(5);
    H6 = IV(6);
    H7 = IV(7);

    uint32_t T0 = 0, T1 = 0, F0 = 0, F1 = 0;

    uint32_t M0, M1, M2, M3, M4, M5, M6, M7;
    uint32_t M8, M9, MA, MB, MC, MD, ME, MF;
    uint32_t V0, V1, V2, V3, V4, V5, V6, V7;
    uint32_t V8, V9, VA, VB, VC, VD, VE, VF;

    uint8_t * nonce_ptr = (uint8_t*)(&nonce);

    // blake2s_update(S, nonce_ptr, 8);
    // blake2s_update(S, buf, 64-8);
    T0 += BLAKE2S_BLOCKBYTES;
    T1 += ( T0 < BLAKE2S_BLOCKBYTES );
    DO_COMPRESS(LOAD32(nonce_ptr), LOAD32(nonce_ptr+4), buf);

    // blake2s_update(S, buf-8+64, 64);
    T0 += BLAKE2S_BLOCKBYTES;
    T1 += ( T0 < BLAKE2S_BLOCKBYTES );
    DO_COMPRESS_SIMPLE(buf-8+64);

    // blake2s_update(S, buf-8+128, 64);
    T0 += BLAKE2S_BLOCKBYTES;
    T1 += ( T0 < BLAKE2S_BLOCKBYTES );
    DO_COMPRESS_SIMPLE(buf-8+128);

    // blake2s_update(S, buf-8+192, 64);
    T0 += BLAKE2S_BLOCKBYTES;
    T1 += ( T0 < BLAKE2S_BLOCKBYTES );
    DO_COMPRESS_SIMPLE(buf-8+192);

    // blake2s_update(S, buf-8+256, 64);
    // assert(S->buflen == 0);
    // printf("%d", S->buflen);
    // blake2s_increment_counter(S, 64);
    F0 = -1;
    T0 += BLAKE2S_BLOCKBYTES;
    T1 += ( T0 < BLAKE2S_BLOCKBYTES );
    DO_COMPRESS_SIMPLE(buf-8+256);

    // blake2s_final(S, hash, BLAKE2S_OUTBYTES);
    store32(hash  + 0, H0);
    store32(hash  + 4, H1);
    store32(hash  + 8, H2);
    store32(hash + 12, H3);
    store32(hash + 16, H4);
    store32(hash + 20, H5);
    store32(hash + 24, H6);
    store32(hash + 28, H7);
}

int blake2s( void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen )
{
  blake2s_state S[1];

  /* Verify parameters */
  if ( NULL == in && inlen > 0 ) return -1;

  if ( NULL == out ) return -1;

  if ( NULL == key && keylen > 0) return -1;

  if( !outlen || outlen > BLAKE2S_OUTBYTES ) return -1;

  if( keylen > BLAKE2S_KEYBYTES ) return -1;

  if( keylen > 0 )
  {
    if( blake2s_init_key( S, outlen, key, keylen ) < 0 ) return -1;
  }
  else
  {
    if( blake2s_init( S, outlen ) < 0 ) return -1;
  }

  blake2s_update( S, ( const uint8_t * )in, inlen );
  blake2s_final( S, out, outlen );
  return 0;
}