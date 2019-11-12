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

#define G(m0, m1, a,b,c,d)       \
  do {                           \
    a = a + b + (m0);            \
    d = rotate(d ^ a, (uint)16); \
    c = c + d;                   \
    b = rotate(b ^ c, (uint)20); \
    a = a + b + (m1);            \
    d = rotate(d ^ a, (uint)24); \
    c = c + d;                   \
    b = rotate(b ^ c, (uint)25); \
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

#define Bx(r, i) B ## r ## i

#define DO_COMPRESS(r, f0, t0) do { \
    M2 = Bx(r, 2);              \
    M3 = Bx(r, 3);              \
    M4 = Bx(r, 4);              \
    M5 = Bx(r, 5);              \
    M6 = Bx(r, 6);              \
    M7 = Bx(r, 7);              \
    M8 = Bx(r, 8);              \
    M9 = Bx(r, 9);              \
    MA = Bx(r, A);              \
    MB = Bx(r, B);              \
    MC = Bx(r, C);              \
    MD = Bx(r, D);              \
    ME = Bx(r, E);              \
    MF = Bx(r, F);              \
    V0 = H0;                    \
    V1 = H1;                    \
    V2 = H2;                    \
    V3 = H3;                    \
    V4 = H4;                    \
    V5 = H5;                    \
    V6 = H6;                    \
    V7 = H7;                    \
    V8 = IV0;                   \
    V9 = IV1;                   \
    VA = IV2;                   \
    VB = IV3;                   \
    VC = t0 ^ IV4;              \
    VD = IV5;                   \
    VE = f0 ^ IV6;              \
    VF = IV7;                   \
    ROUND(0);                   \
    ROUND(1);                   \
    ROUND(2);                   \
    ROUND(3);                   \
    ROUND(4);                   \
    ROUND(5);                   \
    ROUND(6);                   \
    ROUND(7);                   \
    ROUND(8);                   \
    ROUND(9);                   \
    H0 = H0 ^ V0 ^ V8;          \
    H1 = H1 ^ V1 ^ V9;          \
    H2 = H2 ^ V2 ^ VA;          \
    H3 = H3 ^ V3 ^ VB;          \
    H4 = H4 ^ V4 ^ VC;          \
    H5 = H5 ^ V5 ^ VD;          \
    H6 = H6 ^ V6 ^ VE;          \
    H7 = H7 ^ V7 ^ VF;          \
  } while (0)

#define DO_COMPRESS_SIMPLE(r, f0, t0) do { \
    M0 = Bx(r, 0);                         \
    M1 = Bx(r, 1);                         \
    DO_COMPRESS(r, f0, t0);                \
  } while (0)

#ifdef COMPARE_ALL
  #define TEST_RESULT() (                           \
      A0 > A                                        \
      || (A0 == A && B0 > B)                        \
      || (A0 == A && B0 == B && C0 > C)             \
      || (A0 == A && B0 == B && C0 == C && D0 >= D) \
    )
#else
  #define TEST_RESULT() (A0 > A)
#endif


kernel void search_nonce(uint64_t start_nonce, global uint64_t* result_ptr) {
  size_t gid = get_global_id(0);

  uint64_t nonce0 = start_nonce + gid * WORKSET_SIZE;

  for (uint64_t i = 0; i < WORKSET_SIZE; i++) {
    uint64_t nonce = nonce0 + i;
    uint32_t H0, H1, H2, H3, H4, H5, H6, H7;

    H0 = 0x6b08e647UL;
    H1 = IV(1);
    H2 = IV(2);
    H3 = IV(3);
    H4 = IV(4);
    H5 = IV(5);
    H6 = IV(6);
    H7 = IV(7);

    uint32_t M0, M1, M2, M3, M4, M5, M6, M7;
    uint32_t M8, M9, MA, MB, MC, MD, ME, MF;
    uint32_t V0, V1, V2, V3, V4, V5, V6, V7;
    uint32_t V8, V9, VA, VB, VC, VD, VE, VF;

    M0 = (uint32_t) (nonce & 0xFFFFFFFF);
    M1 = (uint32_t) (nonce >> 32);

    DO_COMPRESS(0, 0, 0x40);
    DO_COMPRESS_SIMPLE(1, 0x0, 0x80);
    DO_COMPRESS_SIMPLE(2, 0x0, 0xC0);
    DO_COMPRESS_SIMPLE(3, 0x0, 0x100);
    DO_COMPRESS_SIMPLE(4, 0xFFFFFFFF, 0x11E);

    uint64_t A = (((uint64_t) H7) << 32) | H6;

    #ifdef COMPARE_ALL
    uint64_t B = (((uint64_t) H5) << 32) | H4;
    uint64_t C = (((uint64_t) H3) << 32) | H2;
    uint64_t D = (((uint64_t) H1) << 32) | H0;
    #endif

    if (TEST_RESULT()) {
      *result_ptr = nonce;
    }
  }
}
