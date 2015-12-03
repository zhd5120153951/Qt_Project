#ifndef GLDDES_H
#define GLDDES_H

#include "GLDCrypt_Global.h"
#include "GLDStream.h"
#include "GLDCrypt_Global.h"

#pragma pack(push, 1)
struct GDESContext
{
    unsigned long transformedKey[32];/*range 0..31*/
    bool encrypt;
};

#pragma pack(pop)

typedef unsigned char GKey64[8];/*range 0..7*/
typedef unsigned char GKey128[16];/*range 0..15*/

/*3 DES */
typedef GDESContext GTripleDESContext[2];/*range 0..1*/

/*GSP 加密文件头 */

struct CGSPEncryptRec
{
    unsigned int version;
    byte key[16];
};

/*GSP 处理密钥 */

GLDCRYPTSHARED_EXPORT extern const unsigned char c_DefDESKey[16];


GLDCRYPTSHARED_EXPORT unsigned char* DES(const unsigned char* inString, int inSize, GKey64 key64, bool encrypt);

GLDCRYPTSHARED_EXPORT unsigned char* DESEncrypt(const unsigned char* inString, int inSize, GKey64 key64, bool encrypt);
GLDCRYPTSHARED_EXPORT void DESEncryptStream(GStream *inStream, GStream *outStream, GKey64 key64, bool encrypt);

GLDCRYPTSHARED_EXPORT unsigned char* tripleDES(unsigned char* inString, int inSize, const GKey128 key128, bool encrypt);

GLDCRYPTSHARED_EXPORT unsigned char* tripleDESEncrypt(const unsigned char* inString, int inSize, GKey128 key128, bool encrypt);
GLDCRYPTSHARED_EXPORT void tripleDESEncryptStream(GStream *inStream, GStream *outStream, const GKey128 key128, bool encrypt);

GLDCRYPTSHARED_EXPORT void overlapDES(unsigned char *inString, int inSize, GKey64 key64, bool encrypt);

#endif // GLDDES_H
