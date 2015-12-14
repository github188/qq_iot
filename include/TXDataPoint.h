#ifndef __TX_DATA_POINT_H__
#define __TX_DATA_POINT_H__

#include "TXSDKCommonDef.h"
#include "TXMsg.h"

CXX_EXTERN_BEGIN

///////////////////////////////////////////////////////////////////////////
//
//                 �ͼ��ӿڣ� ֱ�Ӵ��� DATAPOINT ��Ϣ
//
///////////////////////////////////////////////////////////////////////////
//
//   DATAPOINT�ṹ��
//
//  ��1��һ�� datapoint ������Ҫ����Ϣ���� id �� value
//  ��2��QQ������̨��������Ϣ������ datapoint id ����Լ���ģ�
//  ��3����� datapoint id ��ͬ����value����ͬ���ĸ�ʽ������֯
//  ��4��QQ����ƽ̨�е����������豸��ͬ����  datapoint id ����ͬ���Ľ����߼�����������໥���
//   

typedef struct tag_tx_data_point
{
    // char * : utf8 with '\0'
    unsigned int            id;
    char *                  value;
    unsigned int            seq;
    unsigned int            ret_code;
}tx_data_point;

// 
typedef struct tag_tx_data_point_notify
{
    void (*on_receive_data_point)(unsigned long long from_client, tx_data_point * data_points, int data_points_count);
} tx_data_point_notify;

// ��ʼ��datapoint
// notify: receive datapoint callback
SDK_API int tx_init_data_point(const tx_data_point_notify *notify);






// �ϱ�datapoint���� ���callback
typedef void (*on_report_data_point_ret)(unsigned int cookie, int err_code);

// �ϱ�datapoint����
// [in]     id:             �ϱ���datapoint ID
// [in]     value:          �ϱ���datapoint value
// [out]    cookie:         ���ص���cookie
// [in]     ret_callback:   ���ͽ��callback
SDK_API int tx_report_data_point(unsigned int id, char * value, unsigned int * cookie, on_report_data_point_ret ret_callback);

// �ϱ�һ��datapoint����
// [in]     data_points:        �ϱ���datapoint
// [in]     data_points_count:  �ϱ���datapoint count
// [out]    cookie:             ���ص���cookie
// [in]     ret_callback:       ���ͽ��callback
SDK_API int tx_report_data_points(tx_data_point * data_points, int data_points_count, unsigned int * cookie, on_report_data_point_ret ret_callback);






typedef void (*on_ack_data_point_ret)(unsigned int cookie, unsigned long long from_client, int err_code);

// Ӧ���յ���datapoint
// [in]    from_client:     datapoint��������ĳ�����ߣ�       ��������յ���datapointʱ��from_clientһ�£�����ᱻ����
// [in]    id:              Ӧ���datapoint��Ӧ��ID��          ��������յ���datapoint��IDһ�£�����ᱻ����
// [in]    value:           Ӧ���datapoint���Զ�������
// [in]    seq:             Ӧ���datapoint��Ӧ��seq��         ��������յ���datapoint��seqһ�£�����ᱻ����
// [out]   cookie:          ���ص���cookie
// [in]    ret_callback:    ���ͽ��callback
SDK_API int tx_ack_data_point(unsigned long long from_client, unsigned int id, char * value, unsigned int seq, unsigned int ret_code, unsigned int * cookie, on_ack_data_point_ret ret_callback);

// Ӧ���յ���һ��datapoint
// [in]    from_client:         datapoint��������ĳ�����ߣ�       ��������յ���datapointʱ��from_clientһ�£�����ᱻ����
// [in]    data_points:         Ӧ���datapoint��                 Ӧ���һ��datapoint������յ���һ��datapoint�� ID & seq��Ҫһ�£������� frome_client & ID & seq �������յ���datapoint��¼���ᱻ����
// [in]    data_points_count:   datapoint count
// [out]   cookie:              ���ص���cookie
// [in]    ret_callback:        ���ͽ��callback
SDK_API int tx_ack_data_points(unsigned long long from_client, tx_data_point * data_points, int data_points_count, unsigned int * cookie, on_ack_data_point_ret ret_callback);


CXX_EXTERN_END

#endif // __TX_DATA_POINT_H__
