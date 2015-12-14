#ifndef __TX_TV_SDK_H__
#define __TX_TV_SDK_H__

#include "TXSDKCommonDef.h"

typedef struct tag_tx_tv_notify
{
	//�����״α��󶨳ɹ�ʱ�ص�
	void (*on_bind_complete)(unsigned long long ddwID, int error);

	//�յ���Ƶ����
	void (*on_receive_video_push)(char * pBufReply, int nLenReply);

}tx_tv_notify;


//���õ�����صĻص�
SDK_API void tx_set_tv_notify(tx_tv_notify * notify);

//������Ƶ����
typedef void (*on_receive_video_reply)(char * pBufReply, int nLenReply);
SDK_API void tx_send_video_request(int type, unsigned long long toDin, const char * pBuff, unsigned int uLen, on_receive_video_reply callback);

#ifdef __cplusplus
extern "C"
{
#endif
SDK_API unsigned long long tx_get_uin_by_tinyid(unsigned long long ddwID);
SDK_API unsigned long long tx_get_tinyid_by_uin(unsigned long long ddwID);
#ifdef __cplusplus
}
#endif
#endif // __TX_TV_SDK_H__
