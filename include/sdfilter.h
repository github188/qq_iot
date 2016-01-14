/**
 * @file	sdfilter.h
 * @brief	Anyka Sound Device Module interfaces header file.
 *
 * This file declare Anyka Sound Device Module interfaces.\n
 * Copyright (C) 2008 Anyka (GuangZhou) Software Technology Co., Ltd.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @version V0.0.1
 * @ref
 */

#ifndef __SOUND_FILTER_H__
#define __SOUND_FILTER_H__

#include "medialib_global.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup Audio Filter library
 * @ingroup ENG
 */
/*@{*/

/* @{@name Define audio version*/
/**	Use this to define version string */	
#define AUDIO_FILTER_VERSION_STRING		(T_U8 *)"AudioFilter Version V1.5.02_svn4716"
/** @} */

#ifdef _WIN32
#define _SD_FILTER_EQ_SUPPORT    
#define _SD_FILTER_WSOLA_SUPPORT   
#define _SD_FILTER_3DSOUND_SUPPORT
#define _SD_FILTER_RESAMPLE_SUPPORT
#define _SD_FILTER_DENOISE_SUPPORT
#define _SD_FILTER_AGC_SUPPORT
#define _SD_FILTER_VOICECHANGE_SUPPORT
#define _SD_FILTER_PCMMIXER_SUPPORT
#define _SD_FILTER_3DENHANCE_SUPPORT
#define _SD_FILTER_MVBASS_SUPPORT
#define _SD_FILTER_ASLC_SUPPORT
// #define _SD_FILTER_TONEDETECTION_SUPPORT
#define _SD_FILTER_VOLUMECONTROL_SUPPORT
#endif

#if defined ANDROID
#define _SD_FILTER_EQ_SUPPORT    
#define _SD_FILTER_WSOLA_SUPPORT   
#define _SD_FILTER_3DSOUND_SUPPORT
#define _SD_FILTER_RESAMPLE_SUPPORT
#define _SD_FILTER_DENOISE_SUPPORT
#define _SD_FILTER_AGC_SUPPORT
//#define _SD_FILTER_VOICECHANGE_SUPPORT
#endif /*#if defined ANDROID*/

typedef enum
{
	_SD_FILTER_UNKNOWN ,
	_SD_FILTER_EQ ,
	_SD_FILTER_WSOLA ,
	_SD_FILTER_RESAMPLE,
	_SD_FILTER_3DSOUND,
	_SD_FILTER_DENOICE,
	_SD_FILTER_AGC,
	_SD_FILTER_VOICECHANGE,
    _SD_FILTER_PCMMIXER,
    _SD_FILTER_3DENHANCE,
    _SD_FILTER_MVBASS,
    _SD_FILTER_ASLC,
    _SD_FILTER_TONE_DETECTION,
    _SD_FILTER_VOLUME_CONTROL
}T_AUDIO_FILTER_TYPE;

typedef enum
{
	_SD_EQ_MODE_NORMAL,
	_SD_EQ_MODE_CLASSIC,
	_SD_EQ_MODE_JAZZ,
    _SD_EQ_MODE_POP,
    _SD_EQ_MODE_ROCK,
    _SD_EQ_MODE_EXBASS,
    _SD_EQ_MODE_SOFT,
    _SD_EQ_USER_DEFINE,
} T_EQ_MODE;

//to define the filter type
typedef enum
{
    FILTER_TYPE_NO ,
    FILTER_TYPE_HPF ,
    FILTER_TYPE_LPF ,
    FILTER_TYPE_HSF ,
    FILTER_TYPE_LSF ,
    FILTER_TYPE_PF1    //PeaKing filter
}T_EQ_FILTER_TYPE;


#define _SD_EQ_MAX_BANDS 10

typedef enum
{
	_SD_WSOLA_0_5 ,
	_SD_WSOLA_0_6 ,
	_SD_WSOLA_0_7 ,
	_SD_WSOLA_0_8 ,
	_SD_WSOLA_0_9 ,
	_SD_WSOLA_1_0 ,
	_SD_WSOLA_1_1 ,
	_SD_WSOLA_1_2 ,
	_SD_WSOLA_1_3 ,
	_SD_WSOLA_1_4 ,
	_SD_WSOLA_1_5 ,
	_SD_WSOLA_1_6 ,
	_SD_WSOLA_1_7 ,
	_SD_WSOLA_1_8 ,
	_SD_WSOLA_1_9 ,
	_SD_WSOLA_2_0 
}T_WSOLA_TEMPO;

typedef enum
{
	_SD_WSOLA_ARITHMATIC_0 , // 0:WSOLA, fast but tone bab
	_SD_WSOLA_ARITHMATIC_1   // 1:PJWSOLA, slow but tone well
}T_WSOLA_ARITHMATIC;


typedef enum
{
    RESAMPLE_ARITHMETIC_0 = 0,
    RESAMPLE_ARITHMETIC_1
}RESAMPLE_ARITHMETIC;

typedef enum
{
    _SD_OUTSR_UNKNOW = 0,
	_SD_OUTSR_48KHZ = 1,
	_SD_OUTSR_44KHZ,
	_SD_OUTSR_32KHZ,
	_SD_OUTSR_24KHZ,
	_SD_OUTSR_22KHZ,
	_SD_OUTSR_16KHZ,
	_SD_OUTSR_12KHZ,
	_SD_OUTSR_11KHZ,
	_SD_OUTSR_8KHZ
}T_RES_OUTSR;

typedef enum
{
    PITCH_NORMAL = 0,
    PITCH_CHILD_VOICE ,
    PITCH_MACHINE_VOICE,
    PITCH_ECHO_EFFECT,
    PITCH_ROBOT_VOICE,
    PITCH_RESERVE
}T_PITCH_MODES;


typedef struct
{
    int num;
    struct 
    {
        int x;
        int y;
    }stone[10];
    int lookAheadTime;  //ms
    int gainAttackTime;  //ms
    int gainReleaseTime;  //ms
}T_FILTER_MILESTONE;

typedef struct
{
	MEDIALIB_CALLBACK_FUN_MALLOC			Malloc;
	MEDIALIB_CALLBACK_FUN_FREE				Free;
	MEDIALIB_CALLBACK_FUN_PRINTF			printf;
	MEDIALIB_CALLBACK_FUN_RTC_DELAY			delay;
}T_AUDIO_FILTER_CB_FUNS;


typedef struct
{
	T_U32	m_Type;				//media type
	T_U32	m_SampleRate;		//sample rate, sample per second
	T_U16	m_Channels;			//channel number
	T_U16	m_BitsPerSample;	//bits per sample 

	union {
		struct {
			T_EQ_MODE eqmode;
            /* 
            ����������ֵ(db)��ע�⣺preGain ��ֵ��ʽΪ (T_S16)(x.xxx*(1<<10))
            */
            T_S16 preGain;      //-12 <= x.xxx <= 12
			
			// For User Presets
			T_U32 bands;      //1~10
			T_U32 bandfreqs[_SD_EQ_MAX_BANDS];
            /* 
            ����ÿ��Ƶ��������ֵ��ע�⣺bandgains ��ֵ��ʽΪ (T_S16)(x.xxx*(1<<10))
            */
			T_S16 bandgains[_SD_EQ_MAX_BANDS];  //-12 <= x.xxx <= 12
            /* 
            ����ÿ��Ƶ����Qֵ��ע�⣺
            1. bandQ��ֵ��ʽΪ (T_U16)(x.xxx*(1<<10))
            2. bandQ�������Ϊ0������ÿ��ڲ���Ĭ��ֵΪ (T_U16)(1.22*(1<<10))
            3. x.xxx < ������/(2*��Ƶ��������Ƶ��), ����x.xxxֵ����С��64.000
            */
            T_U16 bandQ[_SD_EQ_MAX_BANDS];     // q < sr/(2*f)
            T_U16 bandTypes[_SD_EQ_MAX_BANDS];		

            /*** for ffeq dc_remove ***/
            T_U8     dcRmEna;
            T_U32    dcfb;

            /*** for EQ aslc ***/
            T_U8   aslcEna;
            T_U16  aslcLevelMax;
		} m_eq;
		struct {
			T_WSOLA_TEMPO tempo;
            T_WSOLA_ARITHMATIC arithmeticChoice;
		} m_wsola;
		struct{
			T_U8 is3DSurround;
		}m_3dsound;
		struct {
			//Ŀ������� 1:48k 2:44k 3:32k 4:24K 5:22K 6:16K 7:12K 8:11K 9:8K
			T_RES_OUTSR  outSrindex;

			//����������볤��(bytes)��openʱ��Ҫ������̬��������ݡ�
			//�����������ز���ʱ�����볤�Ȳ��ܳ������ֵ
			T_U32 maxinputlen; 

            // ����outSrindex�������ֻ����enum�еļ�������ϣ����Ŀ���������enum֮���ֵ��ʱ�������������
            // ����������ǲ����ʵ������ˣ�ֱ����Ŀ������ʵ�ֵ������8000�� 16000 ...
            // ����������������Ч����������outSrindex=0
            T_U32 outSrFree; 
            
            T_U32 reSampleArithmetic;
		}m_resample;
		struct{
			T_U16 AGClevel;  // make sure AGClevel < 32767
            /* used in AGC_1 */
            T_U32  max_noise;
            T_U32  min_noise;
            /* used in AGC_2 */
            T_U8  noiseReduceDis;  // �Ƿ������Դ��Ľ��빦��
            T_U8  agcDis;  // �Ƿ������Դ���AGC����
            /*
            agcPostEna����agcDis==0������£������Ƿ�������AGC2��������AGC��
            0����ʾ�����ڿ�������agc����filter_control�������������Ѿ�����agc�ģ�
            1: ��ʾ������ֻҪ����agc��gainֵ������Ҫ������agc����������agc������ĵ����ߺ�������
            */
            T_U8  agcPostEna;  
            T_U16 maxGain;  // ���Ŵ���
            T_U16 minGain;  // ��С�Ŵ���
            T_U32 dc_freq;  // hz
            T_U32 nr_range; // 1~300,Խ�ͽ���Ч��Խ����
		}m_agc;
		struct{
			T_U32 ASLC_ena;  // 0:disable aslc;  1:enable aslc
			T_U32 NR_Level;  //  0 ~ 4 Խ��,����Խ��
		}m_NR;
		struct{
			T_PITCH_MODES pitchMode;  // 
		}m_pitch;
        struct{
            /* 
            ����������ֵ(db)��
            ע�⣺preGain ��ֵ��ʽΪ (T_S16)(x.xxx*(1<<10))�� 
            ���� -12 <= x.xxx <= 12
            */
            T_S16 preGain;  
            T_S16 cutOffFreq;
            /* 
            ����3D��ȣ�
            ע��: depth��ֵ��ʽΪ (T_S16)(x.xxx*(1<<10)), 
            ���� -1 < x.xxx < 1
            */
            T_S16 depth;   
            /*** for 3D Enhance's aslc ***/
            T_U8   aslcEna;
            T_U16  aslcLevelMax;
		}m_3DEnhance;
        struct{
            /* 
            ����������ֵ(db)��
            ע�⣺preGain ��ֵ��ʽΪ (T_S16)(x.xxx*(1<<10))�� 
            ���� -12 <= x.xxx <= 12
            */
            T_S16 preGain;
            T_S16 cutOffFreq;  
            /* 
            ������ǿ���ȣ�
            ע��: bassGain ��ֵ��ʽΪ (T_S16)(x.xxx*(1<<10)), 
            ���� 0 < x.xxx < 12
            */
            T_S16 bassGain;
            /*** for MVBass's aslc ***/
            T_U8   aslcEna;
            T_U16  aslcLevelMax;
		}m_mvBass;
        struct{
            T_BOOL aslcEna;
            T_U16  aslcLimitLevel;  //�޷���ֵ
            T_U16  aslcStartLevel; //Ҫ�޷�����ʼ����
            T_U16  jointChannels;
            /* 
            ��������, ��Χ0~32767, ��ӦС��0.0~31.999��
            512 �൱��С��0.5��������Сһ��
            1024�൱��С��1.0������������
            2048�൱��С��2.0�������Ŵ�����
            */
            T_U16 preVolume; 
            /* Ϊ�˷�ֹ�����任���̲���pipa��������������ƽ��������������ƽ���Ĺ���ʱ�� */
            T_U16 volSmoothTime;  //ms
		}m_aslc;
        struct{
            /* 
            ��������, ��Χ0~32767, ��ӦС��0.0~31.999��
            512 �൱��С��0.5��������Сһ��
            1024�൱��С��1.0������������
            2048�൱��С��2.0�������Ŵ�����
            �������õ�����ֵ��Ҫ����1024����Ϊ�������ܻᵼ�����������������������
            */
            T_U16 volume; 
            /* Ϊ�˷�ֹ�����任���̲���pipa��������������ƽ��������������ƽ���Ĺ���ʱ�� */
            T_U16 volSmoothTime;  //ms
		}m_volumeControl;
	}m_Private;
}T_AUDIO_FILTER_IN_INFO;

typedef struct
{
	T_AUDIO_FILTER_CB_FUNS	cb_fun;
	T_AUDIO_FILTER_IN_INFO	m_info;
    T_AUDIO_CHIP_ID         chip;
}T_AUDIO_FILTER_INPUT;

typedef struct
{
	T_VOID *buf_in;
	T_U32 len_in;
	T_VOID *buf_out;
	T_U32 len_out;
    T_VOID *buf_in2;  //for mix pcm samples
	T_U32 len_in2;
}T_AUDIO_FILTER_BUF_STRC;

//////////////////////////////////////////////////////////////////////////

/**
 * @brief	��ȡ��Ч�����汾��Ϣ.
 * @author	Deng Zhou
 * @date	2009-04-21
 * @param	[in] T_VOID
 * @return	T_S8 *
 * @retval	������Ч�����汾��
 */
T_S8 *_SD_GetAudioFilterVersionInfo(void);

/**
 * @brief	��ȡ��Ч��汾��Ϣ, ����֧����Щ����.
 * @author  Tang Xuechai
 * @date	2014-05-05
 * @param	[in] T_AUDIO_FILTER_CB_FUNS
 * @return	T_S8 *
 * @retval	���ؿ�汾��
 */
T_S8 *_SD_GetAudioFilterVersions(T_AUDIO_FILTER_CB_FUNS *cb);

/**
 * @brief	����Ч�����豸.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @param	[in] filter_input:
 * ��Ч���������ṹ
 * @return	T_VOID *
 * @retval	������Ч���ڲ��ṹ��ָ�룬�ձ�ʾʧ��
 */
T_VOID *_SD_Filter_Open(T_AUDIO_FILTER_INPUT *filter_input);

/**
 * @brief	��Ч����.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @param	[in] audio_filter:
 * ��Ч�����ڲ����뱣��ṹ
 * @param	[in] audio_filter_buf:
 * �������buffer�ṹ
 * @return	T_S32
 * @retval	������Ч�⴦������Ƶ���ݴ�С����byteΪ��λ
 */
T_S32 _SD_Filter_Control(T_VOID *audio_filter, T_AUDIO_FILTER_BUF_STRC *audio_filter_buf);

/**
 * @brief	�ر���Ч�����豸.
 * @author	Deng Zhou
 * @date	2008-04-10
 * @param	[in] audio_decode:
 * ��Ч�����ڲ����뱣��ṹ
 * @return	T_S32
 * @retval	AK_TRUE :  �رճɹ�
 * @retval	AK_FLASE :  �ر��쳣
 */
T_S32 _SD_Filter_Close(T_VOID *audio_filter);

/**
 * @brief	������Ч����:�����ٶ�,EQģʽ.
 *          ���m_SampleRate,m_BitsPerSample,m_Channels������1��Ϊ0,�򲻸ı��κ���Ч,����AK_TRUE
 * @author	Wang Bo
 * @date	2008-10-07
 * @param	[in] audio_filter:
 * ��Ч�����ڲ����뱣��ṹ
 * @param	[in] info:
 * ��Ч��Ϣ����ṹ
 * @return	T_S32
 * @retval	AK_TRUE :  ���óɹ�
 * @retval	AK_FLASE :  �����쳣
 */
T_S32 _SD_Filter_SetParam(T_VOID *audio_filter, T_AUDIO_FILTER_IN_INFO *info);


/**
 * @brief	����ASLCģ�������ֵ.
 * @author	Tang Xuechai
 * @date	2015-02-04
 * @param	[in] audio_filter: ��Ч�����ڲ����뱣��ṹ
 * @param	[in] volume: ���õ�����ֵ����Χ0~32767, ��ӦС��0.0~31.999��
 *                          512 �൱��С��0.5��������Сһ��
 *                          1024�൱��С��1.0������������
 *                          2048�൱��С��2.0�������Ŵ�����
 * @return	T_S32
 * @retval	AK_TRUE :  ���óɹ�
 * @retval	AK_FLASE :  �����쳣
 */
T_S32 _SD_Filter_SetAslcVolume(T_VOID *audio_filter, T_U16 volume);

/**
 * @brief	����ASLCģ����޷�����.
 * @author	Tang Xuechai
 * @date	2015-04-17
 * @param	[in] audio_filter: ��Ч�����ڲ����뱣��ṹ
 * @param	[in] fmileStones: ASLC���޷����߲���������ο���Ƶ��ӿ�˵���ĵ�
 * @return	T_S32
 * @retval	AK_TRUE :  ���óɹ�
 * @retval	AK_FLASE :  �����쳣
 */
T_S32 _SD_Filter_SetAslcMileStones(T_VOID *audio_filter, T_FILTER_MILESTONE *fmileStones);

/**
 * @brief	�����ز���
 * @author	Tang_Xuechai
 * @date	    2013-07-03
 * @param	[in] audio_filter:
 *               ��Ч�����ڲ����뱣��ṹ
 * @param	[out] dstData 
 *               �����pcm����
 * @param	[in] srcData:
 *               �����pcm����
 * @param	[in] srcLen 
 *               ����pcm���ݵ�byte��
 * @return	T_S32
 * @retval	>=0 :  �ز���������pcm���ݵ�byte��
 * @retval	<0  :  �ز���ʧ��
 */
T_S32  _SD_Filter_Audio_Scale(T_VOID *audio_filter, T_S16 dstData[], T_S16 srcData[], T_U32 srcLen);


/**
* @brief	��EQ��Ƶ�����תΪʱ�����.
* @author	Tang Xuechai
* @date	    2015-03-24
* @param	[in] audio_filter:
*           ��Ч�����ڲ�����ṹ����_SD_Filter_Open�ķ���ָ��
* @param	[in] info:
*           ��Ч��Ϣ����ṹ
* @return	T_VOID *
* @retval	����EQ���ڲ���ȡ��ʱ�������ָ�룬�ձ�ʾʧ��
*/
T_VOID *_SD_Filter_GetEqTimePara(T_VOID *audio_filter, T_AUDIO_FILTER_IN_INFO *info);

/**
* @brief	�ѵ�ǰҪʹ�õ�EQʱ��������ݸ�EQ��.
* @author	Tang Xuechai
* @date	    2015-03-24
* @param	[in] audio_filter:
*           ��Ч�����ڲ�����ṹ����_SD_Filter_Open�ķ���ָ��
* @param	[in] peqTime:
*           ʱ�����ָ��
* @return	T_S32
* @retval	AK_TRUE :  ���óɹ�
* @retval	AK_FLASE:  �����쳣
*/
T_S32 _SD_Filter_SetEqTimePara(T_VOID *audio_filter, T_VOID *peqTime);

/**
* @brief	�ͷ�EQʱ�����ռ�õĿռ�.
* @author	Tang Xuechai
* @date	    2015-03-24
* @param	[in] audio_filter:
*           ��Ч�����ڲ�����ṹ����_SD_Filter_Open�ķ���ָ��
* @param	[in] peqTime:
*           ʱ�����ָ��
* @return	T_S32
* @retval	AK_TRUE :  ���óɹ�
* @retval	AK_FLASE:  �����쳣
*/
T_S32 _SD_Filter_DestoryEqTimePara(T_VOID *audio_filter, T_VOID *peqTime);


/**
 * @brief	������������ģ�������ֵ.
 * @author	Tang Xuechai
 * @date	2015-08-11
 * @param	[in] audio_filter: ��Ч�����ڲ����뱣��ṹ
 * @param	[in] volume: ���õ�����ֵ����Χ0~32767, ��ӦС��0.0~31.999��
 *                          512 �൱��С��0.5��������Сһ��
 *                          1024�൱��С��1.0������������
 *                          2048�൱��С��2.0�������Ŵ�����
 *                       ���飺���õ�����ֵ��Ҫ����1024����Ϊ�������ܻᵼ�����������������������
 * @return	T_S32
 * @retval	AK_TRUE :  ���óɹ�
 * @retval	AK_FLASE :  �����쳣
 */
T_S32 _SD_Filter_SetVolume(T_VOID *audio_filter, T_U16 volume);

#ifdef __cplusplus
}
#endif

#endif
/* end of sdfilter.h */
/*@}*/
