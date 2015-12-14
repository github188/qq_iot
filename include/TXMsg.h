#ifndef __TX_MSG_H__
#define __TX_MSG_H__

#include "TXSDKCommonDef.h"

CXX_EXTERN_BEGIN


///////////////////////////////////////////////////////////////////////////
//
//                          �����ֻ�QQ������Ϣ��
//
//   ���ֻ�QQ������Ϣ��QQ��������Ӳ��ƽ̨��һ����ɫͨ�����������ͨ�����豸���Խ�
//   ��Ҫ����Ϣ�����ֻ�QQ��ֱ��չʾ���ֻ�QQ����Ϣ�б��С�
//
//   ATTENTION:
//   Ҫ������Щ��Ϣ����Ҫ�Ƚ�������ƽ̨��iot.qq.com���������ã�����QQ������̨�������ת��
//
///////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
//                          �����ֻ�QQ�����ı���Ϣ��
//
///////////////////////////////////////////////////////////////////////////

//callback: ����ȷ����Ϣ�Ƿ��ʹ���QQ������̨������
typedef void (*on_send_text_msg_ret)(unsigned int cookie, int err_code);

/**
 * �ӿ�˵�������ֻ�QQ�����ı���Ϣ
 * ����˵����
 *  msg_id                  ��̬��ϢID���뵽����ƽ̨����ע�ᣬû��ע�����ID�޷��ʹ��ֻ�QQ
 *  text                    Ҫ���͵��ı�
 *  ret_callback            ���ͽ���ص�
 *  cookie                  ��sdk�ڲ�����Ψһֵ�������ڷ��ͽ���ص�ʱ�ش�������Ψһ��ʶ��ǰ����
 *  to_targetids            ָ������ĳЩtarget
 *  to_targetids_count      ָ������ĳЩtarget��count����0��ʾ���͸����еİ���
 */
SDK_API void tx_send_text_msg(int msg_id, char *text, on_send_text_msg_ret ret_callback, unsigned int *cookie, unsigned long long* to_targetids, unsigned int to_targetids_count);


///////////////////////////////////////////////////////////////////////////
//
//                          �����ֻ�QQ���ͽṹ����Ϣ��
//
///////////////////////////////////////////////////////////////////////////
//
//   ��ν�ṹ����һ����Ϣ��������˶����Ϣ������title, timestamp, file_key, UI�ṹ���£�
//            __________________________________________
//           | title                                    |
//           | timestamp                                |
//           |      ______________________________      |
//           |     |                              |     |
//           |     |                              |     |
//           |     |            IMAGE             |     |
//           |     |                              |     |
//           |     |                              |     |
//           |     |______________________________|     |
//           | content                                  |
//           | jump url >>>                             |
//           |__________________________________________|
//
//
typedef struct tag_structuring_msg
{
	int                     msg_id;             // ��̬��ϢID���뵽����ƽ̨����ע�ᣬû��ע�����ID�޷��ʹ��ֻ�QQ
    char*					file_path;          // '\0'��β�ַ����������ļ���path����Ϊ�գ�
    char*					thumb_path;         // '\0'��β�ַ���������ͼpath���ɿգ�
    char*					title;              // '\0'��β�ַ������ṹ����Ϣ����
    char*					digest;             // '\0'��β�ַ�������������
    char*					guide_words;        // '\0'��β�ַ�������������
	unsigned int            duration;           // �����������Ϣ�����ֶ���������¼��ʱ�� ��λ:��
	unsigned long long*		to_targetids;       // ָ������ĳЩtarget
	unsigned int			to_targetids_count; // ָ������ĳЩtarget��count
} structuring_msg;

typedef struct _tx_send_msg_notify
{
    void (*on_file_transfer_progress)(const unsigned int cookie, unsigned long long transfer_progress, unsigned long long max_transfer_progress);
    void (*on_send_structuring_msg_ret)(const unsigned int cookie, int err_code);
} tx_send_msg_notify;

/**
 * �ӿ�˵�������ֻ�QQ���ͽṹ����Ϣ��ÿ����Ϣ������Ψһ�� cookie, tx_send_msg_notify �����˽ⷢ��״̬
 */
SDK_API void tx_send_structuring_msg(const structuring_msg *msg, tx_send_msg_notify *notify, unsigned int *cookie);




///////////////////////////////////////////////////////////////////////////
//
//                 ���ֻ�QQ����ǿ����֪ͨ
//
//////////////////////////////////////////////////////////////////////////
//
//  ��νǿ����֪ͨ��һ�����ơ��������ѡ���ǿ���ѻ��ƣ�һ������һЩ�ض������������������壬�ȵȡ�
//  ��Androidƽ̨�£����Ч������������û�������ϵ���������һ��Ҫ���ã���IOSƽ̨�£����ں�̨���е����ƣ�
//  ����ʵЧ������APNS��ƻ�����ͻ��ƣ��Ŀɿ��ԡ�
//

//callback: ����ȷ����Ϣ�Ƿ��ʹ���QQ������̨������
typedef void (*on_send_notify_msg_ret)(unsigned int cookie, int err_code);

/**
 * �ӿ�˵�������ֻ�QQ����ǿ����֪ͨ��ÿ����Ϣ������Ψһ�� cookie
 * ����˵����
 *  msg_id                  ��̬��ϢID���뵽����ƽ̨����ע�ᣬû��ע�����ID�޷��ʹ��ֻ�QQ
 *  digest                  ��ʾ��֪ͨ�����е���ʾ����
 *  ret_callback            ���ͽ���ص�
 *  cookie                  ��sdk�ڲ�����Ψһֵ�������ڷ��ͽ���ص�ʱ�ش�������Ψһ��ʶ��ǰ����
 *  to_targetids            ָ������ĳЩtarget
 *  to_targetids_count      ָ������ĳЩtarget��count����0��ʾ���͸����еİ���
 */
SDK_API void tx_send_notify_msg(int msg_id, char* digest, on_send_notify_msg_ret ret_callback, unsigned int *cookie, unsigned long long* to_targetids, unsigned int to_targetids_count);

CXX_EXTERN_END

#endif // __TX_MSG_H__
