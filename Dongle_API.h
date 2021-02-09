/*
  ARM ������ API �ӿڿ�
*/

#ifndef  __DONGLE_HEADER_H
#define  __DONGLE_HEADER_H

#include "winscard.h"
#pragma comment (lib, "winscard.lib")

#ifdef __cplusplus
extern "C" {
#endif

#ifndef         IN
    #define     IN
#endif

#ifndef         OUT
    #define     OUT
#endif

/************************************************************************/
/*                                  ����                                */
/************************************************************************/
//�������������
typedef void * DONGLE_HANDLE;

//Ĭ�ϵ�PIN�����Դ���Ϊ������
//�����������ʼ����ʱ��ͬʱ��ʼ��PID��ADMINPIN (PID���ɸ���, ADMINPIN�ɸ���)


#define CONST_PID                  0xFFFFFFFF   //����ʱĬ�ϵ�PID
#define CONST_USERPIN              "12345678"   //����ʱĬ�ϵ�USERPIN
#define CONST_ADMINPIN             "FFFFFFFFFFFFFFFF" //����ʱĬ�ϵ�ADMINPIN

//ͨѶЭ�����Ͷ���
#define PROTOCOL_HID               0 //hidЭ��
#define PROTOCOL_CCID              1 //ccidЭ��

//�ļ����Ͷ���
#define FILE_DATA                  1 //��ͨ�����ļ�
#define FILE_PRIKEY_RSA            2 //RSA˽Կ�ļ�
#define FILE_PRIKEY_ECCSM2         3 //ECC����SM2˽Կ�ļ�(SM2˽Կ�ļ���ECC˽Կ�ļ��ṹ��ͬ������ͬ�ļ�����)
#define FILE_KEY                   4 //SM4��3DES��Կ�ļ�
#define FILE_EXE                   5 //��ִ���ļ�

//LED��״̬����
#define LED_OFF			           0 //����
#define LED_ON			           1 //����
#define LED_BLINK		           2 //����

//PIN������
#define FLAG_USERPIN   			   0 //�û�PIN
#define FLAG_ADMINPIN			   1 //������PIN

//�ӽ��ܱ�־
#define FLAG_ENCODE                0 //����
#define FLAG_DECODE                1 //����

//HASH�㷨����
#define FLAG_HASH_MD5			   0 //MD5     ������16�ֽ�
#define FLAG_HASH_SHA1             1 //SHA1    ������20�ֽ�
#define FLAG_HASH_SM3              2 //SM3     ������32�ֽ�

//Զ�������Ĺ��ܺ�
#define UPDATE_FUNC_CreateFile     1 //�����ļ�
#define UPDATE_FUNC_WriteFile      2 //д�ļ�
#define UPDATE_FUNC_DeleteFile     3 //ɾ���ļ�
#define UPDATE_FUNC_FileLic        4 //�����ļ���Ȩ
#define UPDATE_FUNC_SeedCount      5 //������������������
#define UPDATE_FUNC_DownloadExe    6 //������ִ���ļ�
#define UPDATE_FUNC_UnlockUserPin  7 //�����û�PIN
#define UPDATE_FUNC_Deadline       8 //ʱ��������ʹ������

/************************************************************************/
/*                              �ṹ                                    */
/************************************************************************/
//RSA��Կ��ʽ(����1024,2048)
typedef struct {
	unsigned int  bits;                   // length in bits of modulus
	unsigned int  modulus;				  // modulus
	unsigned char exponent[256];          // public exponent

} RSA_PUBLIC_KEY;

//RSA˽Կ��ʽ(����1024,2048)
typedef struct {
	unsigned int  bits;                   // length in bits of modulus
	unsigned int  modulus;                // modulus
	unsigned char publicExponent[256];    // public exponent
	unsigned char exponent[256];          // private exponent

} RSA_PRIVATE_KEY;

//�ⲿECCSM2��Կ��ʽ ECC(֧��bitsΪ192��256)��SM2��(bitsΪ�̶�ֵ0x8100)��Կ��ʽ
typedef struct{
	unsigned int bits;                    // length in bits of modulus
	unsigned int XCoordinate[8];          // �����ϵ��X����
	unsigned int YCoordinate[8];          // �����ϵ��Y����

} ECCSM2_PUBLIC_KEY;

//�ⲿECCSM2˽Կ��ʽ ECC(֧��bitsΪ192��256)��SM2��(bitsΪ�̶�ֵ0x8100)˽Կ��ʽ
typedef struct{
	unsigned int bits;                    // length in bits of modulus
	unsigned int PrivateKey[8];           // ˽Կ

} ECCSM2_PRIVATE_KEY;

//��������Ϣ
typedef struct
{
	unsigned short  m_Ver;               //COS�汾,����:0x0201,��ʾ2.01�� 
	unsigned short  m_Type;              //��Ʒ����: 0xFF��ʾ��׼��, 0x00Ϊ��׼ʱ����,0x02Ϊ��׼U����  
	unsigned char   m_BirthDay[8];       //�������� 
	unsigned long   m_Agent;             //�����̱��,����:Ĭ�ϵ�0xFFFFFFFF
	unsigned long   m_PID;               //��ƷID
	unsigned long   m_UserID;            //�û�ID
	unsigned char   m_HID[8];            //8�ֽڵ�Ӳ��ID
	unsigned long   m_IsMother;          //ĸ����־: 0x01��ʾ��ĸ��, 0x00��ʾ����ĸ��
    unsigned long   m_DevType;           //�豸����(PROTOCOL_HID����PROTOCOL_CCID)

} DONGLE_INFO;

/**   
 *   �����ļ�˵��
 *   1.RSA˽Կ�ļ�����������������Ϊ8��
 *   2.ECCSM2˽Կ�ļ�����������������Ϊ16��
 *   3.3DES/SM4��Կ�ļ�����������������Ϊ32��
 *   4.��ִ���ļ�����������������Ϊ64��,�ܴ�С���ܳ���64K
 *   5.�����ļ��������������ڿռ��С���ļ�ϵͳ�������ص�Ӱ�죬������������54����
 *   6.�ļ�IDȡֵ��ΧΪ0x0000~0xFFFF֮�䣬����ID��0x0000��0xFFFF��0x3F00������ϵͳռ�ã��û�����ʹ�á�
 */

/*************************�ļ���Ȩ�ṹ***********************************/
//�����ļ���Ȩ�ṹ
typedef struct
{
	unsigned short  m_Read_Priv;     //��Ȩ��: 0Ϊ��С����Ȩ�ޣ�1Ϊ��С�û�Ȩ�ޣ�2Ϊ��С������Ȩ��
	unsigned short  m_Write_Priv;    //дȨ��: 0Ϊ��С����Ȩ�ޣ�1Ϊ��С�û�Ȩ�ޣ�2Ϊ��С������Ȩ��

} DATA_LIC;

//˽Կ�ļ���Ȩ�ṹ
typedef struct
{
	long           m_Count;        //�ɵ�����: 0xFFFFFFFF��ʾ������, �ݼ���0��ʾ�Ѳ��ɵ���
	unsigned char  m_Priv;         //����Ȩ��: 0Ϊ��С����Ȩ�ޣ�1Ϊ��С�û�Ȩ�ޣ�2Ϊ��С������Ȩ��
	unsigned char  m_IsDecOnRAM;   //�Ƿ������ڴ��еݼ�: 1Ϊ���ڴ��еݼ���0Ϊ��FLASH�еݼ�
	unsigned char  m_IsReset;      //�û�̬���ú��Ƿ��Զ��ص�����̬: TRUEΪ����ص�����̬ (������̬���ܴ�����)
	unsigned char  m_Reserve;      //����,����4�ֽڶ���

} PRIKEY_LIC;

//�ԳƼ����㷨(SM4/TDES)��Կ�ļ���Ȩ�ṹ
typedef struct
{
	unsigned long  m_Priv_Enc;   //����ʱ�ĵ���Ȩ��: 0Ϊ��С����Ȩ�ޣ�1Ϊ��С�û�Ȩ�ޣ�2Ϊ��С������Ȩ��

} KEY_LIC;

//��ִ���ļ���Ȩ�ṹ
typedef struct
{
	unsigned short  m_Priv_Exe;   //���е�Ȩ��: 0Ϊ��С����Ȩ�ޣ�1Ϊ��С�û�Ȩ�ޣ�2Ϊ��С������Ȩ��
	
} EXE_LIC;

/****************************�ļ����Խṹ********************************/
//�����ļ��������ݽṹ
typedef struct
{
	unsigned long    m_Size;      //�����ļ����ȣ���ֵ���Ϊ4096
	DATA_LIC         m_Lic;       //��Ȩ
	
} DATA_FILE_ATTR;

//ECCSM2/RSA˽Կ�ļ��������ݽṹ
typedef struct
{
	unsigned short  m_Type;       //��������:ECCSM2˽Կ �� RSA˽Կ
	unsigned short  m_Size;       //���ݳ���:RSA��ֵΪ1024��2048, ECC��ֵΪ192��256, SM2��ֵΪ0x8100
	PRIKEY_LIC      m_Lic;        //��Ȩ

} PRIKEY_FILE_ATTR;

//�ԳƼ����㷨(SM4/TDES)��Կ�ļ��������ݽṹ
typedef struct
{
	unsigned long  m_Size;       //��Կ���ݳ���=16
	KEY_LIC        m_Lic;        //��Ȩ

} KEY_FILE_ATTR;

//��ִ���ļ��������ݽṹ
typedef struct
{
	EXE_LIC         m_Lic;        //��Ȩ	
	unsigned short  m_Len;        //�ļ�����

} EXE_FILE_ATTR;

/*************************�ļ��б�ṹ***********************************/
//��ȡ˽Կ�ļ��б�ʱ���ص����ݽṹ
typedef struct
{
	unsigned short    m_FILEID;  //�ļ�ID
	unsigned short    m_Reserve; //����,����4�ֽڶ���
    PRIKEY_FILE_ATTR  m_attr;    //�ļ�����

}PRIKEY_FILE_LIST;

//��ȡSM4��TDES��Կ�ļ��б�ʱ���ص����ݽṹ
typedef struct
{
	unsigned short    m_FILEID;  //�ļ�ID
	unsigned short    m_Reserve; //����,����4�ֽڶ���
    KEY_FILE_ATTR     m_attr;    //�ļ�����

}KEY_FILE_LIST;

//��ȡ�����ļ��б�ʱ���ص����ݽṹ
typedef struct
{
    unsigned short    m_FILEID;  //�ļ�ID
	unsigned short    m_Reserve; //����,����4�ֽڶ���
	DATA_FILE_ATTR    m_attr;    //�ļ�����

}DATA_FILE_LIST;

//��ȡ��ִ���ļ��б�ʱ���ص����ݽṹ
typedef struct
{
	unsigned short  m_FILEID;    //�ļ�ID
	EXE_FILE_ATTR   m_attr;
	unsigned short  m_Reserve;  //����,����4�ֽڶ���
	
}EXE_FILE_LIST;

//���غ��п�ִ���ļ�ʱ�������ݽṹ
typedef struct
{    
	unsigned short  m_dwSize;           //��ִ���ļ���С
	unsigned short  m_wFileID;          //��ִ���ļ�ID
	unsigned char   m_Priv;             //����Ȩ��: 0Ϊ��С����Ȩ�ޣ�1Ϊ��С�û�Ȩ�ޣ�2Ϊ��С������Ȩ��
	unsigned char  *m_pData;            //��ִ���ļ�����

}EXE_FILE_INFO;

//��Ҫ���������ĳ�ʼ������
typedef struct
{
	int    m_SeedLen;                 //�����볤��
	BYTE   m_SeedForPID[256];         //������ƷID�Ϳ���������������� (�250���ֽ�)
	char   m_UserPIN[18];             //�û�����(16���ַ���0��ֹ�ַ���)
	BYTE   m_UserTryCount;            //�û�������������������Դ���
	BYTE   m_AdminTryCount;           //������������������������Դ���
	RSA_PRIVATE_KEY m_UpdatePriKey;   //Զ������˽Կ
	DWORD  m_UserID_Start;            //��ʼ�û�ID

} SON_DATA;

//ĸ������
typedef struct
{
	SON_DATA  m_Son;                  //������ʼ������
	long      m_Count;                //�ɲ���������ʼ�����ݵĴ��� (-1��ʾ�����ƴ���, �ݼ���0ʱ������)

} MOTHER_DATA;

/**************************************************************************/
/*                                      API                               */
/**************************************************************************/

/** 
 * @brief  ö�ټ�����������������ö�ٳ�32��hid�豸��32��ccid�豸��
 *
 * @param  pDongleInfo     [out]     �豸��Ϣ�����顣���˲���ΪNULLʱ, pCount�����ҵ����豸����Ŀ��
 * @param  pCount 	       [out]     �豸��Ŀ���ú���������ͬʱö�ٳ�32��HID�豸��32��CCID�豸��
 *
 * @return DONGLE_SUCCESS            ִ�гɹ���
 */
DWORD WINAPI Dongle_Enum(DONGLE_INFO * pDongleInfo, int * pCount);

/**
 * @brief  ��ָ���ļ�������
 *
 * @param  phDongle     [out]     ���ָ�롣����򿪳ɹ�,�ᱻ��䡣
 * @param  nIndex       [in]      ����0������ֵ��ָʾ���ҵ��ĵڼ��Ѽ�������
 *
 * @return DONGLE_SUCCESS         �򿪳ɹ���
 */
DWORD WINAPI Dongle_Open(DONGLE_HANDLE * phDongle, int nIndex);

/**
 * @brief  ���PIN����֤״̬����������״̬��Ϊ������
 *
 * @param  hDongle     [in]     �򿪵ļ����������
 *
 * @return DONGLE_SUCCESS       ִ�гɹ���
 */
DWORD WINAPI Dongle_ResetState(DONGLE_HANDLE hDongle);

/**
 * @brief  �رմ򿪵ļ�������
 *
 * @param  hDongle     [in]     �򿪵ļ����������
 *
 * @return DONGLE_SUCCESS       �رճɹ���
 */
DWORD WINAPI Dongle_Close(DONGLE_HANDLE hDongle);

/**
 * @brief  ���������������Ȩ�޼��ɲ�����
 *
 * @param  hDongle          [in]      �򿪵ļ����������
 * @param  nLen             [in]      Ҫ������������ĳ��ȡ�nLen��ȡֵ��ΧΪ 1~128��
 * @param  pRandom          [out]     �������������
 *
 * @return DONGLE_SUCCESS             ��ȡ������ɹ���
 */
DWORD WINAPI Dongle_GenRandom(DONGLE_HANDLE hDongle, int nLen, BYTE * pRandom);

/**
 * @brief  LED�ƵĿ��Ʋ���������Ȩ�޼��ɲ�����
 *
 * @param  hDongle     [in]     �򿪵ļ����������
 * @param  nFlag       [in]     �������͡����磺nFlag = LED_ON����ʾ����LEDΪ����״̬��
 *                              nFlag = LED_OFF����ʾ����LEDΪ���״̬��nFlag = LED_BLINK��
 *                              ��ʾ����LEDΪ��˸��״̬��
 *
 * @return DONGLE_SUCCESS       ����ִ�гɹ���
 */
DWORD WINAPI Dongle_LEDControl(DONGLE_HANDLE hDongle, int nFlag);

/**
 * @brief  �л�ͨѶЭ�顣����ִ�гɹ�����������Զ�����,�򿪵ľ��hDongle����Ч,������ִ�йرղ����᷵��
 *         0xF0000002�Ĵ����룬�����������������������������ö�ٲ��������ò�������Ҫ��֤������PIN��֮
 *	       �󷽿�ʹ�á�
 *
 * @param  hDongle       [in]   �򿪵ļ����������
 * @param  nFlag         [in]   Э�����͡����磺nFlagֵΪPROTOCOL_HID����ʾ���������л�ΪHID�豸��
 *                              nFlagֵΪPROTOCOL_CCID����ʾ���������л�ΪCCID�豸
 *
 * @return DONGLE_SUCCESS       ִ�гɹ���
 */
DWORD WINAPI Dongle_SwitchProtocol(DONGLE_HANDLE hDongle, int nFlag);

/**
 * @brief  һ���ָ��������س���״̬����������PID���û�PIN�롢������PIN��ȣ�ȫ���ָ�������״̬����д���
 *       ���ݶ�������ա����⣬����ִ�гɹ�����������Զ�����,�򿪵ľ��hDongle����Ч,������ִ�йرղ�
 *       ���᷵��0xF0000002�Ĵ����룬�����������������������������ö�ٲ��������ò�����Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle       [in]     �򿪵ļ����������
 *
 * @return DONGLE_SUCCESS         ִ�гɹ���
 */
DWORD WINAPI Dongle_RFS(DONGLE_HANDLE hDongle);

/**
 * @brief  �����ļ����ú�����֧�ֿ�ִ���ļ��Ĵ������ò�����Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle       [in]     �򿪵ļ����������
 * @param  nFileType     [in]     �ļ����͡�
 *                                nFileType = FILE_DATA����ʾ���������ļ����������ļ�������˵����
 *                                   1.�ļ���С��Ϊ252�ֽ�ʱ,���ɴ���54���ļ�,��ռ�ÿռ�13608�ֽ�
 *                                   2.�ļ���С��Ϊ1024�ֽ�ʱ,���ɴ���31���ļ�����ռ�ÿռ�31744�ֽ�
 *                                   3.�ļ���С��Ϊ4096�ֽ�ʱ�����ɴ���9���ļ�,��ռ�ÿռ�36864�ֽ�
 *                                nFileType = FILE_PRIKEY_RSA����ʾ����RSA˽Կ�ļ���
 *                                nFileType = FILE_PRIKEY_ECCSM2����ʾ����ECCSM2˽Կ�ļ���

 *                                nFileType = FILE_KEY����ʾ����SM4��3DES��Կ�ļ���
 *                                ��֧��nFileType = FILE_EXE���ļ����͡�
 * @param  wFileID       [in]     �ļ�ID��
 * @param  pFileAttr     [in]     �ļ������ԡ������ĽṹΪ��DATA_FILE_ATTR��PRIKEY_FILE_ATTR��KEY_FILE_ATTR��
 *
 * @return DONGLE_SUCCESS         �����ļ��ɹ���  
 */
DWORD WINAPI Dongle_CreateFile(DONGLE_HANDLE hDongle, int nFileType, WORD wFileID, void * pFileAttr);

/**
 * @brief  д�ļ����ú�����֧�ֿ�ִ���ļ���д��������Ҹò�����Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle       [in]     �򿪵ļ����������
 * @param  nFileType     [in]     �ļ����͡����磬
 *                                nFileType = FILE_DATA����ʾ���������ļ���
 *                                nFileType = FILE_PRIKEY_RSA����ʾ����RSA˽Կ�ļ���
 *                                nFileType = FILE_PRIKEY_ECCSM2����ʾ����ECCSM2˽Կ�ļ���
 *                                nFileType = FILE_KEY����ʾ����SM4��3DES��Կ�ļ���
 *                                ��֧��nFileType = FILE_EXE���ļ����͡�
 * @param  wFileID       [in]     �ļ�ID��
 * @param  wOffset       [in]     �ļ�ƫ�ơ��ļ�д�����ʼƫ������
 * @param  pInData       [in]     ׼��д������ݡ�
 * @param  nDataLen      [in]     ����pInData�Ĵ�С��
 *
 * @return DONGLE_SUCCESS         д���ļ��ɹ���
 */
DWORD WINAPI Dongle_WriteFile(DONGLE_HANDLE hDongle, int nFileType, WORD wFileID, WORD wOffset, BYTE * pInData, int nDataLen);

/**
 * @brief  ��ȡ�������ڵ������ļ��������ļ��Ķ�ȡȨ��ȡ���ڴ���ʱ���趨��
 *
 * @param  hDongle      [in]         �򿪵ļ����������
 * @param  wFileID      [in]         �ļ�ID��
 * @param  wOffset      [in]         �ļ�ƫ������
 * @param  pOutData     [in]         ���ݻ�������
 * @param  nDataLen     [out]        ����pOutData�ĳ��ȡ�
 *
 * @return DONGLE_SUCCESS            ��ȡ�����ļ��ɹ�  
 */
DWORD WINAPI Dongle_ReadFile(DONGLE_HANDLE hDongle, WORD wFileID, WORD wOffset, BYTE * pOutData, int nDataLen);

/**
 * @brief  �������ؿ�ִ���ļ������ڿ�ִ���ļ����������ܳ���64������ִ���ļ����ܴ�С���ܳ���64K��
 *         �ò�����Ҫ��֤����ԱȨ��
 *
 * @param  hDongle          [in]     �򿪵ļ����������
 * @param  pExeFileInfo     [in]     ��ִ���ļ���Ϣ�����顣
 * @param  nCount           [in]     ����ִ���ļ���������
 * 
 * @return DONGLE_SUCCESS            �������ؿ�ִ���ļ��ɹ���
 */
DWORD WINAPI Dongle_DownloadExeFile(DONGLE_HANDLE hDongle, EXE_FILE_INFO * pExeFileInfo, int nCount);

/**
 * @brief  ����ָ�������ڿ�ִ�г������п�ִ���ļ���Ȩ�ޣ�ȡ������������ʱ��ÿ����ִ���ļ������ã�
 *         ����EXE_FILE_INFO�е�m_Priv����������������ݵ���󳤶Ȳ��ܳ���1024�ֽڣ�����������ݻ�
 *         ����pInOutBuf��Ӧ���ڵ�InOutBuf��
 *
 * @param  hDongle            [in]             �򿪵ļ����������
 * @param  wFileID            [in]             ��ִ���ļ����ļ�ID��
 * @param  pInOutBuf          [in,out]         ����������ݻ�������
 * @param  wInOutBufLen       [in]             ����������ݻ�����pInOutBuf�Ĵ�С��                                           
 * @param  pMainRet           [out]            ���ڿ�ִ�г���main�����ķ���ֵ������ΪNULL��
 *
 * @return DONGLE_SUCCESS                  ����ָ���Ŀ�ִ���ļ��ɹ���
 */
DWORD WINAPI Dongle_RunExeFile(DONGLE_HANDLE hDongle, WORD wFileID, BYTE * pInOutBuf, WORD wInOutBufLen, int * pMainRet);

/**
 * @brief  ɾ���ļ�����Ҫ������Ȩ�ޡ�
 * 
 * @param  hDongle       [in]      �򿪵ļ����������
 * @param  nFileType     [in]      �ļ����͡�
 * @param  wFileID       [in]      �ļ�ID��
 *
 * @return DONGLE_SUCCESS          ɾ���ļ��ɹ� 
 */
DWORD WINAPI Dongle_DeleteFile(DONGLE_HANDLE hDongle, int nFileType, WORD wFileID);

/**
 * @brief ���ļ�����Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle       [in]         �򿪵ļ����������
 * @param  nFileType     [in]         ָʾ�ļ����͡����磬FILE_DATA�ȡ�
 * @param  pFileList     [in]         pList������ļ����б� (�˲���ΪNULLʱ, pLen�з�������Ļ���������)
 *                                    ��nFileType = FILE_DATAʱ,       pFileListָ��DATA_FILE_LIST�ṹ��
 *                                    ��nFileType = FILE_PRIKEY_RSAʱ, pFileListָ��PRIKEY_FILE_LIST�ṹ��
 *                                    ��nFileType = FILE_PRIKEY_ECCSM2ʱ, pFileListָ��PRIKEY_FILE_LIST�ṹ
 *                                    ��nFileType = FILE_KEYʱ,        pFileListָ��KEY_FILE_LIST�ṹ��
 *	                                  ��nFileType = FILE_EXEʱ,        pFileListָ��EXE_FILE_LIST�ṹ��
 * @param  pDataLen      [in,out]     ����pFileList�����볤�ȣ�ִ�гɹ�����pFileList���ֽڳ��ȡ�
 *
 * @return DONGLE_SUCCESS             ���ļ��ɹ���
 */
DWORD WINAPI Dongle_ListFile(DONGLE_HANDLE hDongle, int nFileType, void* pFileList, int * pDataLen);

/**
 * @brief Ψһ�������������������PID�Ϳ�����PIN�룬��Ҫ������Ȩ�ޣ�ִ�гɹ���������Զ��ص�����̬��
 *		  ����������PIN���Ŀ����Ϊ��ʹ����϶����������룬�����Ŀ�����PIN���Խ���Dongle_ChangePIN
 *        ���и��ġ����⣬������һ��Ҫ�μǣ������κ�����Զ�޷���֪������PIN�롣
 * 
 * @param hDongle		   [in]       �򿪵ļ����������
 * @param nSeedLen		   [in]       ����pSeed�Ļ��������ȡ�
 * @param pSeed            [in]       ������Ļ�������
 * @param pPIDStr          [out]      ����ִ�гɹ�����PID���û�������С����Ӧ��Ϊ8�ֽڣ�����һ��
 *                                    8�ֽڵ���0��ֹ��ansi�ַ�����
 * @param pAdminPINstr     [out]      ����ִ�гɹ����ؿ�����PIN�롣�û�������С����Ӧ��Ϊ16�ֽڣ�
 *                                    �����ַ�������Ϊ16�ֽڵ���0��ֹ��ansi�ַ�����
 *
 * @return DONGLE_SUCCESS             Ψһ�����ɹ���
 */
DWORD WINAPI Dongle_GenUniqueKey(DONGLE_HANDLE hDongle,int nSeedLen, BYTE * pSeed, char * pPIDstr, char * pAdminPINstr);

/**
 * @brief  У������
 *
 * @param  hDongle		    [in]       �򿪵ļ����������
 * @param  nFlags           [in]       PIN�����͡�����ȡֵΪFLAG_USERPIN����FLAG_ADMINPIN��
 * @param  pPIN             [in]       PIN�룬0��ֹ��ansi�ַ�����
 * @param  pRemainCount     [out]      ʣ�����Դ���������0��ʾ��������1~253��ʾʣ�������255��ʾ���������Դ�����
 *
 * @return DONGLE_SUCCESS              У��ɹ������У��ʧ�ܣ������ķ���ֵ��Ҳ����ʣ������Դ�����
 *                                     (������ & 0xFFFFFF00) == DONGLE_INCORRECT_PIN��������λ��ʾʣ�������
 */
DWORD WINAPI Dongle_VerifyPIN(DONGLE_HANDLE hDongle, int nFlags, char * pPIN, int * pRemainCount);

/**
 * @brief  ��������
 * 
 * @param  hDongle       [in]     �򿪵ļ����������
 * @param  nFlags        [in]     PIN�����͡�����ȡֵΪFLAG_USERPIN����FLAG_ADMINPIN��
 * @param  pOldPIN       [in]     �ɵ�PIN�뻺������������һ���ַ�������Ϊ16�ֽڵ�0��ֹ��ansi�ַ���,�ҿ��������ġ�
 * @param  pNewPIN       [in]     �µ�PIN�뻺������������һ���ַ�������Ϊ16�ֽڵ�0��ֹ��ansi�ַ�����
 * @param  nTryCount     [in]     ���Դ������ò�����ȡֵ��ΧΪ1~255,����255��ʾ���������Դ�����
 *
 * @return DONGLE_SUCCESS         �޸�����ɹ���
 */
DWORD WINAPI Dongle_ChangePIN(DONGLE_HANDLE hDongle, int nFlags, char * pOldPIN, char * pNewPIN, int nTryCount);

/**
 * @brief  �����û�PIN�롣����(��PID=FFFFFFFF)�����������롣ִ�гɹ����û�����ָ�Ϊ����Ĭ�� CONST_USERPIN
 *
 * @param  hDongle       [in]     �򿪵ļ����������
 * @param  pAdminPIN     [in]     ������PIN�뻺����������Ϊ16�ֽڵ�0��ֹ��ansi�ַ���
 *
 * @return DONGLE_SUCCESS         �����û�PIN��ɹ���
 */
DWORD WINAPI Dongle_ResetUserPIN(DONGLE_HANDLE hDongle, char * pAdminPIN);

/**
 * @brief  �����û�ID����Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle      [in]     �򿪵ļ����������
 * @param  dwUserID     [in]     �û�ID��
 *
 * @return DONGLE_SUCCESS        �����û�PIN��ɹ���
 */
DWORD WINAPI Dongle_SetUserID(DONGLE_HANDLE hDongle, DWORD dwUserID);

/** 
 * @brief  ��ȡ����������ʱ�䡣����Ȩ�޻�ȡ��
 *
 * @param  hDongle     [in]      �򿪵ļ����������
 * @param  pdwTime     [out]     ��ȡ�ĵ���UTCʱ��ֵ��
 *                               ��*pdwTime = 0XFFFFFFFF����ʾ�����Ƶ���ʱ��
 *                               ��(*pdwTime & 0XFFFF0000) == 0��ֵ��ʾ��ʣ�༸Сʱ
 *                               ��(*pdwTime & 0XFFFF0000) != 0��ֵ��ʾ���ڵ�UTC��ʱ�䣬����ͨ��gmtime�Ƚ���ֵ����ת����
 *
 * @return DONGLE_SUCCESS        ��ȡ����������ʱ��ɹ���
 */
DWORD WINAPI Dongle_GetDeadline(DONGLE_HANDLE hDongle, DWORD * pdwTime);

/**
 * @brief  ���ü������ĵ���ʱ�䡣�ò�����Ҫ����ԱȨ�ޡ�
 * 
 * @param  hDongle     [in]     �򿪵ļ����������                              
 * @param  dwTime      [in]     ʱ��ֵ��˵����
 *                              1.���ÿ���Сʱ������Χ��1~65535������dwTime = 24�����������У�����û�PIN���ʼ��ʱ��
 *                              2.���õ��ڵ�������ʱ���롣��ͨ������time����mktime ȡ�ü�ʱ��utcʱ��ֵ(utcֵ������65535)��
 *                              3.ȡ������ʱ�����ƣ���ʱdwTime��ֵֻ��Ϊ0xFFFFFFFF��
 *
 * @return DONGLE_SUCCESS       ���ü���������ʱ��ɹ���
 */
DWORD WINAPI Dongle_SetDeadline(DONGLE_HANDLE hDongle, DWORD dwTime);

/**
 * @brief  ��ȡ��������UTCʱ�䡣
 * 
 * @param  hDongle         [in]      �򿪵ļ����������
 * @param  pdwUTCTime      [out]     UTCʱ��ֵָ�롣
 *
 * @return DONGLE_SUCCESS       ���ü���������ʱ��ɹ���
 */
DWORD WINAPI Dongle_GetUTCTime(DONGLE_HANDLE hDongle, DWORD * pdwUTCTime);

/**
 * @brief  ��ȡ�������������ݡ���������С��8k��ǰ4k(0~4095)�Ķ�дû��Ȩ�����ƣ���4k(4096~8191)����Ȩ�޿ɶ���
 *         ����ֻ�п�����Ȩ�޿�д��
 * 
 * @param  hDongle      [in]      �򿪵ļ����������
 * @param  nOffset      [in]      ��ʼƫ�ơ���Χ��0~8191
 * @param  pData        [out]     ��ȡ�����ݻ�������
 * @param  nDataLen     [in]      ����pData�Ļ�������С��
 *
 * @return  DONGLE_SUCCESS        ��ȡ���������ݳɹ��� 
 */
DWORD WINAPI Dongle_ReadData(DONGLE_HANDLE hDongle, int nOffset, BYTE* pData, int nDataLen);

/**
 * @brief  д���������������ݡ���������С��8k��ǰ4k(0~4095)�Ķ�дû��Ȩ�����ƣ���4k(4096~8191)����Ȩ�޿ɶ���
 *         ����ֻ�п�����Ȩ�޿�д��
 * 
 * @param  hDongle      [in]      �򿪵ļ����������
 * @param  nOffset      [in]      ��ʼƫ�ơ���Χ��0~8191
 * @param  pData        [in]      д������ݻ�������
 * @param  nDataLen     [in]      ����pData�Ļ�������С��
 *
 * @return  DONGLE_SUCCESS        д�����������ݳɹ��� 
 */
DWORD WINAPI Dongle_WriteData(DONGLE_HANDLE hDongle, int nOffset, BYTE * pData, int nDataLen);

/**
 * @brief  ��ȡ�����ڴ����ݡ������ڴ��ܴ�СΪ32�ֽڡ�û��Ȩ�����ƣ����������Զ�������
 *
 * @param  hDongle     [in]      �򿪵ļ����������
 * @param  pData       [out]     ��������ݡ���������ڴ�����ݣ��̶�Ϊ32���ֽڡ�
 *
 * @return  DONGLE_SUCCESS       ��ȡ�����ڴ����ݳɹ���
 */
DWORD WINAPI Dongle_ReadShareMemory(DONGLE_HANDLE hDongle, BYTE * pData);

/**
 * @brief  ���ù����ڴ����ݡ�û��Ȩ�����ƣ����������Զ�������
 * 
 * @param  hDongle      [in]     �򿪵ļ����������
 * @param  pData        [in]     �������ݡ�
 * @param  nDataLen     [in]     ����pData�Ļ�������С�����Ȳ��ܳ���32��
 *
 * @return  DONGLE_SUCCESS       ���ù����ڴ����ݳɹ���
 */
DWORD WINAPI Dongle_WriteShareMemory(DONGLE_HANDLE hDongle, BYTE * pData, int nDataLen);

/**
 * @brief  ����RSA��Կ��˽Կ��ʹ�øú���֮ǰ��Ҫ�ȴ���һ��RSA˽Կ�ļ�����Ҫ������Ȩ�ޡ��ɹ���ע�Ᵽ��RSA��˽Կ���ݡ�
 *
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  wPriFileID     [in]      RSA˽Կ�ļ�ID��
 * @param  pPubBakup      [out]     RSA��Կ���ݡ�
 * @param  pPriBakup      [out]     RSA˽Կ���ݡ�
 *
 * @return DONGLE_SUCCESS           ����RSA��˽Կ�ɹ��� 
 */
DWORD WINAPI Dongle_RsaGenPubPriKey(DONGLE_HANDLE hDongle, WORD wPriFileID, RSA_PUBLIC_KEY * pPubBakup, RSA_PRIVATE_KEY * pPriBakup);

/**
 * @brief  RSA˽Կ���㡣������ʹ��Ȩ��ȡ��������RSA˽Կ�ļ���Ȩ�ޣ���RSA˽Կ�ļ�����ʱ�趨��˵����
 *         1.���ڼ�������,�������ݳ��ȱ���С��˽ԿIDΪwPriFileID����Կ���ȳ���8�ټ�ȥ11,�Ա��ں����ڲ�����padding
 *         2.���ڽ�������,�������ݳ��ȱ�����wPriFileID��ָʾ����Կ������һ��(����1024λ��ԿʱΪ128��2048ʱΪ256)
 *         3.����ʱ�ڲ�padding��ʽΪ:PKCS1_TYPE_1 (���ڶ����ֽ�Ϊ0x01,���������0XFF)   
 *
 * @param  hDongle         [in]         �򿪵ļ����������
 * @param  wPriFileID      [in]         RSA˽Կ�ļ�ID��
 * @param  nFlag           [in]         �������͡����磬FLAG_ENCODE��ʾ�������㣻FLAG_DECODE��ʾ�������㡣
 * @param  pInData         [in]         �������ݡ�
 * @param  nInDataLen      [in]         ����pInData�Ĵ�С
 * @param  pOutData        [out]        ������ݻ�������
 * @param  pOutDataLen     [in,out]     ����pOutData�Ĵ�С�ͷ��ص����ݴ�С��
 *
 * @return DONGLE_SUCCESS               RSA˽Կ����ɹ��� 
 */
DWORD WINAPI Dongle_RsaPri(DONGLE_HANDLE hDongle, WORD wPriFileID, int nFlag, BYTE * pInData, int nInDataLen, BYTE * pOutData, int * pOutDataLen);

/**
 * @brief  RSA��Կ���㡣����Ȩ�޿ɵ��á�˵����
 *         1.���ڼ�������,�������ݳ��ȱ���С��pPubKey��ָʾ����Կ���ȳ���8�ټ�ȥ11,�Ա��ں����ڲ�����padding
 *         2.���ڽ�������,�������ݳ��ȱ�����pPubKey��ָʾ����Կ������һ��(����1024λ��ԿʱΪ128��2048ʱΪ256)
 *         3.����ʱ�ڲ�padding��ʽΪ:PKCS1_TYPE_2 (���ڶ����ֽ�Ϊ0x02,��������������)  
 *
 * @param  hDongle         [in]         �򿪵ļ����������
 * @param  nFlag           [in]         �������͡����磬FLAG_ENCODE��ʾ�������㣻FLAG_DECODE��ʾ�������㡣
 * @param  pPubKey         [in]         RSA��Կ���ݡ���������Դ������RSA��˽Կʱ�Ĺ�Կ���ݡ�
 * @param  pInData         [in]         �������ݡ�
 * @param  nInDataLen      [in]         ����pInData�Ĵ�С��
 * @param  pOutData        [out]        ������ݻ�������
 * @param  pOutDataLen     [in,out]     ����pOutData�Ĵ�С�ͷ��ص����ݴ�С��
 *
 * @return DONGLE_SUCCESS               RSA��Կ����ɹ���
 */
DWORD WINAPI Dongle_RsaPub(DONGLE_HANDLE hDongle, int nFlag, RSA_PUBLIC_KEY * pPubKey, BYTE * pInData, int nInDataLen, BYTE * pOutData, int * pOutDataLen);

/**
 * @brief  ����ECC��Կ��˽Կ��ʹ�øú���֮ǰ��Ҫ�ȴ���һ��ECC˽Կ�ļ�����Ҫ������Ȩ�ޡ��ɹ���ע�Ᵽ��ECC��˽Կ���ݡ�
 *
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  wPriFileID     [in]      ECC˽Կ�ļ�ID��
 * @param  pPubBakup      [out]     ECC��Կ���ݡ�
 * @param  pPriBakup      [out]     ECC˽Կ���ݡ�
 *
 * @return DONGLE_SUCCESS           ����ECC��˽Կ�ɹ��� 
 */
DWORD WINAPI Dongle_EccGenPubPriKey(DONGLE_HANDLE hDongle, WORD wPriFileID, ECCSM2_PUBLIC_KEY * pPubBakup, ECCSM2_PRIVATE_KEY * pPriBakup);

/**
 * @brief  ECC˽Կǩ����������ʹ��Ȩ��ȡ��������ECC˽Կ�ļ���Ȩ�ޣ���ECC˽Կ�ļ�����ʱ�趨��˵����
 *         1.����ǩ���㷨Ϊ: ECDSA_Sign
 *         2.�����Hashֵ�ĳ�����ECC˽Կ����Կ�����й�(�����Կ��192λ��,��hashֵ���Ȳ��ܳ���24(192/8 = 24)�ֽ�)
 *                                                    (�����Կ��256λ��,��hashֵ���Ȳ��ܳ���32(256/8 = 32)�ֽ�)
 *         3.���߲���Ϊ:EC_NIST_PRIME_192��EC_NIST_PRIME_256
 *
 * @param  hDongle          [in]      �򿪵ļ����������
 * @param  wPriFileID       [in]      ECC˽Կ�ļ�ID��
 * @param  pHashData        [in]      Hash���ݡ�
 * @param  nHashDataLen     [in]      ����pHashData�Ĵ�С��
 * @param  pOutData         [out]     ǩ�����ݡ���С�̶�Ϊ64�ֽ�(256λECCʱ������,192λECCʱ��λ�Ჹ0)
 *
 * @return DONGLE_SUCCESS             ��ʾǩ���ɹ���
 */
DWORD WINAPI Dongle_EccSign(DONGLE_HANDLE hDongle, WORD wPriFileID, BYTE * pHashData, int nHashDataLen, BYTE * pOutData);

/**
 * @brief  ECC��Կ��ǩ��������ʹ��Ȩ��ȡ��������ECC˽Կ�ļ���Ȩ�ޣ���ECC˽Կ�ļ�����ʱ�趨��˵����
 *         1.����ǩ���㷨Ϊ: ECDSA_Verify
 *         2.�����Hashֵ�ĳ�����ECC˽Կ����Կ�����й�(�����Կ��192λ��,��hashֵ���Ȳ��ܳ���24(192/8 = 24)�ֽ�)
 *                                                    (�����Կ��256λ��,��hashֵ���Ȳ��ܳ���32(256/8 = 32)�ֽ�)
 *         3.���߲���Ϊ:EC_NIST_PRIME_192��EC_NIST_PRIME_256
 *
 * @param  hDongle          [in]      �򿪵ļ����������
 * @param  pPubKey          [in]      ECC��Կ���ݡ�
 * @param  pHashData        [in]      Hash���ݡ�
 * @param  nHashDataLen     [in]      ����pHashData�Ĵ�С��
 * @param  pSign            [in]      ǩ�����ݡ���С�̶�Ϊ64�ֽڣ�ΪDongle_EccSign�������ص�pOutData���ݡ�
 *
 * @return DONGLE_SUCCESS             ��ʾ��ǩ�ɹ�,�����ʾ��ǩʧ�ܡ�
 */
DWORD WINAPI Dongle_EccVerify(DONGLE_HANDLE hDongle, ECCSM2_PUBLIC_KEY * pPubKey, BYTE * pHashData, int nHashDataLen, BYTE * pSign);

/**
 * @brief  ����SM2��Կ��˽Կ��ʹ�øú���֮ǰ��Ҫ�ȴ���һ��SM2˽Կ�ļ�����Ҫ������Ȩ�ޡ��ɹ���ע�Ᵽ��ECC��˽Կ���ݡ�
 *
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  wPriFileID     [in]      SM2˽Կ�ļ�ID��
 * @param  pPubBakup      [out]     SM2��Կ���ݡ�
 * @param  pPriBakup      [out]     SM2˽Կ���ݡ�
 *
 * @return DONGLE_SUCCESS           ����ECC��˽Կ�ɹ��� 
 */
DWORD WINAPI Dongle_SM2GenPubPriKey(DONGLE_HANDLE hDongle, WORD wPriFileID, ECCSM2_PUBLIC_KEY * pPubBakup, ECCSM2_PRIVATE_KEY * pPriBakup);

/**
 * @brief  SM2˽Կǩ����������ʹ��Ȩ��ȡ��������SM2˽Կ�ļ���Ȩ�ޣ���SM2˽Կ�ļ�����ʱ�趨��
 *
 * @param  hDongle          [in]      �򿪵ļ����������
 * @param  wPriFileID       [in]      SM2˽Կ�ļ�ID��
 * @param  pHashData        [in]      Hash���ݡ�
 * @param  nHashDataLen     [in]      ����pHashData�Ĵ�С�����ݳ��ȱ���С��32���ֽڡ�
 * @param  pOutData         [out]     ǩ�����ݡ���С�̶�Ϊ64�ֽڡ�
 *
 * @return DONGLE_SUCCESS             ��ʾǩ���ɹ���
 */
DWORD WINAPI Dongle_SM2Sign(DONGLE_HANDLE hDongle, WORD wPriFileID, BYTE * pHashData, int nHashDataLen, BYTE * pOutData);

/**
 * @brief  SM2��Կ��ǩ��������ʹ��Ȩ��ȡ��������SM2˽Կ�ļ���Ȩ�ޣ���SM2˽Կ�ļ�����ʱ�趨��
 *
 * @param  hDongle          [in]      �򿪵ļ����������
 * @param  wPriFileID       [in]      SM2��Կ���ݡ�
 * @param  pHashData        [in]      Hash���ݡ�
 * @param  nHashDataLen     [in]      ����pHashData�Ĵ�С��
 * @param  pSign            [in]      ǩ�����ݡ���С�̶�Ϊ64�ֽڣ�ΪDongle_EccSign�������ص�pOutData���ݡ�
 *
 * @return DONGLE_SUCCESS             ��ʾ��ǩ�ɹ�,�����ʾ��ǩʧ�ܡ�
 */
DWORD WINAPI Dongle_SM2Verify(DONGLE_HANDLE hDongle, ECCSM2_PUBLIC_KEY * pPubKey, BYTE * pHashData, int nHashDataLen, BYTE * pSign);

/**
 * @brief  3DES�ӽ��ܡ�������������Ȩ�޼���, ���������Ȩ��ȡ������Կ�ļ���Ȩ�ޡ�
 * 
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  wKeyFileID     [in]      ��Կ�ļ�ID��
 * @param  nFlag          [in]      �������͡����磬FLAG_ENCODE��ʾ�������㣻FLAG_DECODE��ʾ�������㡣
 * @param  pInData        [in]      �������ݻ�������
 * @param  pOutData       [out]     ������ݻ���������С����Ҫ���������ݻ�������ͬ�������������ݻ���������Ϊͬһ����
 * @param  nDataLen       [in]      ����pInData�����ݴ�С�����ݳ��ȱ�����16��������,��������ֵ��1024��
 *
 * @return DONGLE_SUCCESS           3DES���ܻ��������ɹ���
 */
DWORD WINAPI Dongle_TDES(DONGLE_HANDLE hDongle, WORD wKeyFileID, int nFlag, BYTE * pInData, BYTE * pOutData, int nDataLen);

/**
 * @brief  SM4�ӽ��ܡ�������������Ȩ�޼���, ���������Ȩ��ȡ������Կ�ļ���Ȩ�ޡ�
 * 
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  wKeyFileID     [in]      ��Կ�ļ�ID��
 * @param  nFlag          [in]      �������͡����磬FLAG_ENCODE��ʾ�������㣻FLAG_DECODE��ʾ�������㡣
 * @param  pInData        [in]      �������ݻ�������
 * @param  pOutData       [out]     ������ݻ���������С����Ҫ���������ݻ�������ͬ�������������ݻ���������Ϊͬһ����
 * @param  nDataLen       [in]      ����pInData�����ݴ�С�����ݳ��ȱ�����16��������,��������ֵ��1024��
 *
 * @return DONGLE_SUCCESS           SM4���ܻ��������ɹ���
 */
DWORD WINAPI Dongle_SM4(DONGLE_HANDLE hDongle, WORD wKeyFileID, int nFlag, BYTE * pInData, BYTE * pOutData, int nDataLen);

/**
 * @brief  HASH���㡣
 * 
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  nFlag          [in]      Hash�����㷨���͡�
 *                                  nFlag = FLAG_HASH_MD5����ʾMD5���㣬��ʱpHash�Ļ�������СΪ16�ֽڡ�
 *                                  nFlag = FLAG_HASH_SHA1����ʾSHA1���㣬��ʱpHash�Ļ�������СΪ20�ֽڡ�
 *                                  nFlag = FLAG_HASH_SM3����ʾ����SM3���㣬��ʱpHash�Ļ�������СΪ32�ֽڡ�
 * @param  pInData        [in]      �������ݻ�������
 * @param  nDataLen       [in]      ����pInData�����ݴ�С��SHA1��MD5Ϊ�������㣬���Ȳ����ƣ�SM3Ϊ�������㣬
 *                                  ��󲻳���1024�ֽڡ�
 * @param  pHash          [out]     �����Hashֵ��
 *
 * @return DONGLE_SUCCESS           HASH����ɹ���
 */
DWORD WINAPI Dongle_HASH(DONGLE_HANDLE hDongle, int nFlag, BYTE * pInData, int nDataLen, BYTE * pHash);

/**
 * @brief  �������㷨������Ȩ�޿�ʹ��, �����̿����ÿ����������
 *         1.�������㷨��PID�йأ�����(��PID=FFFFFFFF)���ܽ������������㡣
 *         2.����ڲ�����������������Ϊ-1������ݼ���0��˺���������ʹ�á�
 *
 * @param  hDongle        [in]      �򿪵ļ����������
 * @param  pSeed          [in]      ��������������ݡ� 
 * @param  nSeedLen       [in]      �����볤�ȡ�ȡֵ��ΧΪ1~250�ֽڡ�
 * @param  pOutData       [out]     ������ݻ�����������Ĵ�С�̶�Ϊ16�ֽڡ�
 *
 * @return DONGLE_SUCCESS           ����������ɹ���
 */
DWORD WINAPI Dongle_Seed(DONGLE_HANDLE hDongle, BYTE * pSeed, int nSeedLen, BYTE * pOutData);

/**
 * @brief �����������㷨�������������Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle     [in]      �򿪵ļ����������
 * @param  nCount      [in]      ����������������ֵ����Ϊ-1����ʾ���������������
 * 
 * @return DONGLE_SUCCESS        ���óɹ���
 */
DWORD WINAPI Dongle_LimitSeedCount(DONGLE_HANDLE hDongle, int nCount);

/**
 * @brief  ����һ��ĸ������ĸ���ķ�ʽ����һ�ֿ�ѡ�ĳ�ʼ������ʽ����ȫ�ֿ��٣��Ƽ�ʹ�á���Ҫ������Ȩ�ޡ�
 *         1.����(��PID=FFFFFFFF)����д��ĸ�����ݡ�
 *	       2.���ڰ�ȫ����,MOTHER_DATA�е�Զ������˽Կ�������ĸ�������Զ������˽Կ��ͬ,��������ʧ�ܡ�
 *
 * @param  hDongle     [in]      �򿪵ļ����������
 * @param  pInData     [in]      �������ݡ����ڳ�ʼ��ĸ���ĽṹΪMOTHER_DATA�����ݡ�
 *
 * @return DONGLE_SUCCESS        ����ĸ���ɹ���
 */
DWORD WINAPI Dongle_GenMotherKey(DONGLE_HANDLE hDongle, MOTHER_DATA * pInData);

/**
 * @brief  �ӿ�����ȡ�������󡣴˺���ֻ��PIDΪFFFFFFFF�Ŀ�����Ч����Ҫ������Ȩ�ޡ�
 *
 * @param  hDongle      [in]       �򿪵ļ����������
 * @param  pRequest     [out]      ������ݡ����ظ����ݵ���Ч����Ϊ16�ֽڣ������Ҫ����16�ֽڵĿռ䡣
 *
 * @return DONGLE_SUCCESS          ��ȡ�����������ݳɹ���
 */
DWORD WINAPI Dongle_RequestInit(DONGLE_HANDLE hDongle, BYTE * pRequest);

/**
 * @brief  ��ĸ����ȡ���ڳ�ʼ�����������ݣ��ú���ֻ��ĸ����Ч������Ȩ�޿�ʹ�á�
 *
 * @param  hDongle       [in]          �򿪵ļ����������
 * @param  pRequest      [in]          �������ݡ�ͨ��Dongle_RequestInit��ȡ���������ݡ�
 * @param  pInitData     [out]         ������ݡ�����ִ�гɹ��������ڳ�ʼ�����������ݡ�
 * @param  pDataLen      [int,out]     ����pInitData����Ч���ȡ���ʾpInitData�Ļ��������ȣ�����ִ�гɹ�
 *                                     ����pInitData����Ч���ȡ�
 *
 * @return DONGLE_SUCCESS              ��ĸ����ȡ�������ݳɹ���
 */
DWORD WINAPI Dongle_GetInitDataFromMother(DONGLE_HANDLE hDongle, BYTE * pRequest, BYTE * pInitData, int * pDataLen);

/*
 * @brief  ��������������ĸ���ķ�ʽ��������������Ȩ�޼��ɵ��á�
 *
 * @param  hDongle       [in]         �򿪵ļ����������
 * @param  pInitData     [in]         �������ݡ�����Dongle_GetInitDataFromMother���ص����ڳ�ʼ�����������ݡ�
 * @param  nDataLen      [in]         ����pInitData���ݻ���������Ч���ȡ�
 *
 * @return DONGLE_SUCCESS             ���������ɹ���
 */
DWORD WINAPI Dongle_InitSon(DONGLE_HANDLE hDongle, BYTE * pInitData, int nDataLen);

/**
 * @brief  ����������Զ������˽Կ��˽Կ����Ϊ1024��RSA˽Կ����Ҫ������Ȩ�ޡ�
 *         ���ڰ�ȫ���ǣ��������ĸ���Ļ���Զ������˽Կ�������ĸ���������е�����Զ������˽Կ��ͬ����������ʧ�ܡ�
 *
 * @param  hDongle       [in]          �򿪵ļ����������
 * @param  pPriKey       [in]          RSA˽Կ��
 *
 * @return DONGLE_SUCCESS              ����Զ������˽Կ�ɹ���
 */
DWORD WINAPI Dongle_SetUpdatePriKey(DONGLE_HANDLE hDongle, RSA_PRIVATE_KEY * pPriKey);

/**
 * @brief  ����Զ���������ݰ�������Ȩ�޼��ɵ��á�
 *
 * @param  hDongle         [in]         �򿪵ļ����������
 * @param  pHID            [in]         Ӳ�����кš��������Ҫ�󶨸ò�������ΪNULL��
 * @param  nFunc           [in]         ���������͡�
 *                                      nFunc = UPDATE_FUNC_CreateFile����ʾ�����ļ���
 *                                      nFunc = UPDATE_FUNC_WriteFile��д�ļ���ֻ���������е��ļ��ſ�����д�ļ�������
 *                                      nFunc = UPDATE_FUNC_DeleteFile��ɾ���ļ���
 *                                      nFunc = UPDATE_FUNC_FileLic�������ļ���Ȩ����֧�ֿ�ִ���ļ���Ȩ������
 *                                      nFunc = UPDATE_FUNC_SeedCount����������������������
 *                                      nFunc = UPDATE_FUNC_DownloadExe��������ִ���ļ���
 *                                      nFunc = UPDATE_FUNC_UnlockUserPin�������û�PIN�����ڰ�ȫ���ǽ����û�PIN������HID��
 *                                      ��pHID����Ϊ�գ�ֻ���������������ɹ��������ɹ����û�PIN��ָ�Ϊ"12345678"��
 *                                      nFunc = UPDATE_FUNC_Deadline��ʱ��������ʹ�����ޡ�
 *
 * @param  nFileType       [in]         �ļ����͡������й��ļ�����ʱ���ļ����͡������������͸ò�����Ч��
 * @param  wFileID         [in]         �ļ�ID�������й��ļ�����ʱ���ļ�ID�������������͸ò�����Ч��
 *                                      ��wFileID=0xFFFFʱ����ʾ�������������������ݡ�
 * @param  nOffset         [in]         ƫ�����������й��ļ�����ʱ���ļ�ƫ�����������������͸ò�����Ч��
 * @param  pBuffer         [in]         �������ݡ�
 *                                      ��nFunc = UPDATE_FUNC_CreateFileʱ, pBufferָ��Ҫ�ļ������Խṹ������KEY_FILE_ATTR��
 *                                      ��nFunc = UPDATE_FUNC_WriteFileʱ, pBufferָ��Ҫд������ݡ�
 *                                      ��nFunc = UPDATE_FUNC_FileLicʱ, pBufferָ���ļ�Ȩ�޵����ݽṹ�����磺DATA_LIC��
 *                                      ��nFunc = UPDATE_FUNC_SeedCountʱ, pBufferָ��longֵ����ʾ����������������
 *                                      ��nFunc = UPDATE_FUNC_DownloadExeʱ, pBufferָ��EXE_FILE_INFO�ṹ����Dongle_DownloadExeFile�����÷����ơ�
 *                                      ��nFunc = UPDATE_FUNC_Deadlineʱ, pBufferָ��DWORDֵ����ʾ���ڵ�ʱ�䡣
 * @param  nBufferLen      [in]         ����pBuffer�Ļ�������С��
 * @param  pUPubKey        [in]         ������������RSA��Կ�������õ����ڵ�Զ������˽Կ���Ӧ����ֵ���ۺ����������Ͷ�������д��
 * @param  pOutData        [out]        ������ݡ����������������ݡ�
 * @param  pOutDataLen     [in,out]     ����pOutData�����С����������������Ч���ȡ�
 *
 * @return DONGLE_SUCCESS               �����������ɹ���
 */
DWORD WINAPI Dongle_MakeUpdatePacket(DONGLE_HANDLE hDongle, char * pHID, int nFunc, int nFileType, WORD wFileID, int nOffset, BYTE * pBuffer, int nBufferLen, RSA_PUBLIC_KEY * pUPubKey, BYTE * pOutData, int * pOutDataLen);

/**
 * @brief  ����Զ���������ݰ����ú�������ĸ����ʽ��������Dongle_MakeUpdatePacket�������Զ��������Կ��������ͬ������Ȩ�޼��ɵ��á�
 *
 * @param  hDongle         [in]         �򿪵ļ����������
 * @param  pHID            [in]         Ӳ�����кš��������Ҫ�󶨸ò�������ΪNULL��
 * @param  nFunc           [in]         ���������͡�
 *                                      nFunc = UPDATE_FUNC_CreateFile����ʾ�����ļ���
 *                                      nFunc = UPDATE_FUNC_WriteFile��д�ļ���ֻ���������е��ļ��ſ�����д�ļ�������
 *                                      nFunc = UPDATE_FUNC_DeleteFile��ɾ���ļ���
 *                                      nFunc = UPDATE_FUNC_FileLic�������ļ���Ȩ����֧�ֿ�ִ���ļ���Ȩ������
 *                                      nFunc = UPDATE_FUNC_SeedCount����������������������
 *                                      nFunc = UPDATE_FUNC_DownloadExe��������ִ���ļ���
 *                                      nFunc = UPDATE_FUNC_UnlockUserPin�������û�PIN�����ڰ�ȫ���ǽ����û�PIN������HID��
 *                                      ��pHID����Ϊ�գ�ֻ���������������ɹ��������ɹ����û�PIN��ָ�Ϊ"12345678"��
 *                                      nFunc = UPDATE_FUNC_Deadline��ʱ��������ʹ�����ޡ�
 *
 * @param  nFileType       [in]         �ļ����͡������й��ļ�����ʱ���ļ����͡������������͸ò�����Ч��
 * @param  wFileID         [in]         �ļ�ID�������й��ļ�����ʱ���ļ�ID�������������͸ò�����Ч��
 *                                      ��wFileID=0xFFFFʱ����ʾ�������������������ݡ�
 * @param  nOffset         [in]         ƫ�����������й��ļ�����ʱ���ļ�ƫ�����������������͸ò�����Ч��
 * @param  pBuffer         [in]         �������ݡ�
 *                                      ��nFunc = UPDATE_FUNC_CreateFileʱ, pBufferָ��Ҫ�ļ������Խṹ������KEY_FILE_ATTR��
 *                                      ��nFunc = UPDATE_FUNC_WriteFileʱ, pBufferָ��Ҫд������ݡ�
 *                                      ��nFunc = UPDATE_FUNC_FileLicʱ, pBufferָ���ļ�Ȩ�޵����ݽṹ�����磺DATA_LIC��
 *                                      ��nFunc = UPDATE_FUNC_SeedCountʱ, pBufferָ��longֵ����ʾ����������������
 *                                      ��nFunc = UPDATE_FUNC_DownloadExeʱ, pBufferָ��EXE_FILE_INFO�ṹ����Dongle_DownloadExeFile�����÷����ơ�
 *                                      ��nFunc = UPDATE_FUNC_Deadlineʱ, pBufferָ��DWORDֵ����ʾ���ڵ�ʱ�䡣
 * @param  nBufferLen      [in]         ����pBuffer�Ļ�������С��
 * @param  pOutData        [out]        ������ݡ����������������ݡ�
 * @param  pOutDataLen     [in,out]     ����pOutData�����С����������������Ч���ȡ�
 *
 * @return DONGLE_SUCCESS               �����������ɹ���
 */
DWORD WINAPI Dongle_MakeUpdatePacketFromMother(DONGLE_HANDLE hDongle, char * pHID, int nFunc, int nFileType, WORD wFileID, int nOffset, BYTE * pBuffer, int nBufferLen, BYTE * pOutData, int * pOutDataLen);

/**
 * @brief  Զ�����������е����ݡ�����Ȩ�޼��ɡ���������pUpdateData��һ����ֻ��ʹ��һ�Ρ�
 *         1.�������ڲ��ǰ�1024�ֽڵķֿ���Ʒ���,��������ֵ����DONGLE_SUCCESS�������жϷ��Ͳ����ء�
 *         2.�����Ҫ�������̿���,���ɵ��÷����ֿ�(ÿ��1024�ֽ�)���ͣ�����֤���ݿ��˳�򲻱����ң����ݷ��صĴ��������������̡�
 *
 * @param  hDongle         [in]     �򿪵ļ����������
 * @param  pUpdateData     [in]     �������ݡ��������ݣ���Dongle_MakeUpdatePacket����Dongle_MakeUpdatePacketFromMother����
 * @param  nDataLen        [in]     ����pUpdateData�Ĵ�С������Ϊ1024����������
 *
 * @return DONGLE_SUCCESS           �����ɹ���
 */
DWORD WINAPI Dongle_Update(DONGLE_HANDLE hDongle, BYTE * pUpdateData, int nDataLen);


//������
#define   DONGLE_SUCCESS                       0x00000000          // �����ɹ�
#define   DONGLE_NOT_FOUND                     0xF0000001          // δ�ҵ�ָ�����豸
#define   DONGLE_INVALID_HANDLE				   0xF0000002		   // ��Ч�ľ��
#define   DONGLE_INVALID_PARAMETER			   0xF0000003		   // ��������
#define   DONGLE_COMM_ERROR					   0xF0000004		   // ͨѶ����
#define   DONGLE_INSUFFICIENT_BUFFER		   0xF0000005		   // �������ռ䲻��
#define   DONGLE_NOT_INITIALIZED               0xF0000006		   // ��Ʒ��δ��ʼ�� (��û����PID)
#define   DONGLE_ALREADY_INITIALIZED		   0xF0000007		   // ��Ʒ�Ѿ���ʼ�� (��������PID)
#define   DONGLE_ADMINPIN_NOT_CHECK			   0xF0000008		   // ����������û����֤
#define   DONGLE_USERPIN_NOT_CHECK			   0xF0000009		   // �û�����û����֤
#define   DONGLE_INCORRECT_PIN				   0xF000FF00		   // ���벻��ȷ (��2λָʾʣ�����)
#define   DONGLE_PIN_BLOCKED				   0xF000000A		   // PIN��������
#define   DONGLE_ACCESS_DENIED				   0xF000000B		   // ���ʱ��ܾ� 
#define   DONGLE_FILE_EXIST					   0xF000000E		   // �ļ��Ѵ���
#define   DONGLE_FILE_NOT_FOUND				   0xF000000F		   // δ�ҵ�ָ�����ļ�
#define	  DONGLE_READ_ERROR                    0xF0000010          // ��ȡ���ݴ���
#define	  DONGLE_WRITE_ERROR                   0xF0000011          // д�����ݴ���
#define	  DONGLE_FILE_CREATE_ERROR             0xF0000012          // �����ļ�����
#define	  DONGLE_FILE_READ_ERROR               0xF0000013          // ��ȡ�ļ�����
#define	  DONGLE_FILE_WRITE_ERROR              0xF0000014          // д���ļ�����
#define	  DONGLE_FILE_DEL_ERROR                0xF0000015          // ɾ���ļ�����
#define   DONGLE_FAILED                        0xF0000016          // ����ʧ��
#define   DONGLE_CLOCK_EXPIRE                  0xF0000017          // ������ʱ�ӵ���
#define   DONGLE_ERROR_UNKNOWN		           0xFFFFFFFF		   // δ֪�Ĵ���

#ifdef __cplusplus 
}
#endif

#endif