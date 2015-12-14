#ifndef __TX_OLD_INTERFACE_H__
#define __TX_OLD_INTERFACE_H__

#include "TXSDKCommonDef.h"
#include "TXDataPoint.h"

CXX_EXTERN_BEGIN

/////////////////////////////////////////////////////////////////////////
//
//  ���ֻ�QQ����CC datapoint��Ϣ�����ڽӿڣ�����û�кܺõĽ��������ߵ����⣬������������
//
////////////////////////////////////////////////////////////////////////

//��Ϣ ���շ�/���ͷ� ��Ϣ
typedef struct tag_tx_ccmsg_inst_info
{
    unsigned long long          target_id;          // ccmsg target id for send to / from
    unsigned int                appid;
    unsigned int                instid;
    unsigned int                platform;           // ָ��ƽ̨
    unsigned int                open_appid;         // ��ƽ�����������app��appid
    unsigned int                productid;          //
    unsigned int                sso_bid;            // SSO�ն˹�������appid
    char *                      guid;               // �豸��Ψһ��ʶ
    int                         guid_len;
}tx_ccmsg_inst_info;


// ����C2C datapoint����
typedef void (*on_send_cc_data_point_ret)(unsigned int cookie, unsigned long long to_client, int err_code);
/**
* �ӿ�˵��������Client to Client data point ��Ϣ
*/
SDK_API int tx_send_cc_data_point(unsigned long long to_client, unsigned int id, char * value, unsigned int * cookie, on_send_cc_data_point_ret ret_callback);
/**
* �ӿ�˵��������Client to Client data point ��Ϣ
*/
SDK_API int tx_send_cc_data_points(unsigned long long to_client, tx_data_point * data_points, int data_points_count, unsigned int * cookie, on_send_cc_data_point_ret ret_callback);

CXX_EXTERN_END

#endif // __TX_OLD_INTERFACE_H__
