#ifndef __TX_WIFI_SYNC_H__
#define __TX_WIFI_SYNC_H__

#include "TXSDKCommonDef.h"

CXX_EXTERN_BEGIN

/*
 *                                    SmartLink API���
 * SmartLink API�����׽ӿڣ�1.0��2.0��Ŀǰ�Ƽ�ʹ��2.0�ӿڣ�
 * 1.0�ӿ��ṩԭʼ��ץ����������Ҫ�û�ȷ��ԭʼ����offsetƫ�Ƶ������Ϣ���ӿڱȽ϶࣬ʹ��������΢�е㸴�ӣ�
 * 2.0�ӿ�����1.0�ӿڻ����ϵķ�װ������ض�WIFIоƬ�����Ż���ʹ�������ǳ��򵥣�����Ŀǰֻ֧������оƬ��������Ҫ������ϵ����Helper���Ӷ��ض�оƬ��֧�֣�
 *
 */

////////////////////////////////////////////////////////////////////////////////////////////////
//
//              SmartLink1.0 API
//
////////////////////////////////////////////////////////////////////////////////////////////////

/*                             SmartLink1.0�ӿ�ʹ��˵��
 * ���ڲ�֪��wifisync ���Ͷ����Ǹ�channel�Ϸ����������Ҫ��ÿ�����ܵ�channel��ץ�����������ظ�������̣�ֱ���ص�֪ͨ��
 *
 * step1. init_wifi_sync��ʼ��smartlink
 * step2. ��������ģʽ�������ŵ��е���һ���ŵ�n��1~13����ͬʱ����wifi_sync_notify_hop(n), ���ڵ�ǰ�ŵ�ͣ��200ms
 * step3. ץ������fill_80211_frame����
 * 	 step2.1 ��fill_80211_frame����QLERROR_LOCKʱ��������(*pChannel)�뵱ǰ�ŵ��Ƿ�һ�£���һ����������ŵ�(*pChannel)��ͬʱ������ǰ�ŵ�ͣ��10000ms��תstep3
 *   step2.2 ��fill_80211_frame����QLERROR_HOPʱ��תstep2
 *   step2.3 ��fill_80211_frame����QLERROR_SUCCESSʱ���ڲ��ص���FUNC_NOTIFY����������ssid��pwd��������AP��תstep4
 * step4. ����ʼ������destory_wifi_sync
 */

/** init_wifi_sync & start_smartlink�ķ���ֵʹ�ô�enum����
 */
enum wifisyncerror {
	QLERROR_INIT_SUCCESS        = 0,         //suc
	QLERROR_MEMORY_ALLOC  		= 1,        //�ڴ����ʧ��
	QLERROR_NOTIFY_NULL		  	= 2,        //FUNC_NOTIFY�ص�����Ϊ��
	QLERROR_PARAM_KEY       	= 3,        //key����ΪNULL
	QLERROR_PARAM_KEY_LEN   	= 4,        //key���Ȳ��Ϸ�
	QLERROR_OPENSSL_LOAD_FAILED = 5,        //����openssl��ʧ��
	QLERROR_HOP_NULL			= 6,		//��Ƶ�ص�����Ϊ��
	QLERROR_SZIFNAME_INVALID	= 7,		//szifname���Ϸ�
	QLERROR_WIFICHIP_NOTSUPPORT = 8,		//��WIFI CHIP 2.0�ӿڲ�֧��
	QLERROR_INIT_OTHER		    = 9,		//��������
};

/** fill_80211_frame�ķ���ֵʹ�ô�enum����
 */
enum fill80211relust {
	QLERROR_SUCCESS 		= 0,		//�����ɹ�
	QLERROR_HOP				= 1,		//�յ��˷���ֵ��ʾ��ǰ�ŵ��������������л���һ���ŵ�
	QLERROR_LOCK    		= 2,        //�յ��˷���ֵ��ʾ�ǵ�ǰ�ŵ�������

	//������Щ����ֵ������ʹ�ã����ŵ������ŵ�������������ֵȷ��,BCAST��ʾBraodcast��MCAST��ʾMulticast
	QLERROR_OTHER			= 3,    //��������
	QLERROR_DECRYPT_FAILED  = 4,    //���ܳ���
	QLERROR_NEED_INIT     	= 5,    //wifi syncû�г�ʼ��
	QLERROR_VERSION        	= 6,    //SDK�汾����Ӧ
	QLERROR_START_FRAME     = 7, 	//��Ч�İ�

    //Broadcast relative
    QLERROR_BCAST_NOT_FRAME	= 8, 	//���ǹ㲥��
    QLERROR_BCAST_CALC_C	= 9,	//�ɹ�����Cֵ
    QLERROR_BCAST_ONE_DATA	= 10,	//������һ���㲥����

    //Multicast relative
    QLERROR_MCAST_NOT_FRAME	= 11,  //�����鲥��
    QLERROR_MCAST_ONE_DATA	= 12,  //������һ���鲥����
};

#define QLMAX_SSID_LEN      128
#define QLMAX_PSWD_LEN      128
#define QLMAX_IP_LEN        16

typedef struct
{
	char                sz_ssid[QLMAX_SSID_LEN];      // AP�˺�����
	char                sz_password[QLMAX_PSWD_LEN];  // AP����
    char                sz_ip[QLMAX_IP_LEN];          // ���Ͷ�IP��ַ���豸����AP���������Ͷ�ackʱ�õ�
    unsigned short      sh_port;                      // ���Ͷ˶˿ڣ�  �豸����AP���������Ͷ�ackʱ�õ�
    unsigned char       sz_bssid[6];                  // AP��48λMAC��ַ�� ��Ҫ������������SSID�㲥��AP�����̿ɸ���ʵ�����ѡ���Ƿ�ʹ��
	                                                  // ����豸����ɨ�赽beacon֡��ͨ��beancon�е���Ϣ����AP��ɺ��Դ˳�Ա
} tx_wifi_sync_param;


/**  ���l��1~13,���ݴ�����ŵ�ֵ�����ƶ����ŵ�����Ҫ����ʵ����Ƶ����
 @param nchannel  Ҫ��ת���ŵ�ֵ
 */
typedef void (* FUNC_HOP)(int nchannel);


/**  smartlink��ɺ�ص�����Ҫ�����Լ�ʵ�֣���Ҫ�Ƿ�������·������Ҫ��SSID��PWD
 @param pqq_link_param  ����·����SSID��������Ϣ���ص�������ᱻ����
 @param puserdata  ���̴��������ָ��
 */
typedef void (* FUNC_NOTIFY)(tx_wifi_sync_param *pwifi_sync_param, void *puserdata);


/**  ��ʼ�� wifi sync
 @param fNotify  wifi sync ��ɺ�ص�֪ͨ����
 @param szSN  16λ���豸SN
 @param puserdata  �����û�����,�ɵ��÷�����,FUNC_NOTIFY�ص�������ʱ��ᴫ��ȥ
 @return  QLERROR_INIT_SUCCESS��ʾ�ɹ���������������μ�wifisyncerror����
 */
SDK_API int init_wifi_sync(FUNC_NOTIFY fNotify, char *szSN, void *puserdata);


/**  �ú����Զ��жϲ��ռ����ݣ����ж��ռ����ݽ���ʱ�����FUNC_NOTIFY�ص���������SSID��PWD����
 @note
     *pChannelΪ���Σ�����fill_80211_frame����ֵΪQLERROR_LOCKʱ������ֵΪ��0ʱ��Ч
 @param buff  ��������ץ����802.11���ݰ�
 @param nlen  �������������ݰ�����
 @param npackoffset  ����MPDUƫ��λ�ã���macframe��ʼλ�ã�����оƬ��ͬ����ͬ��������ѯwifiоƬԭ��
 @param pChannel[out]  ���Σ�0��ʾ��Чֵ��1~13��ʾ�ڲ���������ȷ�ŵ�������fill_80211_frame����ֵΪQLERROR_LOCKʱ��Ч
				       ��fill_80211_frame����ֵΪQLERROR_LOCK, ��*pChannel����ֵΪ��0ʱ����ʾ�ڲ�����������ȷ�ŵ����������ǰ�ŵ���һ�����������*pChannel�ŵ��ϣ�ͬʱ����
  				       ��fill_80211_frame����ֵ����QLERROR_LOCKʱ�������*pChannel�ķ���ֵ
 @return  QLERROR_SUCCESS��������ɹ�
          QLERROR_HOP������Ҫ�����л�����һ���ŵ�
          QLERROR_LOCK����������ŵ�
          ��������ֵ��������ʹ��
 */
SDK_API int fill_80211_frame(const unsigned char *buff, int nlen, int npackoffset, int* pChannel);


/**  �ⲿ�л��ŵ�ʱ֪ͨ��sdk�ڲ�����һЩ������
 @note
     ��fill_80211_frame����QLERROR_LOCKʱ��������ȷ���ŵ���������¾Ͳ�Ҫ���ô˺���
 @param channel  ��ǰ�л����ŵ�ֵ
 */
SDK_API void wifi_sync_notify_hop(int channel);


/**  ����ʼ�� wifi sync
 */
SDK_API void destory_wifi_sync();



///////////////////////////////////////////////////////////////////////////////
//     
//        SmartLink2.0 API
//
///////////////////////////////////////////////////////////////////////////////

/*
 *                                    SmartLink2.0�ӿ�ʹ��˵��
 * step1. ��ȷ��ʹ�õ�оƬ�Ƿ������Enum WifiChipType���棬�ݲ�����С�ͺţ�������������ϵ����helper�����ʹ�õ�оƬ��������WifiChipType��������ʹ��1.0�ӿڣ�
 * step2. ����start_smartlink�ӿڣ�������ز�����������μ�����˵����
 * step3. �����ɹ��ص�FUNC_NOTIFY���õ�SSID��PWD�����̴�ʱ����AP��
 * step4. ֻ������Ҫǿ��ֹͣsmartlinkʱ�����ܹ�����stop_smartlink�����������������ɹ���
 */

typedef enum {
	WCT_REALTEK8188,
	WCT_MTK7601,
} WifiChipType;


/**  start_smartlink������һ���߳�->ץ��->����->�ص�֪ͨ->�����߳�
 @param fHopping  ���������Ƶ�ص�����
 @param szifName  ��������������ƣ��������� wlan0
 @param fNotify   ���������ɺ�ص�֪ͨ����
 @param szSN  �������16�ֽڵ�SN,ȷ���Ͷ�ά�� http://iot.qq.com/add?pid=XXXX&sn=XXXX �е�sn����һ��
 @param chipType  �����������оƬ���ͣ��μ�WifiChipType
 @param hoppingTime  ������ѡ��Ĭ����0����ӦĬ��ֵ200 ms����Ƶʱ��������Ҫ����200ms
 @param snifferTime  ������ѡ��Ĭ����0����ӦĬ��ֵ10000 ms�������ŵ�ʱ�䣬��Ҫ����10000ms
 @param pUserData  ������ѡ��Ĭ����NULL���û����������ݣ�Smartlink�����������ݣ��ڻص���ԭ������
 @return  QLERROR_INIT_SUCCESS��ʾ�ɹ���������������μ�wifisyncerror����
*/
SDK_API int start_smartlink(
    FUNC_HOP fHopping,
    char* szifName,
    FUNC_NOTIFY fNotify,
    char* szSN, 
	WifiChipType chipType,
    int hoppingTime,
    int snifferTime,
    void* pUserData
    );


/**  ���ô�API��ֹͣsmartlink��ֻ������Ҫǿ��ֹͣsmartlinkʱ�����ܹ�����stop_smartlink�����������������ɹ���
 @note
     һ��������FUNC_NOTIFY�ص�������ô˺���
 */
SDK_API void stop_smartlink();


/**  is_smartlink_running
*/
SDK_API int is_smartlink_running();

CXX_EXTERN_END

#endif // __TX_WIFI_SYNC_H__
