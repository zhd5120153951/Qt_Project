#ifndef GLDSHA1_H
#define GLDSHA1_H

#include "GLDCrypt_Global.h"
#include "GLDString.h"
#include "GLDIODevice.h"
#include "GLDSystem.h"
#include "GLDCrypt_Global.h"

GLDCRYPTSHARED_EXPORT GString getSHA1(const unsigned char *buffer, size_t bufSize);
GLDCRYPTSHARED_EXPORT GString getSHA1(GStream *stream);

struct SHA1Digest
{
    byte bytes[20];
};

GLDCRYPTSHARED_EXPORT void SHA1HashBuffer(SHA1Digest &digest, const unsigned char *buffer, size_t bufSize);
GLDCRYPTSHARED_EXPORT void SHA1HashStream(SHA1Digest &digest, GStream *stream);

#endif // GLDSHA1_H
