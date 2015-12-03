#include "GLDDes.h"

#include "GLDZlib.h"
#include "GLDGlobal.h"
#include "GLDXMLUtils.h"
#include "GLDException.h"

const unsigned char c_DefDESKey[16] = {0xCE, 0xD2, 0xCA, 0xC7, 0xC1, 0xF5, 0xCF, 0xFE,
                      0xB6, 0xB0, 0xA3, 0xAC, 0xCE, 0xD2, 0xC5, 0xC2};

void initEncryptTripleDES(const GKey128 key, GDESContext *context, bool encrypt);
void encryptTripleDES(GDESContext *context, unsigned char *block);
void encryptDES(GDESContext &context, unsigned char *block);

#define BLOCK_SIZE 8

static const unsigned long c_PBox[8][64] =
{
    {0x01010400, 0x00000000, 0x00010000, 0x01010404,
     0x01010004, 0x00010404, 0x00000004, 0x00010000,
     0x00000400, 0x01010400, 0x01010404, 0x00000400,
     0x01000404, 0x01010004, 0x01000000, 0x00000004,
     0x00000404, 0x01000400, 0x01000400, 0x00010400,
     0x00010400, 0x01010000, 0x01010000, 0x01000404,
     0x00010004, 0x01000004, 0x01000004, 0x00010004,
     0x00000000, 0x00000404, 0x00010404, 0x01000000,
     0x00010000, 0x01010404, 0x00000004, 0x01010000,
     0x01010400, 0x01000000, 0x01000000, 0x00000400,
     0x01010004, 0x00010000, 0x00010400, 0x01000004,
     0x00000400, 0x00000004, 0x01000404, 0x00010404,
     0x01010404, 0x00010004, 0x01010000, 0x01000404,
     0x01000004, 0x00000404, 0x00010404, 0x01010400,
     0x00000404, 0x01000400, 0x01000400, 0x00000000,
     0x00010004, 0x00010400, 0x00000000, 0x01010004},

    {0x80108020, 0x80008000, 0x00008000, 0x00108020,
     0x00100000, 0x00000020, 0x80100020, 0x80008020,
     0x80000020, 0x80108020, 0x80108000, 0x80000000,
     0x80008000, 0x00100000, 0x00000020, 0x80100020,
     0x00108000, 0x00100020, 0x80008020, 0x00000000,
     0x80000000, 0x00008000, 0x00108020, 0x80100000,
     0x00100020, 0x80000020, 0x00000000, 0x00108000,
     0x00008020, 0x80108000, 0x80100000, 0x00008020,
     0x00000000, 0x00108020, 0x80100020, 0x00100000,
     0x80008020, 0x80100000, 0x80108000, 0x00008000,
     0x80100000, 0x80008000, 0x00000020, 0x80108020,
     0x00108020, 0x00000020, 0x00008000, 0x80000000,
     0x00008020, 0x80108000, 0x00100000, 0x80000020,
     0x00100020, 0x80008020, 0x80000020, 0x00100020,
     0x00108000, 0x00000000, 0x80008000, 0x00008020,
     0x80000000, 0x80100020, 0x80108020, 0x00108000},

    {0x00000208, 0x08020200, 0x00000000, 0x08020008,
     0x08000200, 0x00000000, 0x00020208, 0x08000200,
     0x00020008, 0x08000008, 0x08000008, 0x00020000,
     0x08020208, 0x00020008, 0x08020000, 0x00000208,
     0x08000000, 0x00000008, 0x08020200, 0x00000200,
     0x00020200, 0x08020000, 0x08020008, 0x00020208,
     0x08000208, 0x00020200, 0x00020000, 0x08000208,
     0x00000008, 0x08020208, 0x00000200, 0x08000000,
     0x08020200, 0x08000000, 0x00020008, 0x00000208,
     0x00020000, 0x08020200, 0x08000200, 0x00000000,
     0x00000200, 0x00020008, 0x08020208, 0x08000200,
     0x08000008, 0x00000200, 0x00000000, 0x08020008,
     0x08000208, 0x00020000, 0x08000000, 0x08020208,
     0x00000008, 0x00020208, 0x00020200, 0x08000008,
     0x08020000, 0x08000208, 0x00000208, 0x08020000,
     0x00020208, 0x00000008, 0x08020008, 0x00020200},

    {0x00802001, 0x00002081, 0x00002081, 0x00000080,
     0x00802080, 0x00800081, 0x00800001, 0x00002001,
     0x00000000, 0x00802000, 0x00802000, 0x00802081,
     0x00000081, 0x00000000, 0x00800080, 0x00800001,
     0x00000001, 0x00002000, 0x00800000, 0x00802001,
     0x00000080, 0x00800000, 0x00002001, 0x00002080,
     0x00800081, 0x00000001, 0x00002080, 0x00800080,
     0x00002000, 0x00802080, 0x00802081, 0x00000081,
     0x00800080, 0x00800001, 0x00802000, 0x00802081,
     0x00000081, 0x00000000, 0x00000000, 0x00802000,
     0x00002080, 0x00800080, 0x00800081, 0x00000001,
     0x00802001, 0x00002081, 0x00002081, 0x00000080,
     0x00802081, 0x00000081, 0x00000001, 0x00002000,
     0x00800001, 0x00002001, 0x00802080, 0x00800081,
     0x00002001, 0x00002080, 0x00800000, 0x00802001,
     0x00000080, 0x00800000, 0x00002000, 0x00802080},

    {0x00000100, 0x02080100, 0x02080000, 0x42000100,
     0x00080000, 0x00000100, 0x40000000, 0x02080000,
     0x40080100, 0x00080000, 0x02000100, 0x40080100,
     0x42000100, 0x42080000, 0x00080100, 0x40000000,
     0x02000000, 0x40080000, 0x40080000, 0x00000000,
     0x40000100, 0x42080100, 0x42080100, 0x02000100,
     0x42080000, 0x40000100, 0x00000000, 0x42000000,
     0x02080100, 0x02000000, 0x42000000, 0x00080100,
     0x00080000, 0x42000100, 0x00000100, 0x02000000,
     0x40000000, 0x02080000, 0x42000100, 0x40080100,
     0x02000100, 0x40000000, 0x42080000, 0x02080100,
     0x40080100, 0x00000100, 0x02000000, 0x42080000,
     0x42080100, 0x00080100, 0x42000000, 0x42080100,
     0x02080000, 0x00000000, 0x40080000, 0x42000000,
     0x00080100, 0x02000100, 0x40000100, 0x00080000,
     0x00000000, 0x40080000, 0x02080100, 0x40000100},

    {0x20000010, 0x20400000, 0x00004000, 0x20404010,
     0x20400000, 0x00000010, 0x20404010, 0x00400000,
     0x20004000, 0x00404010, 0x00400000, 0x20000010,
     0x00400010, 0x20004000, 0x20000000, 0x00004010,
     0x00000000, 0x00400010, 0x20004010, 0x00004000,
     0x00404000, 0x20004010, 0x00000010, 0x20400010,
     0x20400010, 0x00000000, 0x00404010, 0x20404000,
     0x00004010, 0x00404000, 0x20404000, 0x20000000,
     0x20004000, 0x00000010, 0x20400010, 0x00404000,
     0x20404010, 0x00400000, 0x00004010, 0x20000010,
     0x00400000, 0x20004000, 0x20000000, 0x00004010,
     0x20000010, 0x20404010, 0x00404000, 0x20400000,
     0x00404010, 0x20404000, 0x00000000, 0x20400010,
     0x00000010, 0x00004000, 0x20400000, 0x00404010,
     0x00004000, 0x00400010, 0x20004010, 0x00000000,
     0x20404000, 0x20000000, 0x00400010, 0x20004010},

    {0x00200000, 0x04200002, 0x04000802, 0x00000000,
     0x00000800, 0x04000802, 0x00200802, 0x04200800,
     0x04200802, 0x00200000, 0x00000000, 0x04000002,
     0x00000002, 0x04000000, 0x04200002, 0x00000802,
     0x04000800, 0x00200802, 0x00200002, 0x04000800,
     0x04000002, 0x04200000, 0x04200800, 0x00200002,
     0x04200000, 0x00000800, 0x00000802, 0x04200802,
     0x00200800, 0x00000002, 0x04000000, 0x00200800,
     0x04000000, 0x00200800, 0x00200000, 0x04000802,
     0x04000802, 0x04200002, 0x04200002, 0x00000002,
     0x00200002, 0x04000000, 0x04000800, 0x00200000,
     0x04200800, 0x00000802, 0x00200802, 0x04200800,
     0x00000802, 0x04000002, 0x04200802, 0x04200000,
     0x00200800, 0x00000000, 0x00000002, 0x04200802,
     0x00000000, 0x00200802, 0x04200000, 0x00000800,
     0x04000002, 0x04000800, 0x00000800, 0x00200002},

    {0x10001040, 0x00001000, 0x00040000, 0x10041040,
     0x10000000, 0x10001040, 0x00000040, 0x10000000,
     0x00040040, 0x10040000, 0x10041040, 0x00041000,
     0x10041000, 0x00041040, 0x00001000, 0x00000040,
     0x10040000, 0x10000040, 0x10001000, 0x00001040,
     0x00041000, 0x00040040, 0x10040040, 0x10041000,
     0x00001040, 0x00000000, 0x00000000, 0x10040040,
     0x10000040, 0x10001000, 0x00041040, 0x00040000,
     0x00041040, 0x00040000, 0x10041000, 0x00001000,
     0x00000040, 0x10040040, 0x00001000, 0x00041040,
     0x10001000, 0x00000040, 0x10000040, 0x10040000,
     0x10040040, 0x10000000, 0x00040000, 0x10001040,
     0x00000000, 0x10041040, 0x00040040, 0x10000040,
     0x10040000, 0x10001000, 0x10001040, 0x00000000,
     0x10041040, 0x00041000, 0x00041000, 0x00001040,
     0x00001040, 0x00040040, 0x10000000, 0x10041000}
};

/*-----------------------------------------------------------------------------
  设计：Linc 2006.04.12
  功能：初始化
  参数：
-------------------------------------------------------------------------------*/
void initEncryptDES(GKey64 key, GDESContext *context, bool encrypt)
{
    const static byte PC1[56] = {56, 48, 40, 32, 24, 16, 8,  0,  57, 49, 41, 33, 25, 17,
                    9,  1,  58, 50, 42, 34, 26, 18, 10, 2,  59, 51, 43, 35,
                    62, 54, 46, 38, 30, 22, 14, 6,  61, 53, 45, 37, 29, 21,
                    13, 5,  60, 52, 44, 36, 28, 20, 12, 4,  27, 19, 11, 3};

    const static byte PC2[48] = {13, 16, 10, 23, 0,  4,  2,  27, 14, 5,  20, 9,  22, 18, 11, 3,
                    25, 7,  15, 6,  26, 19, 12, 1,  40, 51, 30, 36, 46, 54, 29, 39,
                    50, 44, 32, 47, 43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31};

    const static byte CTotRot[16] = {1,  2,  4,  6,  8,  10, 12, 14,
                        15, 17, 19, 21, 23, 25, 27, 28};

    const static byte CBitMask[8] = {128, 64, 32, 16, 8, 4, 2, 1};

    byte PC1M[56] = {0};
    byte PC1R[56] = {0};
    byte KS[8] = {0};

    /*convert PC1 to bits of key*/
    for (int j = 0; j <= 55; j++)
    {
        const int c_L = PC1[j];
        const int c_M = c_L % 8;
        PC1M[j] = int((key[c_L / 8] & CBitMask[c_M]) != 0);
    }

    /*key chunk for each iteration*/
    for (int i = 0; i <= 15; i++)
    {
        int nL = 0;
        /*rotate PC1 the right amount*/
        for (int j = 0; j <= 27; j++)
        {
            nL = j + CTotRot[i];
            if (nL < 28)
            {
                PC1R[j] = PC1M[nL];
                PC1R[j + 28] = PC1M[nL + 28];
            }
            else
            {
                PC1R[j] = PC1M[nL - 28];
                PC1R[j + 28] = PC1M[nL];
            }
        }

        /*select bits individually*/
        memset((void*) KS, 0, sizeof(KS));
        for (int j = 0; j <= 47; j++)
        {
            if (0 != PC1R[PC2[j]])
            {
                nL = j / 6;
                KS[nL] = KS[nL] | CBitMask[j % 6] >> 2;
            }
        }
        /*now convert to odd/even interleaved form for use in F*/
        if (encrypt)
        {
            context->transformedKey[i * 2]
                    = (int(KS[0]) << 24) | (int(KS[2]) << 16) | (int(KS[4]) << 8) | (int(KS[6]));
            context->transformedKey[i * 2 + 1]
                    = (int(KS[1]) << 24) | (int(KS[3]) << 16) | (int(KS[5]) << 8) | (int(KS[7]));
        }
        else
        {
            context->transformedKey[31 - (i * 2 + 1)]
                    = (int(KS[0]) << 24) | (int(KS[2]) << 16) | (int(KS[4]) << 8) | (int(KS[6]));
            context->transformedKey[31 - (i * 2)]
                    = (int(KS[1]) << 24) | (int(KS[3]) << 16) | (int(KS[5]) << 8) | (int(KS[7]));
        }
    }
    context->encrypt = encrypt;
}

/*-----------------------------------------------------------------------------
  设计：Linc 2006.06.15
  功能：初始化 3DES
  参数：
-------------------------------------------------------------------------------*/
void initEncryptTripleDES(const GKey128 key, GDESContext *context, bool encrypt)
{
    unsigned char keyArray[2][8]; // = {0};
    for (int i = 0; i < 8; i++)
    {
        keyArray[0][i] = key[i];
    }
    for (int i = 0; i < 8; i++)
    {
        keyArray[1][i] = key[i + 8];
    }
    if (encrypt)
    {
        initEncryptDES(keyArray[0], &context[0], true);
        initEncryptDES(keyArray[1], &context[1], false);
    }
    else
    {
        initEncryptDES(keyArray[0], &context[0], false);
        initEncryptDES(keyArray[1], &context[1], true);
    }
}

/*-----------------------------------------------------------------------------
  设计：Linc 2006.06.15
  功能：加密 3DES
  参数：
-------------------------------------------------------------------------------*/
void encryptTripleDES(GDESContext *context, unsigned char *block)
{
    encryptDES(context[0], block);
    encryptDES(context[1], block);
    encryptDES(context[0], block);
}

void splitBlock(const unsigned char *block, unsigned long &L, unsigned long &R)
{
    L = (unsigned int)(block[0] << 24) | (block[1] << 16) | (block[2] << 8) | (block[3]);
    R = (unsigned int)(block[4] << 24) | (block[5] << 16) | (block[6] << 8) | (block[7]);
}

void joinBlock(const int L, const int R, unsigned char *block)
{
    block[0] = (unsigned int)(R >> 24) & 0xFF;
    block[1] = (unsigned int)(R >> 16) & 0xFF;
    block[2] = (unsigned int)(R >> 8) & 0xFF;
    block[3] = (unsigned int)R & 0xFF;
    block[4] = (unsigned int)(L >> 24) & 0xFF;
    block[5] = (unsigned int)(L >> 16) & 0xFF;
    block[6] = (unsigned int)(L >> 8) & 0xFF;
    block[7] = (unsigned int)L & 0xFF;
}

void iPerm(unsigned long &L, unsigned long &R)
{
    unsigned long uWork(0);
    uWork = ((L >> 4) ^ R) & 0x0F0F0F0F;
    R = R ^ uWork;
    L = L ^ uWork << 4;
    uWork = ((L >> 16) ^ R) & 0x0000FFFF;
    R = R ^ uWork;
    L = L ^ uWork << 16;
    uWork = ((R >> 2) ^ L) & 0x33333333;
    L = L ^ uWork;
    R = R ^ uWork << 2;
    uWork = ((R >> 8) ^ L) & 0x00FF00FF;
    L = L ^ uWork;
    R = R ^ uWork << 8;
    R = (unsigned int)(R << 1) | (unsigned int)(R >> 31);
    uWork = (L ^ R) & 0xAAAAAAAA;
    L = L ^ uWork;
    R = R ^ uWork;
    L = (L << 1) | (L >> 31);
}

void fPerm(unsigned long &L, unsigned long &R)
{
    R = (unsigned int)(R << 31) | (R >> 1);
    unsigned long uWork = (unsigned int)(L ^ R) & 0xAAAAAAAA;
    L = (unsigned int)L ^ uWork;
    R = (unsigned int)R ^ uWork;
    L = (L >> 1) | (unsigned int)(L << 31);
    uWork = ((L >> 8) ^ R) & 0x00FF00FF;
    R = R ^ uWork;
    L = L ^ uWork << 8;
    uWork = ((L >> 2) ^ R) & 0x33333333;
    R = R ^ uWork;
    L = L ^ uWork << 2;
    uWork = ((unsigned int)(R >> 16) ^ L) & 0x0000FFFF;
    L = L ^ uWork;
    R = R ^ (unsigned int)(uWork << 16);
    uWork = ((R >> 4) ^ L) & 0x0F0F0F0F;
    L = L ^ uWork;
    R = (unsigned int)R ^ uWork << 4;
}

void encryptDES(GDESContext &context, unsigned char *block)
{
    unsigned long uLeft = 0;
    unsigned long uRight = 0;
    splitBlock(block, uLeft, uRight);
    iPerm(uLeft, uRight);
    unsigned long *cPtr = context.transformedKey;
    for (unsigned i = 0; i <= 7; ++i)
    {
        unsigned long uWork = (unsigned int)((uRight >> 4) | (unsigned int)(uRight << 28)) ^ (*cPtr);
        cPtr++;
        uLeft = (unsigned int)(uLeft) ^ c_PBox[6][uWork & 0x3F];
        uLeft = (unsigned int)uLeft ^ c_PBox[4][uWork >> 8 & 0x3F];
        uLeft = (unsigned int)uLeft ^ c_PBox[2][uWork >> 16 & 0x3F];
        uLeft = (unsigned int)uLeft ^ c_PBox[0][uWork >> 24 & 0x3F];
        uWork = ((unsigned int)uRight ^ (*cPtr));
        cPtr++;
        uLeft = uLeft ^ c_PBox[7][uWork & 0x3F];
        uLeft = uLeft ^ c_PBox[5][uWork >> 8 & 0x3F];
        uLeft = uLeft ^ c_PBox[3][uWork >> 16 & 0x3F];
        uLeft = uLeft ^ c_PBox[1][uWork >> 24 & 0x3F];
        uWork = (((uLeft >> 4) | (unsigned int)(uLeft << 28)) ^ (*cPtr));
        cPtr++;
        uRight = uRight ^ c_PBox[6][uWork & 0x3F];
        uRight = uRight ^ c_PBox[4][uWork >> 8 & 0x3F];
        uRight = uRight ^ c_PBox[2][uWork >> 16 & 0x3F];
        uRight = uRight ^ c_PBox[0][uWork >> 24 & 0x3F];
        uWork = (uLeft ^ (*cPtr));
        cPtr++;
        uRight = uRight ^ c_PBox[7][uWork & 0x3F];
        uRight = uRight ^ c_PBox[5][uWork >> 8 & 0x3F];
        uRight = uRight ^ c_PBox[3][uWork >> 16 & 0x3F];
        uRight = uRight ^ c_PBox[1][uWork >> 24 & 0x3F];
    }
    fPerm(uLeft, uRight);
    joinBlock(uLeft, uRight, block);
}

///////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.06.15
//功能：DES 加密
//参数：
//注意：只处理长度是 8 倍数的
////////////////////////////////////////////////////////////////////////////////
//unsigned char* DES(const unsigned char* inString, int inSize, const unsigned char* key64, bool encrypt)
//{
//    assert((inSize / 8) == 0);
//    unsigned char* result = new unsigned char[inSize + 1];
//    memcpy(result, inString, inSize);
//    result[inSize] = '\0';
//    DES(result, inSize, key64, encrypt);
//    return result;
//}

///////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.06.15
//功能：DES 加密
//参数：
//注意：只处理长度是 8 倍数的
////////////////////////////////////////////////////////////////////////////////
unsigned char* DES(const unsigned char* inString, int inSize, GKey64 key64, bool encrypt)
{
    assert((inSize % 8) == 0);
    if (inSize == 0)
    {
        return NULL;
    }
    unsigned char* buffer = new unsigned char[inSize + 1];
    memcpy(buffer, inString, inSize);
    buffer[inSize] = '\0';
    unsigned char* temp = buffer;

    GDESContext context;
    initEncryptDES(key64, &context, encrypt);

    unsigned long uBlockCount = inSize >> 3;
    while (uBlockCount > 0)
    {
        encryptDES(context, buffer);
        buffer += 8;
        uBlockCount--;
    }
    return temp;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.04.12
//功能：DES 处理字符
//参数：
////////////////////////////////////////////////////////////////////////////////
unsigned char* DESEncrypt(const unsigned char* inString, int inSize, GKey64 key64, bool encrypt)
{
    GBlockMemoryStream inStream;
    inStream.write((const char*)inString, inSize);
    inStream.seek(0);
    GBlockMemoryStream outStream;
    DESEncryptStream(&inStream, &outStream, key64, encrypt);
    outStream.seek(0);
    unsigned char* result = new unsigned char[outStream.size() + 1];
    outStream.read((char*)result, outStream.size());
    result[outStream.size()] = '\0';
    return result;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.04.12
//功能：DES 处理数据流
//参数：
////////////////////////////////////////////////////////////////////////////////
void DESEncryptStream(GStream *inStream, GStream *outStream, GKey64 key64, bool encrypt)
{
    unsigned long uIterator = 0;
    unsigned char block[BLOCK_SIZE + 1];
    memset(block, '\0', BLOCK_SIZE + 1);
    GDESContext context;

    initEncryptDES(key64, &context, encrypt);

    unsigned long uBlockCount = inStream->size() / BLOCK_SIZE;

    if (encrypt)
    {
        uBlockCount++;
    }

    for (uIterator = 1; uIterator <= uBlockCount - 1; ++uIterator)
    {
        if (inStream->read((char*)block, BLOCK_SIZE) != BLOCK_SIZE)
        {
            throw GLDException("");
        }
        encryptDES(context, block);
        outStream->write((char*)block, BLOCK_SIZE);
    }

    if (encrypt)
    {
        memset(block, '\0', BLOCK_SIZE + 1);

        uIterator = (inStream->size() % BLOCK_SIZE);
        if ((unsigned long)inStream->read((char*)block, uIterator) != uIterator)
        {
            throw GLDException("");
        }

        block[BLOCK_SIZE - 1] = uIterator;

        encryptDES(context, block);
        outStream->write((char*)block, BLOCK_SIZE);
    }
    else
    {
        if (inStream->read((char*)block, BLOCK_SIZE) != BLOCK_SIZE)
        {
            throw GLDException("");
        }
        encryptDES(context, block);

        uIterator = block[BLOCK_SIZE - 1];

        outStream->write((char*)block, uIterator);
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.06.15
//功能：DES 加密
//参数：
//注意：只处理长度是 8 倍数的
////////////////////////////////////////////////////////////////////////////////
unsigned char* tripleDES(unsigned char* inString, int inSize, const GKey128 key128, bool encrypt)
{
    if ((inString == NULL) || (inSize == 0))
        return NULL;
    unsigned char* buffer = new unsigned char[inSize + 1];
    memcpy(buffer, inString, inSize);
    buffer[inSize] = '\0';
    unsigned char* temp = buffer;

    GDESContext context[2];

    assert((inSize & 0x7) == 0);

    initEncryptTripleDES(key128, context, encrypt);
    int nBlockCount = inSize >> 3;
    while (nBlockCount > 0)
    {
        encryptTripleDES(context, buffer);
        buffer += 8;
        nBlockCount--;
    }
    return temp;
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.04.12
//功能：3DES 处理字符
//参数：
////////////////////////////////////////////////////////////////////////////////
unsigned char* tripleDESEncrypt(const unsigned char* inString, int inSize, GKey128 key128, bool encrypt)
{
    GBlockMemoryStream inStream;
    inStream.write((const char*)inString, inSize);
    inStream.seek(0);
    GBlockMemoryStream outStream;
    tripleDESEncryptStream(&inStream, &outStream, key128, encrypt);
    outStream.seek(0);
    unsigned char* result = new unsigned char[outStream.size() + 1];
    outStream.read((char*)result, outStream.size());
    result[outStream.size()] = '\0';
    return result;
}

/*-----------------------------------------------------------------------------
  设计：Linc 2006.04.12
  功能：3DES 处理数据流
  参数：
-------------------------------------------------------------------------------*/
void tripleDESEncryptStream(GStream *inStream, GStream *outStream, const GKey128 key128, bool encrypt)
{
    unsigned char block[BLOCK_SIZE + 1];
    memset(block, '\0', BLOCK_SIZE + 1);
    GDESContext context[2];
    initEncryptTripleDES(key128, context, encrypt);

    /*get the number of blocks in the file*/
    int nBlockCount = ((inStream->size() - inStream->pos()) / BLOCK_SIZE);

    /*when encrypting, make sure we have a block with at least one free*/
    /*byte at the end. used to store the odd byte count value*/
    if (encrypt)
    {
        nBlockCount++;
    }

    /*process all except the last block*/
    for (int i = 1; i != nBlockCount; i++)
    {
        if (inStream->read((char *)block, BLOCK_SIZE) != BLOCK_SIZE)
        {
            throw GLDException("");
        }
        encryptTripleDES(context, block);
        outStream->write((char *)block, BLOCK_SIZE);
    }
    if (encrypt)
    {
        memset(block, '\0', BLOCK_SIZE + 1);
        /*set actual number of bytes to read*/
        unsigned int uActual = (inStream->size() % BLOCK_SIZE);
        if (inStream->read((char *)block, uActual) != uActual)
        {
            throw GLDException("");
        }

        /*store number of last(bytes) byte in last block*/

        //        byte (*pByteArray)[32768] = &block;
        //        (*pByteArray)[BLOCK_SIZE - 1] = i;
        block[BLOCK_SIZE - 1] = uActual;

        /*encrypt and save full block*/
        encryptTripleDES(context, block);
        outStream->write((char *)block, BLOCK_SIZE);
    }
    else
    {
        /*encrypted file is always a multiple of the block size*/
        if (inStream->read((char *)block, BLOCK_SIZE) != BLOCK_SIZE)
        {
            throw GLDException("");
        }
        encryptTripleDES(context, block);

        /*get actual number of bytes encoded*/
        unsigned int uLast = block[BLOCK_SIZE - 1];

        /*save valid portion of block*/
        outStream->write((char *)block, uLast);
    }
}

////////////////////////////////////////////////////////////////////////////////
//设计：Linc 2006.07.07
//功能：重叠 DES 加密算法
//参数：
//注意：要求长度必须 >= 8 且长度为 8 的倍数时和 DES 处理结果相同
////////////////////////////////////////////////////////////////////////////////
void overlapDES(unsigned char* inString, int inSize, GKey64 key64, bool encrypt)
{
    unsigned char* pData = inString;

    if (!inString || (inSize < 8))
    {
        return;
    }

    int nCount = inSize / 8;
    int nDelta = inSize % 8;

    if ((!encrypt) && (nDelta != 0))
    {
        pData += inSize - 8;
        unsigned char* pResult = DES(pData, 8, key64, encrypt);
        for (int j = 0; j < 8; ++j)
        {
            pData[j] = pResult[j];
        }
        freeAndNil(pResult);
        pData = inString;
    }
    for (int i = 0; i < nCount; ++i)
    {
        unsigned char* pResult = DES(pData, 8, key64, encrypt);
        for (int j = 0; j < 8; ++j)
        {
            pData[j] = pResult[j];
        }
        freeAndNil(pResult);

        pData += 8;
    }
    if (encrypt && (nDelta != 0))
    {
        pData = inString;
        pData += inSize - 8;
        unsigned char* pResult = DES(pData, 8, key64, encrypt);
        for (int j = 0; j < 8; ++j)
        {
            pData[j] = pResult[j];
        }
        freeAndNil(pResult);
    }
}
