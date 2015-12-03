/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  Based on the public domain implementation of the SHA-1 algorithm
  Copyright (C) Dominik Reichl <dominik.reichl@t-online.de>
*/

#include "GLDSHA1.h"
#include "GLDStrUtils.h"
#include "GLDGlobal.h"

#include <QtEndian>

#ifdef Q_CC_MSVC
#  include <stdlib.h>
#endif


// #define or #undef this, if you want the to wipe all
// temporary variables after processing
#define SHA1_WIPE_VARIABLES


struct Sha1State
{
    quint32 h0;
    quint32 h1;
    quint32 h2;
    quint32 h3;
    quint32 h4;

    quint64 messageSize;
    unsigned char buffer[64];
};

typedef union
{
    quint8  bytes[64];
    quint32 words[16];
} Sha1Chunk;

static inline quint32 rol32(quint32 value, unsigned int shift)
{
#ifdef Q_CC_MSVC
    return _rotl(value, shift);
#else
    return ((value << shift) | (value >> (32 - shift)));
#endif
}

static inline quint32 sha1Word(Sha1Chunk *chunk, const uint position)
{
    return (chunk->words[position & 0xf] = rol32(  chunk->words[(position+13) & 0xf]
                                                 ^ chunk->words[(position+ 8) & 0xf]
                                                 ^ chunk->words[(position+ 2) & 0xf]
                                                 ^ chunk->words[(position)    & 0xf], 1));
}

static inline void sha1Round0(Sha1Chunk *chunk, const uint position,
                              quint32 &v, quint32 &w, quint32 &x, quint32 &y, quint32 &z)
{
    z += ((( w & (x ^ y)) ^ y) + chunk->words[position] + 0x5A827999 + rol32(v, 5));
    w = rol32(w, 30);
}

static inline void sha1Round1(Sha1Chunk *chunk, const uint position,
                              quint32 &v, quint32 &w, quint32 &x, quint32 &y, quint32 &z)
{
    z += ((( w & (x ^ y)) ^ y) + sha1Word(chunk,position) + 0x5A827999 + rol32(v, 5));
    w = rol32(w, 30);
}

static inline void sha1Round2(Sha1Chunk *chunk, const uint position,
                              quint32 &v, quint32 &w, quint32 &x, quint32 &y, quint32 &z)
{
    z += (( w ^ x ^ y) + sha1Word(chunk, position) + 0x6ED9EBA1 + rol32(v, 5));
    w = rol32(w, 30);
}

static inline void sha1Round3(Sha1Chunk *chunk, const uint position,
                              quint32 &v, quint32 &w, quint32 &x, quint32 &y, quint32 &z)
{
    z += (((( w | x) & y) | (w & x)) + sha1Word(chunk, position) + 0x8F1BBCDC + rol32(v, 5));
    w = rol32(w, 30);
}

static inline void sha1Round4(Sha1Chunk *chunk, const uint position,
                              quint32 &v, quint32 &w, quint32 &x, quint32 &y, quint32 &z)
{
    z += ((w ^ x ^ y) + sha1Word(chunk, position) + 0xCA62C1D6 + rol32(v, 5));
    w = rol32(w, 30);
}

static inline void sha1ProcessChunk(Sha1State *state, const unsigned char *buffer)
{
    // Copy state[] to working vars
    quint32 ua = state->h0;
    quint32 ub = state->h1;
    quint32 uc = state->h2;
    quint32 ud = state->h3;
    quint32 ue = state->h4;

    quint8 chunkBuffer[64];
    memcpy(chunkBuffer, buffer, 64);

    Sha1Chunk *chunk = reinterpret_cast<Sha1Chunk*>(&chunkBuffer);

    for (int i = 0; i < 16; ++i)
    {
        chunk->words[i] = qFromBigEndian(chunk->words[i]);
    }

    sha1Round0(chunk,  0, ua,ub,uc,ud,ue); sha1Round0(chunk,  1, ue,ua,ub,uc,ud);
    sha1Round0(chunk,  2, ud,ue,ua,ub,uc); sha1Round0(chunk,  3, uc,ud,ue,ua,ub);
    sha1Round0(chunk,  4, ub,uc,ud,ue,ua); sha1Round0(chunk,  5, ua,ub,uc,ud,ue);
    sha1Round0(chunk,  6, ue,ua,ub,uc,ud); sha1Round0(chunk,  7, ud,ue,ua,ub,uc);
    sha1Round0(chunk,  8, uc,ud,ue,ua,ub); sha1Round0(chunk,  9, ub,uc,ud,ue,ua);
    sha1Round0(chunk, 10, ua,ub,uc,ud,ue); sha1Round0(chunk, 11, ue,ua,ub,uc,ud);
    sha1Round0(chunk, 12, ud,ue,ua,ub,uc); sha1Round0(chunk, 13, uc,ud,ue,ua,ub);
    sha1Round0(chunk, 14, ub,uc,ud,ue,ua); sha1Round0(chunk, 15, ua,ub,uc,ud,ue);
    sha1Round1(chunk, 16, ue,ua,ub,uc,ud); sha1Round1(chunk, 17, ud,ue,ua,ub,uc);
    sha1Round1(chunk, 18, uc,ud,ue,ua,ub); sha1Round1(chunk, 19, ub,uc,ud,ue,ua);
    sha1Round2(chunk, 20, ua,ub,uc,ud,ue); sha1Round2(chunk, 21, ue,ua,ub,uc,ud);
    sha1Round2(chunk, 22, ud,ue,ua,ub,uc); sha1Round2(chunk, 23, uc,ud,ue,ua,ub);
    sha1Round2(chunk, 24, ub,uc,ud,ue,ua); sha1Round2(chunk, 25, ua,ub,uc,ud,ue);
    sha1Round2(chunk, 26, ue,ua,ub,uc,ud); sha1Round2(chunk, 27, ud,ue,ua,ub,uc);
    sha1Round2(chunk, 28, uc,ud,ue,ua,ub); sha1Round2(chunk, 29, ub,uc,ud,ue,ua);
    sha1Round2(chunk, 30, ua,ub,uc,ud,ue); sha1Round2(chunk, 31, ue,ua,ub,uc,ud);
    sha1Round2(chunk, 32, ud,ue,ua,ub,uc); sha1Round2(chunk, 33, uc,ud,ue,ua,ub);
    sha1Round2(chunk, 34, ub,uc,ud,ue,ua); sha1Round2(chunk, 35, ua,ub,uc,ud,ue);
    sha1Round2(chunk, 36, ue,ua,ub,uc,ud); sha1Round2(chunk, 37, ud,ue,ua,ub,uc);
    sha1Round2(chunk, 38, uc,ud,ue,ua,ub); sha1Round2(chunk, 39, ub,uc,ud,ue,ua);
    sha1Round3(chunk, 40, ua,ub,uc,ud,ue); sha1Round3(chunk, 41, ue,ua,ub,uc,ud);
    sha1Round3(chunk, 42, ud,ue,ua,ub,uc); sha1Round3(chunk, 43, uc,ud,ue,ua,ub);
    sha1Round3(chunk, 44, ub,uc,ud,ue,ua); sha1Round3(chunk, 45, ua,ub,uc,ud,ue);
    sha1Round3(chunk, 46, ue,ua,ub,uc,ud); sha1Round3(chunk, 47, ud,ue,ua,ub,uc);
    sha1Round3(chunk, 48, uc,ud,ue,ua,ub); sha1Round3(chunk, 49, ub,uc,ud,ue,ua);
    sha1Round3(chunk, 50, ua,ub,uc,ud,ue); sha1Round3(chunk, 51, ue,ua,ub,uc,ud);
    sha1Round3(chunk, 52, ud,ue,ua,ub,uc); sha1Round3(chunk, 53, uc,ud,ue,ua,ub);
    sha1Round3(chunk, 54, ub,uc,ud,ue,ua); sha1Round3(chunk, 55, ua,ub,uc,ud,ue);
    sha1Round3(chunk, 56, ue,ua,ub,uc,ud); sha1Round3(chunk, 57, ud,ue,ua,ub,uc);
    sha1Round3(chunk, 58, uc,ud,ue,ua,ub); sha1Round3(chunk, 59, ub,uc,ud,ue,ua);
    sha1Round4(chunk, 60, ua,ub,uc,ud,ue); sha1Round4(chunk, 61, ue,ua,ub,uc,ud);
    sha1Round4(chunk, 62, ud,ue,ua,ub,uc); sha1Round4(chunk, 63, uc,ud,ue,ua,ub);
    sha1Round4(chunk, 64, ub,uc,ud,ue,ua); sha1Round4(chunk, 65, ua,ub,uc,ud,ue);
    sha1Round4(chunk, 66, ue,ua,ub,uc,ud); sha1Round4(chunk, 67, ud,ue,ua,ub,uc);
    sha1Round4(chunk, 68, uc,ud,ue,ua,ub); sha1Round4(chunk, 69, ub,uc,ud,ue,ua);
    sha1Round4(chunk, 70, ua,ub,uc,ud,ue); sha1Round4(chunk, 71, ue,ua,ub,uc,ud);
    sha1Round4(chunk, 72, ud,ue,ua,ub,uc); sha1Round4(chunk, 73, uc,ud,ue,ua,ub);
    sha1Round4(chunk, 74, ub,uc,ud,ue,ua); sha1Round4(chunk, 75, ua,ub,uc,ud,ue);
    sha1Round4(chunk, 76, ue,ua,ub,uc,ud); sha1Round4(chunk, 77, ud,ue,ua,ub,uc);
    sha1Round4(chunk, 78, uc,ud,ue,ua,ub); sha1Round4(chunk, 79, ub,uc,ud,ue,ua);

    // Add the working vars back into state
    state->h0 += ua;
    state->h1 += ub;
    state->h2 += uc;
    state->h3 += ud;
    state->h4 += ue;

    // Wipe variables
#ifdef SHA1_WIPE_VARIABLES
    ua = ub = uc = ud = ue = 0;
    memset(chunkBuffer, 0, 64);
#endif
}

static inline void sha1InitState(Sha1State *state)
{
    state->h0 = 0x67452301;
    state->h1 = 0xEFCDAB89;
    state->h2 = 0x98BADCFE;
    state->h3 = 0x10325476;
    state->h4 = 0xC3D2E1F0;

    state->messageSize = 0;
}

static inline void sha1Update(Sha1State *state, const unsigned char *data, gint64 len)
{
    quint32 rest = static_cast<quint32>(state->messageSize & Q_UINT64_C(63));

    quint64 availableData = static_cast<quint64>(len) + static_cast<quint64>(rest);
    state->messageSize += len;

    if (availableData < Q_UINT64_C(64))
    {
        memcpy(&state->buffer[rest], &data[0], len);
    }
    else
    {
        gint64 nIter = static_cast<gint64>(64 - rest);
        memcpy(&state->buffer[rest], &data[0], static_cast<qint32>(nIter));
        sha1ProcessChunk(state, state->buffer);

        gint64 lastI = len - ((len + rest) & Q_INT64_C(63));
        for( ; nIter < lastI; nIter += 64)
            sha1ProcessChunk(state, &data[nIter]);

        memcpy(&state->buffer[0], &data[nIter], len - nIter);
    }
}

static inline void sha1FinalizeState(Sha1State *state)
{
    quint64 messageSize = state->messageSize;
    unsigned char sizeInBits[8];
    qToBigEndian(messageSize << 3, sizeInBits);

    sha1Update(state, (const unsigned char *)"\200", 1);

    unsigned char zero[64];
    memset(zero, 0, 64);
    if (static_cast<int>(messageSize & 63) > 56 - 1)
    {
        sha1Update(state, zero, 64 - 1 - static_cast<int>(messageSize & 63));
        sha1Update(state, zero, 64 - 8);
    }
    else
    {
        sha1Update(state, zero, 64 - 1 - 8 - static_cast<int>(messageSize & 63));
    }

    sha1Update(state, sizeInBits, 8);
#ifdef SHA1_WIPE_VARIABLES
    memset(state->buffer, 0, 64);
    memset(zero, 0, 64);
    state->messageSize = 0;
#endif
}

//static inline void sha1ToHash(Sha1State *state, unsigned char* buffer)
//{
//    qToBigEndian(state->h0, buffer);
//    qToBigEndian(state->h1, buffer + 4);
//    qToBigEndian(state->h2, buffer + 8);
//    qToBigEndian(state->h3, buffer + 12);
//    qToBigEndian(state->h4, buffer + 16);
//}


GString getSHA1(const unsigned char *buffer, size_t bufSize)
{
    SHA1Digest digest;
    SHA1HashBuffer(digest, buffer, bufSize);
    GString result;
    for (int i = 0; i != 20; ++i)
    {
        result.append(intToHex(digest.bytes[i], 2));
    }
    return result;
}

GString getSHA1(GStream *stream)
{
    SHA1Digest digest;
    SHA1HashStream(digest, stream);
    GString result;
    for (int i = 0; i != 20; ++i)
    {
        result.append(intToHex(digest.bytes[i], 2));
    }
    return result;
}

void Sha1StateToSHA1Digest(const Sha1State &state, SHA1Digest &digest)
{
    gMemMove(&digest, &state, 20);
    for (byte i = 0; i != 5; ++i)
    {
        for (byte j = 0; j != 2; ++j)
        {
            byte byteValue = digest.bytes[i * 4 + j];
            digest.bytes[i * 4 + j] = digest.bytes[i * 4 + 3 - j];
            digest.bytes[i * 4 + 3 - j] = byteValue;
        }
    }
}

void SHA1HashBuffer(SHA1Digest &digest, const unsigned char *buffer, size_t bufSize)
{
    Sha1State state;
    sha1InitState(&state);
    sha1Update(&state, buffer, bufSize);
    sha1FinalizeState(&state);
    Sha1StateToSHA1Digest(state, digest);
}

static inline void sha1UpdateStream(Sha1State *state, GStream *stream)
{
    gint64 len = stream->size() - stream->pos();
    quint32 rest = static_cast<quint32>(state->messageSize & Q_UINT64_C(63));

    quint64 availableData = static_cast<quint64>(len) + static_cast<quint64>(rest);
    state->messageSize += len;

    if (availableData < Q_UINT64_C(64))
    {
        //memcpy(&state->buffer[rest], &data[0], len);
        stream->read((char *)(&state->buffer[rest]), len);
    }
    else
    {
        gint64 nIter = static_cast<gint64>(64 - rest);
        //memcpy(&state->buffer[rest], &data[0], static_cast<qint32>(i));
        stream->read((char *)(&state->buffer[rest]), static_cast<qint32>(nIter));
        sha1ProcessChunk(state, state->buffer);

        gint64 lastI = len - ((len + rest) & Q_INT64_C(63));
        unsigned char buf[64];
        for( ; nIter < lastI; nIter += 64)
        {
            //sha1ProcessChunk(state, &data[i]);
            stream->read((char *)&buf[0], 64);
            sha1ProcessChunk(state, &buf[0]);
        }
        //memcpy(&state->buffer[0], &data[i], len - i);
        stream->read((char *)(&state->buffer[0]), len - nIter);
    }
}

void SHA1HashStream(SHA1Digest &digest, GStream *stream)
{
    Sha1State state;
    sha1InitState(&state);
    sha1UpdateStream(&state, stream);
    sha1FinalizeState(&state);
    Sha1StateToSHA1Digest(state, digest);
}
