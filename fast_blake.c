#if __ENDIAN_LITTLE__
# define NATIVE_LITTLE_ENDIAN 1
#endif

#if __LITTLE_ENDIAN__
# define NATIVE_LITTLE_ENDIAN 1
#endif

#define BLAKE2S_BLOCKBYTES 64

typedef unsigned long  uint64_t;
typedef signed   long   int64_t;
typedef unsigned int   uint32_t;
typedef signed   int    int32_t;
typedef unsigned short uint16_t;
typedef signed   short  int16_t;
typedef unsigned char   uint8_t;
typedef signed   char    int8_t;

static inline uint32_t rotr32( const uint32_t w, const unsigned c ) {
  return ( w >> c ) | ( w << ( 32 - c ) );
}

#define SWAP32(x) (( uint32_t )( x & 0xFF000000 ) >> 24) | \
         (( uint32_t )( x & 0x00FF0000 ) >> 8) | \
         (( uint32_t )( x & 0x0000FF00 ) << 8) | \
         (( uint32_t )( x & 0x000000FF ) << 24)

#if defined(NATIVE_LITTLE_ENDIAN)
# define LOAD32(dst, src) do { dst = (*(const uint32_t*)(src)); } while(0)
# define STORE32(dst, src) do { (*(uint32_t*)(dst)) = src; } while(0)
#else
# define LOAD32(dst, src) do { \
    dst = (*(const uint32_t*)src); \
    dst = SWAP32(dst); \
  } while(0)
# define STORE32(dst, src) do { \
    uint32_t tmp = src; \
    tmp = SWAP32(tmp); \
    (*(uint32_t*)(dst)) = tmp; \
  } while(0)
#endif

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

#define G(m0, m1, a,b,c,d)                      \
  do {                                      \
    a = a + b + (m0); \
    d = rotr32(d ^ a, 16);                  \
    c = c + d;                              \
    b = rotr32(b ^ c, 12);                  \
    a = a + b + (m1); \
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
  } while(0)

#define DO_COMPRESS(m0, m1, block) do { \
    LOAD32(M0, m0); \
    LOAD32(M1, m1); \
    LOAD32(M2, block +   8-8); \
    LOAD32(M3, block +  12-8); \
    LOAD32(M4, block +  16-8); \
    LOAD32(M5, block +  20-8); \
    LOAD32(M6, block +  24-8); \
    LOAD32(M7, block +  28-8); \
    LOAD32(M8, block +  32-8); \
    LOAD32(M9, block +  36-8); \
    LOAD32(MA, block +  40-8); \
    LOAD32(MB, block +  44-8); \
    LOAD32(MC, block +  48-8); \
    LOAD32(MD, block +  52-8); \
    LOAD32(ME, block +  56-8); \
    LOAD32(MF, block +  60-8); \
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
  } while(0)

#define DO_COMPRESS_SIMPLE(buf) DO_COMPRESS(buf+0, buf+4, buf+8)

void blake2s_run320(uint64_t* nonce, uint8_t buf[320-8], uint8_t hash[32]) {
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

    uint8_t * nonce_ptr = (uint8_t*)(nonce);

    // blake2s_update(S, nonce_ptr, 8);
    // blake2s_update(S, buf, 64-8);
    T0 += BLAKE2S_BLOCKBYTES;
    T1 += ( T0 < BLAKE2S_BLOCKBYTES );
    DO_COMPRESS(nonce_ptr, nonce_ptr+4, buf);

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
    STORE32(hash  + 0, H0);
    STORE32(hash  + 4, H1);
    STORE32(hash  + 8, H2);
    STORE32(hash + 12, H3);
    STORE32(hash + 16, H4);
    STORE32(hash + 20, H5);
    STORE32(hash + 24, H6);
    STORE32(hash + 28, H7);
}

void search_nonce(
  uint64_t gid,
  uint64_t* start_nonce,
  uint64_t work_set,
  uint8_t* buf,
  uint8_t* target_hash,
  uint8_t* result_ptr
) {
    uint64_t nonce0 = *start_nonce + gid * work_set;

    printf("gid=%d\n", gid);
    printf("start_nonce=%lld\n", *start_nonce);
    printf("work_set=%lld\n", work_set);
    printf("buf={%d, %d, %d, %d, ...}\n", buf[0], buf[1], buf[2], buf[3]);
    printf("target_hash={%d, %d, %d, %d, ...}\n", target_hash[0], target_hash[1], target_hash[2], target_hash[3]);

    uint8_t target[32];
    for (int i = 0; i < 32; i++) target[i] = target_hash[i];

    for (uint64_t i = 0; i < work_set; i++) {
      uint64_t nonce[1] = { nonce0 + i };

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

      uint8_t * nonce_ptr = (uint8_t*)(nonce);

      // blake2s_update(S, nonce_ptr, 8);
      // blake2s_update(S, buf, 64-8);
      T0 += BLAKE2S_BLOCKBYTES;
      T1 += ( T0 < BLAKE2S_BLOCKBYTES );
      DO_COMPRESS(nonce_ptr, nonce_ptr+4, buf);

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
      uint32_t hash[8] = { H0, H1, H2, H3, H4, H5, H6, H7 };

      // printf("%lld %llx %llx\n", nonce[0], ((uint64_t*) target)[3], ((uint64_t*) &hash)[3]);

      uint8_t result = 0;
      for (int i = 0; i < 4; i++) {
          uint64_t h1 = ((uint64_t*) target)[3 - i];
          uint64_t h2 = ((uint64_t*) &hash)[3 - i];
          if (h1 > h2) {
              result = 1;
              break;
          } else if (h1 == h2) continue;
          else {
            result = -1;
            break;
          }
      }
      result_ptr[gid * work_set + i] = result;
    }
}