/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 *
 * Changed so as no longer to depend on Colin Plumb's `usual.h' header
 * definitions; now uses stuff from dpkg's config.h.
 *  - Ian Jackson <ian@chiark.greenend.org.uk>.
 * Still in the public domain.
 */

#include "GLDMD5.h"
#include "GLDGlobal.h"
#include "GLDStrUtils.h"
#include "GLDFile.h"

#include <string.h>        /* for memcpy() */
#ifndef _WIN32_WCE
#include <sys/types.h>        /* for stupid systems */
#else
#include <windef.h>
#include <types.h>
#endif

GString getMD5(const char *buffer, size_t bufSize)
{
    md5byte arrMD5Digest[16];
    for (int i = 0; i != 16; ++i)
    {
        arrMD5Digest[i] = i + 1;
    }
    MD5Context oMD5Context;
    MD5Init(&oMD5Context);
    MD5UpdateBuffer(&oMD5Context, buffer, bufSize);
    MD5Final(&oMD5Context, arrMD5Digest);
    GString result;
    for (int i = 0; i != 16; ++i)
    {
        result.append(intToHex(arrMD5Digest[i], 2));
    }
    return result;
}

GString getMD5(GStream *stream)
{
    md5byte arrMD5Digest[16];
    for (int i = 0; i != 16; ++i)
    {
        arrMD5Digest[i] = i + 1;
    }
    MD5Context oMD5Context;
    MD5Init(&oMD5Context);
    MD5UpdateBuffer(&oMD5Context, stream);
    MD5Final(&oMD5Context, arrMD5Digest);
    GString result;
    for (int i = 0; i != 16; ++i)
    {
        result.append(intToHex(arrMD5Digest[i], 2));
    }
    return result;
}

GString strMD5(const GString &buffer)
{
    GByteArray buf = strToByteArray(buffer);
    return getMD5(buf.constData(), buf.size());
}

GString fileMD5(const GString &fileName)
{
    GString result;
    GFileStream file(fileName);
    if (!file.open(GStream::ReadOnly))
        return result;
    else
    {
        result = getMD5(&file);
        file.close();
        return result;
    }
}

static void byteSwap(UWORD32 *buf, unsigned words)
{
    const quint32 c_ByteOrderTest = 0x1;
    if (((char *)&c_ByteOrderTest)[0] == 0)
    {
        md5byte *pBuf = (md5byte *)buf;

        do
        {
            *buf++ = (UWORD32)((unsigned)pBuf[3] << 8 | pBuf[2]) << 16
                                                              | ((unsigned)pBuf[1] << 8 | pBuf[0]);
            pBuf += 4;
        } while (--words != 0);
    }
}

/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
static void MD5Init(struct MD5Context *ctx)
{
    memset(ctx, 0, sizeof(MD5Context));
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bytes[0] = 0;
    ctx->bytes[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
static void MD5Update(struct MD5Context *ctx, md5byte const *buf, unsigned len)
{
    UWORD32 uTmp;

    /* Update byte count */

    uTmp = ctx->bytes[0];
    if ((ctx->bytes[0] = uTmp + len) < uTmp)
        ctx->bytes[1]++;    /* Carry from low to high */

    uTmp = 64 - (uTmp & 0x3f);    /* Space available in ctx->in (at least 1) */
    if (uTmp > len)
    {
        memcpy((md5byte *)ctx->in + 64 - uTmp, buf, len);
        return;
    }
    /* First chunk is an odd size */
    memcpy((md5byte *)ctx->in + 64 - uTmp, buf, uTmp);
    byteSwap(ctx->in, 16);
    MD5Transform(ctx->buf, ctx->in);
    buf += uTmp;
    len -= uTmp;

    /* Process data in 64-byte chunks */
    while (len >= 64)
    {
        memcpy(ctx->in, buf, 64);
        byteSwap(ctx->in, 16);
        MD5Transform(ctx->buf, ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
static void MD5Final(struct MD5Context *ctx, md5byte digest[16])
{
    int nCount = ctx->bytes[0] & 0x3f;    /* Number of bytes in ctx->in */
    md5byte *pByte = (md5byte *)ctx->in + nCount;

    /* Set the first char of padding to 0x80.  There is always room. */
    *pByte++ = 0x80;

    /* Bytes of padding needed to make 56 bytes (-8..55) */
    nCount = 56 - 1 - nCount;

    if (nCount < 0)
    {    /* Padding forces an extra block */
        memset(pByte, 0, nCount + 8);
        byteSwap(ctx->in, 16);
        MD5Transform(ctx->buf, ctx->in);
        pByte = (md5byte *)ctx->in;
        nCount = 56;
    }
    memset(pByte, 0, nCount);
    byteSwap(ctx->in, 14);

    /* Append length in bits and transform */
    ctx->in[14] = ctx->bytes[0] << 3;
    ctx->in[15] = ctx->bytes[1] << 3 | ctx->bytes[0] >> 29;
    MD5Transform(ctx->buf, ctx->in);

    byteSwap(ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(*ctx));    /* In case it's sensitive */
}

#ifndef ASM_MD5

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s) \
    (w += f(x,y,z) + in, w = (w<<s | w>>(32-s)) + x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void MD5Transform(UWORD32 buf[4], UWORD32 const in[16])
{
    register UWORD32 uA;
    register UWORD32 uB;
    register UWORD32 uC;
    register UWORD32 uD;

    uA = buf[0];
    uB = buf[1];
    uC = buf[2];
    uD = buf[3];

    MD5STEP(F1, uA, uB, uC, uD, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, uD, uA, uB, uC, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, uC, uD, uA, uB, in[2] + 0x242070db, 17);
    MD5STEP(F1, uB, uC, uD, uA, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, uA, uB, uC, uD, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, uD, uA, uB, uC, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, uC, uD, uA, uB, in[6] + 0xa8304613, 17);
    MD5STEP(F1, uB, uC, uD, uA, in[7] + 0xfd469501, 22);
    MD5STEP(F1, uA, uB, uC, uD, in[8] + 0x698098d8, 7);
    MD5STEP(F1, uD, uA, uB, uC, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, uC, uD, uA, uB, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, uB, uC, uD, uA, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, uA, uB, uC, uD, in[12] + 0x6b901122, 7);
    MD5STEP(F1, uD, uA, uB, uC, in[13] + 0xfd987193, 12);
    MD5STEP(F1, uC, uD, uA, uB, in[14] + 0xa679438e, 17);
    MD5STEP(F1, uB, uC, uD, uA, in[15] + 0x49b40821, 22);

    MD5STEP(F2, uA, uB, uC, uD, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, uD, uA, uB, uC, in[6] + 0xc040b340, 9);
    MD5STEP(F2, uC, uD, uA, uB, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, uB, uC, uD, uA, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, uA, uB, uC, uD, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, uD, uA, uB, uC, in[10] + 0x02441453, 9);
    MD5STEP(F2, uC, uD, uA, uB, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, uB, uC, uD, uA, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, uA, uB, uC, uD, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, uD, uA, uB, uC, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, uC, uD, uA, uB, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, uB, uC, uD, uA, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, uA, uB, uC, uD, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, uD, uA, uB, uC, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, uC, uD, uA, uB, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, uB, uC, uD, uA, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, uA, uB, uC, uD, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, uD, uA, uB, uC, in[8] + 0x8771f681, 11);
    MD5STEP(F3, uC, uD, uA, uB, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, uB, uC, uD, uA, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, uA, uB, uC, uD, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, uD, uA, uB, uC, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, uC, uD, uA, uB, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, uB, uC, uD, uA, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, uA, uB, uC, uD, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, uD, uA, uB, uC, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, uC, uD, uA, uB, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, uB, uC, uD, uA, in[6] + 0x04881d05, 23);
    MD5STEP(F3, uA, uB, uC, uD, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, uD, uA, uB, uC, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, uC, uD, uA, uB, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, uB, uC, uD, uA, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, uA, uB, uC, uD, in[0] + 0xf4292244, 6);
    MD5STEP(F4, uD, uA, uB, uC, in[7] + 0x432aff97, 10);
    MD5STEP(F4, uC, uD, uA, uB, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, uB, uC, uD, uA, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, uA, uB, uC, uD, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, uD, uA, uB, uC, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, uC, uD, uA, uB, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, uB, uC, uD, uA, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, uA, uB, uC, uD, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, uD, uA, uB, uC, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, uC, uD, uA, uB, in[6] + 0xa3014314, 15);
    MD5STEP(F4, uB, uC, uD, uA, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, uA, uB, uC, uD, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, uD, uA, uB, uC, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, uC, uD, uA, uB, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, uB, uC, uD, uA, in[9] + 0xeb86d391, 21);

    buf[0] += uA;
    buf[1] += uB;
    buf[2] += uC;
    buf[3] += uD;
}

const unsigned short c_MaxBufSize = 16384;

static void MD5UpdateBuffer(struct MD5Context *context, const char *buffer, size_t bufSize)
{
    md5byte *pBufTmp = (md5byte *)gMalloc(c_MaxBufSize);
    unsigned short uBytes;
    try
    {
        const char *pBufPtr = buffer;
        do
        {
            if (bufSize > c_MaxBufSize)
                uBytes = c_MaxBufSize;
            else
                uBytes = (unsigned short)bufSize;
            gMemMove(pBufTmp, pBufPtr, uBytes);
            pBufPtr += uBytes;
            bufSize -= uBytes;
            if (uBytes > 0)
                MD5Update(context, pBufTmp, uBytes);
        } while (uBytes >= c_MaxBufSize);
    }
    catch (...)
    {
        gFree(pBufTmp);
        throw;
    }
    gFree(pBufTmp);
}

static void MD5UpdateBuffer(struct MD5Context *context, GStream *stream)
{
    md5byte *pBufTmp = (md5byte *)gMalloc(c_MaxBufSize);
    unsigned short uBytes;
    try
    {
        long long llBufSize = stream->size();
        stream->seek(0);
        do
        {
            if (llBufSize > c_MaxBufSize)
                uBytes = c_MaxBufSize;
            else
                uBytes = (unsigned short)llBufSize;
            stream->read((char *)pBufTmp, uBytes);
            llBufSize -= uBytes;
            if (uBytes > 0)
                MD5Update(context, pBufTmp, uBytes);
        } while (uBytes >= c_MaxBufSize);
    }
    catch (...)
    {
        gFree(pBufTmp);
        throw;
    }
    gFree(pBufTmp);
}

#endif
