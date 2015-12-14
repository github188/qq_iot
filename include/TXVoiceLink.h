#ifndef __TX_DECODE_ENGINE__H__
#define __TX_DECODE_ENGINE__H__ 
#include "TXSDKCommonDef.h"
/***********************************************************************/
/*
*   ����������飺
*   ���ܺ�Ч�õ�ͬ��smartlink��ʹ�÷�ʽ���ƣ���Ҫ��������������Ϊý�飬ͨ����Q��·������ssid�����봫�͸��豸��
*
*	�豸Ҫ��
*		�豸������˷磬��Ͳ����ʴ��ڻ����8000Hz����������16λ������ʹ�õ�Ƶ����1400~3900Hz
*	
*	��QҪ��
*		�汾 >= 5.8 ��ʼ����ʽ����voicelink
*
*	����˵����
*		 
         --------------------------------------- 
        |          ��ʼ����˷�ģ��               |
         ---------------------------------------
		
         ---------------------------------------
        |          ��ʼ����������ģ��             |
         ---------------------------------------
		
         ---------------------------------------
        |       ѭ����ȡ¼�����ݽ�������ģ�����	    |
        |       ����tx_fill_audio ����           |
        |       ʶ��ɹ���ص� VL_FUNC_NOTIFY     |
        |       ���Խ���ѭ��                      |
         ---------------------------------------

         ---------------------------------------
        |           ����ʼ����������ģ��           |
         ---------------------------------------
		
         ---------------------------------------
        |           ����ʼ����˷�ģ��             |
         ---------------------------------------

�ر�˵�����ص����������ڽ�����ssid���������ã��ص�������ؽ���������̡����ǧ�򲻿��ڻص������е���
��������ģ��ķ���ʼ������ tx_uninit_decoder��

�����ɼ������ַ�ʽ
1. ͨ������readPCMdata����ȥ��ȡ
2. ����һ���ص����� onPCMdata���ڻص��л�ȡPCM����

���ڷ�ʽ1. ���Զ���һ��ȫ�ֵ�g_finish��������ʼ��Ϊfalse��0�����ص�����Ϊtrue��1����������̿��Բ��ռ���˵��������
α��������


start_voicelink
{
  tx_init_decoder
  initaudio
  g_finish = 0
  while��!g_finish)
  {
    readPCMdata
    tx_fill_audio
  }
  uninitaudio
  tx_uninit_decoder  
}


���ڷ�ʽ2. Ҳ�Ƕ���һ��ȫ�ֵ�g_finish������
α��������
start_voicelink
{
  tx_init_decoder
  g_finish = 0
  initaudio(onPCMdata)
}

onPCMdata
{
  if (g_finish)
  {
     uninitaudio
     tx_uninit_decoder
     return
  }
  tx_fill_audio
}

�ص�����α������
on_voicelink_notify
{
  connect_ap
  g_finish = 1
}
*/   
/***********************************************************************/


CXX_EXTERN_BEGIN

enum RESULT 
{
	ERROR_NULL 					= 0,
	ERROR_INITED				= 1,
	ERROR_NO_MEMORY 			= 2,
	ERROR_CREATE_LOCK_FAIL 		= 3,
	ERROR_CREATE_THREAD_FAIL	= 4,  
};

#define MAX_SSID_LEN      128
#define MAX_PSWD_LEN      128
#define MAX_IP_LEN        16

typedef struct
{
	char                sz_ssid[MAX_SSID_LEN];
	char                sz_password[MAX_PSWD_LEN];
    char                sz_ip[MAX_IP_LEN];
    unsigned short      sh_port;
} tx_voicelink_param;


typedef void (*VL_FUNC_NOTIFY)(tx_voicelink_param* pparam);

/**
*  ��ʼ����������ģ��
*   samplerate: ��˷�Ĳ����� >= 8000Hz��������Խ�߼�����Խ�󣬽��鲻Ҫʹ���²������ز���,����Ӱ������
*
*/
SDK_API int tx_init_decoder(VL_FUNC_NOTIFY func, int samplerate);

/**
*  ����ʼ����������ģ��
*
*/
SDK_API void tx_uninit_decoder();

/*
*  ����ΪPCM��ʽ���ᱻ�������ڲ� buffer �����У�����ʶ��
*  nlen: ��aduio����ĳ��ȣ������������� 
*  (ͨ��¼�����20ms��50ms��100ms��ע���ڲ���һ������4096��������Ļ���buffer��nlen��ֵ���ܳ���4096)
*/
SDK_API void tx_fill_audio(signed short* audio, int nlen);



CXX_EXTERN_END

#endif