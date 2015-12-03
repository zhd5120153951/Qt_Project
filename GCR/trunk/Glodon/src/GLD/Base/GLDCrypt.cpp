/*!
* @加解密库类
* @ 详细: 包括加解密类基类及子类AES和子类DES及方法;
*                 加解密算法策略类及方法
* @author lijb
* @date 2015.06.05
* @remarks 备注
* Copyright (c) 1998-2015 Glodon Corporation                                                    
*/

//cryptlib库宏定义，以静态lib方式引入cryptlib.lib
#define _CRT_SECURE_NO_DEPRECATE
#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "dll.h"

using namespace CryptoPP;
using namespace std;

#include "qglobal.h"
#include "GLDCrypt.h"
#include "GLDFile.h"

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 加解密算法类私类
//参数：
////////////////////////////////////////////////////////////////////////////////
class GCryptAlgorithmPrivate
{
public:
  GCryptAlgorithmPrivate()
  {
  }

protected:
  GCryptAlgorithm * q_ptr;
  byte * m_pbyKey;	  //密钥
  byte * m_pbyIv; 	  //初始向量
  int    m_nKeyLength; //密钥长度
  EnAlgorithmType m_EnAlgorithmType;  //算法类型，如AES , DES等
  EnEncodeMode  m_EnEncodeMode;      //编码模式，如CBC, ECB等
  EnPaddingType m_EnPaddingType;	  //填充类型，如NoPadding，PKCS5Padding等

private:
  Q_DECLARE_PUBLIC(GCryptAlgorithm);
};

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 加解密算法类构造函数，配置加密参数
//参数：enAlgorithmType  加密算法类型，如AES, DES等
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//     EnPaddingType     填充模式类型，如NoPadding，PKCS5Padding等
////////////////////////////////////////////////////////////////////////////////
GCryptAlgorithm::GCryptAlgorithm(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeKind
                               , EnPaddingType EnPaddingKind)
    : d_ptr(new GCryptAlgorithmPrivate)
{
    Q_D(GCryptAlgorithm);

    d->q_ptr = this;

    d->m_EnAlgorithmType  = EnAlgorithmKind;
    d->m_EnEncodeMode     = EnEncodeKind;
    d->m_EnPaddingType    = EnPaddingKind;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 加解密算法类构造函数，配置加密参数
//参数：enAlgorithmType  加密算法类型，如AES, DES等
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//     EnPaddingType     填充模式类型，如NoPadding，PKCS5Padding等
////////////////////////////////////////////////////////////////////////////////
GCryptAlgorithm::GCryptAlgorithm(GCryptAlgorithmPrivate &dd, EnAlgorithmType EnAlgorithmKind
               , EnEncodeMode EnEncodeType, EnPaddingType EnPadding)
    : d_ptr(&dd)
{
    Q_D(GCryptAlgorithm);

    d->q_ptr = this;

    d->m_EnAlgorithmType  = EnAlgorithmKind;
    d->m_EnEncodeMode     = EnEncodeType;
    d->m_EnPaddingType    = EnPadding;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 加解密算法类析造函数
////////////////////////////////////////////////////////////////////////////////
GCryptAlgorithm::~GCryptAlgorithm()
{
    Q_D(GCryptAlgorithm);

    delete(d);
}
////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 用密钥加密明文字符串，返回加密后的密文字符串
//参数: strPlainText  明文字符串     
//返回:  密文字符串
////////////////////////////////////////////////////////////////////////////////
const GString GCryptAlgorithm::encryptStr(const GString &strPlainText)
{
    Q_D(GCryptAlgorithm);

    string strCipher;

    // 生成加密器对象
    SymmetricCipher *penncryptor = encryptor(d->m_EnEncodeMode);

    StringSource(strPlainText.toLatin1().data(), true, new StreamTransformationFilter(*penncryptor, new StringSink(strCipher)
                  , (StreamTransformationFilter::BlockPaddingScheme)d->m_EnPaddingType));

    //加密密文由string转换为GString
    return QString(QLatin1String(strCipher.c_str()));
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 用密钥解密密文字符串，返回解密后的明文字符串
//参数: strPlainText  明文字符串     
//返回:  明文字符串
////////////////////////////////////////////////////////////////////////////////
const GString GCryptAlgorithm::decryptStr(const GString &strCipher)
{
    Q_D(GCryptAlgorithm);

    string strCipherString = strCipher.toStdString();

    string strRecover;

	// 生成解密器对象
    SymmetricCipher* pdecryptor = decryptor(d->m_EnEncodeMode);
	
    StringSource(strCipher.toLatin1().data(), true, new StreamTransformationFilter(*pdecryptor, new StringSink(strRecover)
                 , (StreamTransformationFilter::BlockPaddingScheme)d->m_EnPaddingType));

    return QString(QLatin1String(strRecover.c_str()));
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 用密钥加密明文文件，返回加密后的密文文件
//参数: inFilename  明文文件名
//            outFilename 密文文件名
//返回: 成功or  失败
////////////////////////////////////////////////////////////////////////////////
bool GCryptAlgorithm::encryptFile(const GString &strOutFilename, const GString &strInFilename)
{
    Q_D(GCryptAlgorithm);

    if (!GFileStream::exists(strInFilename))
    {
        cout << "The file " << strInFilename.data() << " is not exist! " << endl;
        return false;
    }
	
    // 生成加密器对象
    SymmetricCipher* penncryptor = encryptor(d->m_EnEncodeMode);
		
    FileSource(strInFilename.toLatin1().data(), true,
            new StreamTransformationFilter(*penncryptor, new FileSink(strOutFilename.toLatin1().data())
             , (StreamTransformationFilter::BlockPaddingScheme)d->m_EnPaddingType));

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 用密钥解密密文文件，返回解密后的名文文件
//参数: decFilename  密文文件名
//            recoverFilename 名文文件名
//返回: 成功or  失败
////////////////////////////////////////////////////////////////////////////////
bool GCryptAlgorithm::decryptFile(const GString &strRecoverFilename, const GString &strDecFilename)
{
    Q_D(GCryptAlgorithm);

    if (!GFileStream::exists(strDecFilename))
    {
        cout << "The file " << strDecFilename.data() << " is not exist! " << endl;
        return false;
    }
	
	// 生成解密器对象
    SymmetricCipher* pdecryptor = decryptor(d->m_EnEncodeMode);
		
    FileSource(strDecFilename.toLatin1().data(), true, new StreamTransformationFilter((*pdecryptor),
         new FileSink(strRecoverFilename.toLatin1().data()), (StreamTransformationFilter::BlockPaddingScheme)d->m_EnPaddingType));

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据用户参数加密模式EnEncodeType，返回对应的字符串
//             ，缺省返回"CBC"
//参数： EnEncodeMode      加密模式类型，如CBC, ECB等
//    
//返回：加密模式对应的字符串    
////////////////////////////////////////////////////////////////////////////////
GString GCryptAlgorithm::encodeModeStr(EnEncodeMode EnEncodeMode)
{
	switch (EnEncodeMode)
	{
		case ENCODE_MODE_CBC:
		{
			return "CBC";
			break;
		}
	
		case ENCODE_MODE_ECB:
		{
			return "ECB";
			break;
		}
		
		default:
			assert(false);
	}

    return "ECB";
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据用户参数填充模式EnPaddingType，返回对应的字符串，缺省返回"PKCS5Padding"
//参数： EnPaddingType      加密填充类型，如EN_NOPADDING等
//    
//返回：加密填充模式对应的字符串    
////////////////////////////////////////////////////////////////////////////////
GString GCryptAlgorithm::paddingTypeStr(EnPaddingType EnPaddingType)
{
	switch (EnPaddingType)
	{
		case EN_NOPADDING:
		{
			return "NoPadding";
			break;
		}

		case EN_PKCS_PADDING:
		{
			return "PKCS5Padding";
			break;
		}
		
		default:
			assert(false);
	}

	return "PKCS5Padding";
	
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能:  设置加密参数，包括加密算法EnAlgorithmKind，加密模式EnEncodeType，填充类型EnPadding
//参数:  EnAlgorithmKind    加密算法类型，如AES, DES等
//              EnEncodeMode      加密模式类型，如CBC, ECB等
//              EnPaddingType      填充模式类型，如NoPadding，PKCS5Padding等?
//返回：无  
////////////////////////////////////////////////////////////////////////////////
void GCryptAlgorithm::setEncodeMode(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType)
{
    Q_D(GCryptAlgorithm);

    d->m_EnAlgorithmType  = EnAlgorithmKind;
    d->m_EnEncodeMode     = EnEncodeMode;
    d->m_EnPaddingType    = EnPaddingType;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能:  设置秘钥及秘钥长度，非ECB模式
//参数： pbyKey   加密秘钥
//                 nLength  秘钥长度
//                 pbyIv     初始向量
//返回：无  
////////////////////////////////////////////////////////////////////////////////
void GCryptAlgorithm::setKey(byte *pbyKey, int nLength, byte *pbyIv)
{
   Q_D(GCryptAlgorithm);

   d->m_pbyKey = pbyKey;
   d->m_pbyIv  = pbyIv;
   d->m_nKeyLength = nLength;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能:  设置秘钥及秘钥长度，ECB模式
//参数： pbyKey   加密秘钥
//                 nLength  秘钥长度
//返回：无    
////////////////////////////////////////////////////////////////////////////////
void GCryptAlgorithm::setKey(byte *pbyKey, int nLength)
{
   Q_D(GCryptAlgorithm);

   d->m_pbyKey = pbyKey;
   d->m_nKeyLength = nLength;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能:  获取秘钥
//参数：                 
//返回：秘钥字节指针    
////////////////////////////////////////////////////////////////////////////////
byte* GCryptAlgorithm::key()
{
    Q_D(GCryptAlgorithm);

    return d->m_pbyKey;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能:  获取初始向量
//参数：                 
//返回：秘钥字节指针    
////////////////////////////////////////////////////////////////////////////////
byte* GCryptAlgorithm::ivKey()
{
    Q_D(GCryptAlgorithm);

    return d->m_pbyIv;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: AES加解密算法私类构造函数
////////////////////////////////////////////////////////////////////////////////
class GAesAlgorithmPrivate : public GCryptAlgorithmPrivate
{
public:
  GAesAlgorithmPrivate()
  {
  }

private:
  Q_DECLARE_PUBLIC(GAesAlgorithm);

  SymmetricCipher *m_encryptor;  //加密器对象，根据加密模式生成不同对象
  SymmetricCipher *m_decryptor;  //解密器对象，根据加密模式生成不同对象
};

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: AES加解密算法类构造函数，配置加密参数
//参数：enAlgorithmType  加密算法类型，如AES, DES等
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//     EnPaddingType     填充模式类型，如NoPadding，PKCS5Padding等
////////////////////////////////////////////////////////////////////////////////
GAesAlgorithm::GAesAlgorithm(EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType)
    : GCryptAlgorithm(*(new GAesAlgorithmPrivate), EN_ALGORITHM_AES, EnEncodeMode, EnPaddingType)
{
    Q_D(GAesAlgorithm);

    d->m_encryptor = NULL;
    d->m_decryptor = NULL;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.8
//功能: AES加解密算法类析构函数，释放加解密器资源
//参数：   
////////////////////////////////////////////////////////////////////////////////
GAesAlgorithm::~GAesAlgorithm()
{
    Q_D(GAesAlgorithm);

    if (NULL != d->m_encryptor)
	{
        delete d->m_encryptor;
	}

    if (NULL != d->m_decryptor)
	{
        delete d->m_decryptor;
	}
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.8
//功能: 根据内部生成随机的加密密钥并设置
//参数：   
////////////////////////////////////////////////////////////////////////////////
void GAesAlgorithm::setRandomKey()
{
    Q_D(GAesAlgorithm);

	AutoSeededRandomPool rnd;
	byte byKey[AES::DEFAULT_KEYLENGTH];
	rnd.GenerateBlock(byKey, AES::DEFAULT_KEYLENGTH);

    if (ENCODE_MODE_CBC != d->m_EnEncodeMode)
    {
		// Generate a random IV
		byte byIv[AES::BLOCKSIZE];
		rnd.GenerateBlock(byIv, AES::BLOCKSIZE);
		
		setKey(byKey, AES::DEFAULT_KEYLENGTH, byIv);	
	}
	else
	{		
		setKey(byKey, AES::DEFAULT_KEYLENGTH);	
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据用户参数加密模式EnEncodeType，填充类型EnPadding等 生成相应的解密器
//参数：
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//返回:  解密器对象指针
//////////////////////////////////////////////////////////////////////////////////////////////////////////
SymmetricCipher* GAesAlgorithm::decryptor(EnEncodeMode EnEncodeMode)
{
    Q_D(GAesAlgorithm);

	SymmetricCipher* pdecryptor = NULL;
	
	switch (EnEncodeMode)
	{
		case ENCODE_MODE_CBC:
		{
            pdecryptor = new CBC_Mode<AES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}

		case ENCODE_MODE_CFB:
		{
            pdecryptor = new CFB_Mode<AES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
		
		case ENCODE_MODE_ECB:
		{
            pdecryptor = new ECB_Mode<AES>::Decryption(d->m_pbyKey, d->m_nKeyLength);
			break;
		}	
		
		case ENCODE_MODE_OFB:
		{
            pdecryptor = new OFB_Mode<AES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
				
		default:
			assert(false);
	}
	
    d->m_decryptor = pdecryptor;

	return pdecryptor;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据用户参数加密模式EnEncodeType，填充类型EnPadding等 生成相应的加密器
//参数：
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//返回:  加密器对象指针
//////////////////////////////////////////////////////////////////////////////////////////////////////////
SymmetricCipher* GAesAlgorithm::encryptor(EnEncodeMode EnEncodeMode)
{
    Q_D(GAesAlgorithm);

	SymmetricCipher* pencryptor = NULL;

	switch (EnEncodeMode)
	{
		case ENCODE_MODE_CBC:
		{
            pencryptor = new CBC_Mode<AES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}

		case ENCODE_MODE_CFB:
		{
            pencryptor = new CFB_Mode<AES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
		
		case ENCODE_MODE_ECB:
		{
            pencryptor = new ECB_Mode<AES>::Encryption(d->m_pbyKey, d->m_nKeyLength);
			break;
		}	
		
		case ENCODE_MODE_OFB:
		{
            pencryptor = new OFB_Mode<AES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
				
		default:
			assert(false);
	}
	
    d->m_encryptor = pencryptor;
	
	return pencryptor;

}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.8
//功能: DES加解密算法私类
////////////////////////////////////////////////////////////////////////////////
class GDesAlgorithmPrivate : public GCryptAlgorithmPrivate
{
public:
  GDesAlgorithmPrivate()
  {
  }

private:
  Q_DECLARE_PUBLIC(GDesAlgorithm);

  SymmetricCipher *m_encryptor;  //加密器对象，根据加密模式生成不同对象
  SymmetricCipher *m_decryptor;  //解密器对象，根据加密模式生成不同对象
};

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.8
//功能: DES加解密算法类构造函数，配置加密参数
//参数：
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//     EnPaddingType     填充模式类型，如NoPadding，PKCS5Padding等
////////////////////////////////////////////////////////////////////////////////
GDesAlgorithm::GDesAlgorithm(EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType)
    : GCryptAlgorithm(* new GDesAlgorithmPrivate(), EN_ALGORITHM_DES, EnEncodeMode, EnPaddingType)
{	
    Q_D(GDesAlgorithm);

    d->m_encryptor = NULL;
    d->m_decryptor = NULL;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.8
//功能: DES加解密算法类析构函数，释放加解密器资源
//参数：   
////////////////////////////////////////////////////////////////////////////////
GDesAlgorithm::~GDesAlgorithm()
{
    Q_D(GDesAlgorithm);

    if (NULL != d->m_encryptor)
	{
        delete d->m_encryptor;
	}

    if (NULL != d->m_decryptor)
	{
        delete d->m_decryptor;
	}
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.8
//功能: 根据内部生成随机的加密密钥并设置
//参数：   
////////////////////////////////////////////////////////////////////////////////
void GDesAlgorithm::setRandomKey()
{
    Q_D(GDesAlgorithm);

	AutoSeededRandomPool rnd;
	byte byKey[DES::DEFAULT_KEYLENGTH];
	rnd.GenerateBlock(byKey, DES::DEFAULT_KEYLENGTH);

    if (ENCODE_MODE_CBC != d->m_EnEncodeMode)
    {
		// Generate a random IV
		byte byIv[AES::BLOCKSIZE];
		rnd.GenerateBlock(byIv, DES::BLOCKSIZE);
		
		setKey(byKey, DES::DEFAULT_KEYLENGTH, byIv);	

	}
	else
	{		
		setKey(byKey, DES::DEFAULT_KEYLENGTH);	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.9
//功能: 根据用户参数加密模式EnEncodeType，填充类型EnPadding等 生成相应的解密器
//参数：
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//返回:  解密器对象指针
//////////////////////////////////////////////////////////////////////////////////////////////////////////
SymmetricCipher* GDesAlgorithm::decryptor(EnEncodeMode EnEncodeKind)
{
    Q_D(GDesAlgorithm);

	SymmetricCipher* decryptor = NULL;
	
    switch (EnEncodeKind)
	{
		case ENCODE_MODE_CBC:
		{
            decryptor = new CBC_Mode<DES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}

		case ENCODE_MODE_CFB:
		{
            decryptor = new CFB_Mode<DES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
		
		case ENCODE_MODE_ECB:
		{
            decryptor = new ECB_Mode<DES>::Decryption(d->m_pbyKey, d->m_nKeyLength);
			break;
		}	
		
		case ENCODE_MODE_OFB:
		{
            decryptor = new OFB_Mode<DES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
		
		//case ENCODE_MODE_PCBC:
		//{
            //decryptor = new PCBC_Mode<DES>::Decryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			//break;
		//}
					
		default:
			assert(false);
	}
	
    d->m_decryptor = decryptor;

	return decryptor;

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据用户参数加密模式EnEncodeType，填充类型EnPadding等 生成相应的加密器
//参数：
//     EnEncodeKind      加密模式类型，如CBC, ECB等
//返回:  加密器对象指针
//////////////////////////////////////////////////////////////////////////////////////////////////////////
SymmetricCipher* GDesAlgorithm::encryptor(EnEncodeMode EnEncodeKind)
{
    Q_D(GDesAlgorithm);

	SymmetricCipher* encryptor = NULL;
	
    switch (EnEncodeKind)
	{
		case ENCODE_MODE_CBC:
		{
            encryptor = new CBC_Mode<DES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}

		case ENCODE_MODE_CFB:
		{
            encryptor = new CFB_Mode<DES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
		
		case ENCODE_MODE_ECB:
		{
            encryptor = new ECB_Mode<DES>::Encryption(d->m_pbyKey, d->m_nKeyLength);
			break;
		}	
		
		case ENCODE_MODE_OFB:
		{
            encryptor = new OFB_Mode<DES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			break;
		}
		
		//case ENCODE_MODE_PCBC:
		//{
            //cryptAlgorithm = new PCBC_Mode<DES>::Encryption(d->m_pbyKey, d->m_nKeyLength, d->m_pbyIv);
			//break;
		//}
					
		default:
			assert(false);
	}
	
    d->m_encryptor = encryptor;
	
	return encryptor;

}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 加解密算法策略类私类
////////////////////////////////////////////////////////////////////////////////
class GCryptPrivate
{
public:
  GCryptPrivate(GCrypt *parent)
    : q_ptr(parent)
  {
  }

private:
  GCrypt * const q_ptr;
  Q_DECLARE_PUBLIC(GCrypt);

  GCryptAlgorithm *m_algorithm; //加密算法对象指针，如AES , DES等
};

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 加解密算法策略类构造函数，配置加密参数；
//             生成加解密算法类对象
//     EnAlgorithmKind  加密算法类型，如AES, DES等
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//     EnPaddingType     填充模式类型，如NoPadding，PKCS5Padding等
////////////////////////////////////////////////////////////////////////////////
GCrypt::GCrypt(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType)
    : d_ptr(new GCryptPrivate(this))
{
    Q_D(GCrypt);

    d->m_algorithm = createAlgorithm(EnAlgorithmKind, EnEncodeMode, EnPaddingType);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据用户传入的加密参数，生成对应的加解密类对象
//参数：enAlgorithmType  加密算法类型，如AES, DES等
//     EnEncodeMode      加密模式类型，如CBC, ECB等
//     EnPaddingType     填充模式类型，如NoPadding，PKCS5Padding?
//返回: 加解密类对象指针
////////////////////////////////////////////////////////////////////////////////
GCryptAlgorithm* GCrypt::createAlgorithm(EnAlgorithmType EnAlgorithmType, EnEncodeMode EnEncodeMode 
                                            ,EnPaddingType EnPaddingType)
{
	GCryptAlgorithm* cryptAlgorithm = NULL;

	switch (EnAlgorithmType)
	{
		case EN_ALGORITHM_AES:
		{
			cryptAlgorithm = new GAesAlgorithm(EnEncodeMode, EnPaddingType);
			break;
		}
		
		case EN_ALGORITHM_DES:
		{
			cryptAlgorithm = new GDesAlgorithm(EnEncodeMode, EnPaddingType);
			break;
		}
		
		default:
			assert(false);
	}

	return cryptAlgorithm;

}

GCrypt::~GCrypt()
{
    Q_D(GCrypt);

    if (NULL != d->m_algorithm)
    {
        delete d->m_algorithm;
	}

    delete d;
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 使用密钥加密明文字符串plainText 并且返回密文
//参数: strPlainText  加密原文字符串
//返回: 加类密文字符串对象
////////////////////////////////////////////////////////////////////////////////

GString GCrypt::encryptStr(const GString &strPlainText)
{
    Q_D(GCrypt);

    return  d->m_algorithm->encryptStr(strPlainText);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 使用密钥解密密文cipher 并且返回恢复后的明文
//参数: strCipher  加密密文字符串
//返回: 返回恢复后的明文
////////////////////////////////////////////////////////////////////////////////
GString GCrypt::decryptStr(const GString &strCipher)
{
    Q_D(GCrypt);

    return  d->m_algorithm->decryptStr(strCipher);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 使用密钥加密明文文件inFilename 并且返回密文文件outFilename
//参数: inFilename  明文文件
//     outFilename 密文文件
//返回: 成功与否
////////////////////////////////////////////////////////////////////////////////
bool GCrypt::encryptFile(const GString &strOutFilename, const GString &strInFilename)
{
    Q_D(GCrypt);

    return  d->m_algorithm->encryptFile(strOutFilename, strInFilename);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 使用密钥解密密文文件DecFilename 并且返回恢复后的明文文件recoverFilename
//参数: strDecFilename 密文文件
//     strRecoverFilename 恢复后的明文文件recoverFilename
//返回: 成功与否
////////////////////////////////////////////////////////////////////////////////
bool GCrypt::decryptFile(const GString &strRecoverFilename, const GString &strDecFilename)
{
    Q_D(GCrypt);

    return  d->m_algorithm->decryptFile(strRecoverFilename, strDecFilename);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 设置加密参数，包括加密算法EnAlgorithmKind，加密模式EnEncodeType，填充类型EnPadding
//参数: EnAlgorithmKind 加密算法
//     EnEncodeType    加密模式
//     EnPadding       填充类型
//返回: 无
////////////////////////////////////////////////////////////////////////////////
void GCrypt::setEncodeMode(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeType, EnPaddingType EnPadding)
{
    Q_D(GCrypt);

    return  d->m_algorithm->setEncodeMode(EnAlgorithmKind, EnEncodeType, EnPadding);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 根据内部生成随机的加密密钥并设置
//返回: 无
////////////////////////////////////////////////////////////////////////////////
void GCrypt::setRandomKey()
{
    Q_D(GCrypt);

    return  d->m_algorithm->setRandomKey();
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 设置加密密钥及长度，此处需要重载，因为CBC模式不需要初始向量iv
//参数: key    秘钥字符串
//     length 秘钥长度
//     iv     初始向量
//返回: 无
////////////////////////////////////////////////////////////////////////////////
void GCrypt::setKey(byte *key, int length, byte *iv)
{
    Q_D(GCrypt);

    return  d->m_algorithm->setKey(key, length, iv);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 设置加密密钥及长度，此处需要重载，因为CBC模式不需要初始向量iv
//参数: key    秘钥字符串
//     length 秘钥长度
//返回: 无
////////////////////////////////////////////////////////////////////////////////
void GCrypt::setKey(byte *key, int length)
{
    Q_D(GCrypt);

    return  d->m_algorithm->setKey(key, length);
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 获取加密密钥
//参数: 无
//返回: 秘钥字符串
////////////////////////////////////////////////////////////////////////////////
byte * GCrypt::key()
{
    Q_D(GCrypt);

    return  d->m_algorithm->key();
}

////////////////////////////////////////////////////////////////////////////////
//设计: Lijb 2015.06.5
//功能: 获取加密初始向量
//参数: 无
//返回: 初始向量
////////////////////////////////////////////////////////////////////////////////
byte * GCrypt::ivKey()
{
    Q_D(GCrypt);

    return  d->m_algorithm->ivKey();
}
