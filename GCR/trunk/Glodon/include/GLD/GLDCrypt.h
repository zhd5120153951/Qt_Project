/*!
* @加解密库头文件
*
* @author lijb
* @date 2015.06.05
* @remarks 备注
* Copyright (c) 1998-2015 Glodon Corporation                                                    
*/
#ifndef GLDCRYPT_H
#define GLDCRYPT_H

#include "GLDCrypt_Global.h"
#include "GLDString.h"
#include "GLDCrypt_Global.h"
#include "cryptlib.h"

//class CRYPTOPP_DLL CRYPTOPP_NO_VTABLE SymmetricCipher;

//加解密算法枚举，目前支持AES, DES，可扩充
enum EnAlgorithmType
{
    EN_ALGORITHM_AES = 0,
    EN_ALGORITHM_DES = 1,
    EN_ALGORITHM_UNKOWN
};

//加解密算法加密模式枚举
enum EnEncodeMode
{
    ENCODE_MODE_CBC = 0,
    ENCODE_MODE_CFB = 1,
    ENCODE_MODE_ECB = 2,
    ENCODE_MODE_OFB = 3,
    ENCODE_MODE_PCBC = 4,
    ENCODE_MODE_UNKOWN
};

//加解密算法填充模式枚举
enum EnPaddingType
{
	//enum BlockPaddingScheme {NO_PADDING, ZEROS_PADDING, PKCS_PADDING, ONE_AND_ZEROS_PADDING, DEFAULT_PADDING};
	//下边枚举需要和cryptlib库一致
    EN_NOPADDING = 0,
    EN_ZEROS_PADDING = 1,
    EN_PKCS_PADDING = 2,
    EN_ONE_AND_ZEROS_PADDING = 3,
    EN_DEFAULT_PADDING = 4, 
    EN_PADDING_UNKOWN
};

class GCryptAlgorithmPrivate;
////////////////////////////////////////////////////////////////////////////////
//设计:创建一个加解密算法策略基类，提供相应虚接口，相关接口需要子类实现；
//     公共接口如encryptStr等封装到基类实现
//功能:算法基类，提供加解密串/文件，以及设置加解密相关配置接口
////////////////////////////////////////////////////////////////////////////////
class GLDCRYPTSHARED_EXPORT GCryptAlgorithm
{
public:	
	//使用密钥加密明文字符串plainText 并且返回密文
    const GString encryptStr(const GString &plainText);
	
	//使用密钥解密密文cipher 并且返回恢复后的明文
    const GString decryptStr(const GString &cipher);
	
	//使用密钥加密明文文件inFilename 并且返回密文文件outFilename
    bool encryptFile(const GString & outFilename, const GString & inFilename);
	
	//使用密钥解密密文文件DecFilename 并且返回恢复后的名文文件recoverFilename
    bool decryptFile(const GString & recoverFilename, const GString & DecFilename);

	//根据用户参数加密模式EnEncodeType，返回对应的字符串
    static GString encodeModeStr(EnEncodeMode EnEncodeType);

	//根据用户参数填充类型EnPadding，返回对应的字符串
    static GString paddingTypeStr(EnPaddingType EnPadding);

	//设置加密参数，包括加密算法EnAlgorithmKind，加密模式EnEncodeType，填充类型EnPadding
	void setEncodeMode(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeType, EnPaddingType EnPadding);

	//设置加密密钥及长度，此处需要重载，因为CBC模式不需要初始向量iv
    void setKey(byte *key, int length, byte *iv);
	void setKey(byte *key, int length);

	//获取加密密钥
	byte * key();
	//获取加密初始向量
    byte * ivKey();

	//根据内部生成随机的加密密钥并设置
	virtual void setRandomKey() = 0;

    //根据用户参数加密模式EnEncodeType 生成相应的加解密器
    virtual SymmetricCipher* encryptor(EnEncodeMode EnEncodeType) = 0;
    virtual SymmetricCipher* decryptor(EnEncodeMode EnEncodeType) = 0;

protected:
    GCryptAlgorithm(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeType, EnPaddingType EnPadding);
    GCryptAlgorithm(GCryptAlgorithmPrivate &dd, EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeType, EnPaddingType EnPadding);
    ~GCryptAlgorithm();

protected:
    GCryptAlgorithmPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(GCryptAlgorithm);

	friend class GCrypt;
};

class GAesAlgorithmPrivate;
////////////////////////////////////////////////////////////////////////////////
//设计:AES 加解密算法策略类，实现父类GCryptAlgorithm加解密器的方法
//功能:AES算法子类，提供加解密串/文件，以及设置加解密相关配置接口
////////////////////////////////////////////////////////////////////////////////
class GLDCRYPTSHARED_EXPORT GAesAlgorithm : public GCryptAlgorithm
{
public:
	GAesAlgorithm(EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType);
	~GAesAlgorithm();
	
	//根据内部生成随机的加密密钥并设置
	void setRandomKey();
	
    //根据用户参数加解密模式EnEncodeType等 生成相应的加解密密器
    SymmetricCipher* encryptor(EnEncodeMode EnEncodeMode);
    SymmetricCipher* decryptor(EnEncodeMode EnEncodeMode);

private:
    Q_DECLARE_PRIVATE(GAesAlgorithm);
};

class GDesAlgorithmPrivate;
////////////////////////////////////////////////////////////////////////////////
//设计:DES 加解密算法策略类，实现父类GCryptAlgorithm加解密器的方法
//     
//功能:DES算法子类，提供加解密串/文件，以及设置加解密相关配置接口
////////////////////////////////////////////////////////////////////////////////
class GLDCRYPTSHARED_EXPORT GDesAlgorithm : public GCryptAlgorithm
{
public:
	GDesAlgorithm(EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType);
	~GDesAlgorithm();
	
	//根据内部生成随机的加密密钥并设置
	void setRandomKey();
	
    //根据用户参数加解密模式EnEncodeType 生成相应的加解密密器
    SymmetricCipher* encryptor(EnEncodeMode EnEncodeMode);
    SymmetricCipher* decryptor(EnEncodeMode EnEncodeMode);

private:
    Q_DECLARE_PRIVATE(GDesAlgorithm);
};

class GCryptPrivate;
////////////////////////////////////////////////////////////////////////////////
//设计:创建一个加解密策略类，根据用户传入参数，用工厂方法动态创建加解密对象
//            ，屏蔽差异，用户不感知，方便扩展；封装适配器接口，供具体对象实现
//功能:根据用户传入的加密参数，生成对应的加密对象
////////////////////////////////////////////////////////////////////////////////
class GLDCRYPTSHARED_EXPORT GCrypt
{
public:
    GCrypt(EnAlgorithmType enAlgorithmType = EN_ALGORITHM_DES
         , EnEncodeMode EnEncodeType = ENCODE_MODE_CBC, EnPaddingType EnPadding = EN_PKCS_PADDING);
	~GCrypt();
	
	//使用密钥加密明文字符串plainText 并且返回密文
    GString encryptStr(const GString &strPlainText);
	
	//使用密钥解密密文cipher 并且返回恢复后的明文
    GString decryptStr(const GString &strCipher);
	
	//使用密钥加密明文文件inFilename 并且返回密文文件outFilename
    bool encryptFile(const GString &strOutFilename, const GString &strInFilename);
	
	//使用密钥解密密文文件DecFilename 并且返回恢复后的名文文件recoverFilename
    bool decryptFile(const GString &strRecoverFilename, const GString &strDecFilename);

	//设置加密参数，包括加密算法EnAlgorithmKind，加密模式EnEncodeType，填充类型EnPadding
    void setEncodeMode(EnAlgorithmType EnAlgorithmKind, EnEncodeMode EnEncodeType, EnPaddingType EnPadding);
	
	//根据内部生成随机的加密密钥并设置
    void setRandomKey();
	
	//设置加密密钥及长度，此处需要重载，因为CBC模式不需要初始向量iv
    void setKey(byte *key, int length, byte *iv);
    void setKey(byte *key, int length) ;
	
	//获取加密密钥
    byte * key();
	
	//获取加密初始向量
    byte * ivKey();
	
private:	
	//根据用户传入的加密参数，生成对应的加密类对象
	GCryptAlgorithm* createAlgorithm(EnAlgorithmType enAlgorithmType, EnEncodeMode EnEncodeMode, EnPaddingType EnPaddingType);

private:
    GCryptPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(GCrypt);
};

#endif
