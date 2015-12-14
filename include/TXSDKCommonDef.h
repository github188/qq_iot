#ifndef __TX_SDK_COMMON_DEF_H__
#define __TX_SDK_COMMON_DEF_H__

#define SDK_API                     __attribute__((visibility("default")))

#ifndef __cplusplus
#   define bool                     _Bool
#   define true                     1
#   define false                    0
#   define CXX_EXTERN_BEGIN
#   define CXX_EXTERN_END
#   define C_EXTERN                 extern
#else
#   define _Bool                    bool
#   define CXX_EXTERN_BEGIN         extern "C" {
#   define CXX_EXTERN_END           }
#   define C_EXTERN
#endif


CXX_EXTERN_BEGIN

//ȫ�ִ������
enum error_code
{
    err_null                                = 0x00000000,       //success
    err_failed                              = 0x00000001,       //failed
    err_unknown                             = 0x00000002,       //δ֪����
    err_invalid_param                       = 0x00000003,       //�����Ƿ�
    err_buffer_notenough                    = 0x00000004,       //buffer���Ȳ���
    err_mem_alloc                           = 0x00000005,       //�����ڴ�ʧ��
    err_internal                            = 0x00000006,       //�ڲ�����
    err_device_inited                       = 0x00000007,       //�豸�Ѿ���ʼ������
    err_av_service_started                  = 0x00000008,       //av_service�Ѿ�������
    err_invalid_device_info                 = 0x00000009,       //�Ƿ����豸��Ϣ
    err_invalid_serial_number               = 0x0000000A,       //(10)      �Ƿ����豸���к�
    err_invalid_fs_handler                  = 0x0000000B,       //(11)      �Ƿ��Ķ�д�ص�
    err_invalid_device_notify               = 0x0000000C,       //(12)      �Ƿ����豸֪ͨ�ص�
    err_invalid_av_callback                 = 0x0000000D,       //(13)      �Ƿ�������Ƶ�ص�
    err_invalid_system_path                 = 0x0000000E,       //(14)      �Ƿ���system_path
    err_invalid_app_path                    = 0x0000000F,       //(15)      �Ƿ���app_path
    err_invalid_temp_path                   = 0x00000010,       //(16)      �Ƿ���temp_path
    err_not_impl                            = 0x00000011,       //(17)      δʵ��
    err_fetching                            = 0x00000012,       //(18)      �������̨��ȡ������
    err_fetching_buff_not_enough            = 0x00000013,       //(19)      �������̨��ȡ������ & buffer���Ȳ���
    err_off_line                            = 0x00000014,       //(20)      ��ǰ�豸��������״̬
    err_invalid_device_name                 = 0x00000015,       //(21)      �豸��û����߳��ȳ���32byte
    err_invalid_os_platform                 = 0x00000016,       //(22)      ϵͳ��Ϣû����߳��ȳ���32byte
    err_invalid_license                     = 0x00000017,       //(23)      licenseû����߳��ȳ���150byte
    err_invalid_server_pub_key              = 0x00000018,       //(24)      server_pub_keyû����߳��ȳ���120byte
    err_invalid_product_version             = 0x00000019,       //(25)      product_version�Ƿ�
    err_invalid_product_id                  = 0x0000001A,       //(26)      product_id�Ƿ�
    err_connect_failed                      = 0x0000001B,       //socket connectʧ��
    err_call_too_frequently                 = 0x0000001C,       //(28)      ���õ�̫Ƶ����
    err_sys_path_access_permission          = 0x0000001D,       //(29)      system_pathû�ж�дȨ��
    err_invalid_network_type				= 0x0000001E,		//(30)		��ʼ��ʱ������������ͷǷ�

    err_login_failed                        = 0x00010001,       //(65537)   ��¼ʧ��
    err_login_invalid_deviceinfo            = 0x00010002,       //(65538)   �豸��Ϣ�Ƿ�
    err_login_connect_failed                = 0x00010003,       //(65539)   ����Serverʧ��
    err_login_timeout                       = 0x00010004,       //(65540)   ��¼��ʱ
    err_login_eraseinfo                     = 0x00010005,       //(65541)   �����豸��Ϣ
    err_login_servererror                   = 0x00010006,       //(65542)   ��¼Server���ش���
 
    err_msg_sendfailed                      = 0x00020001,       //(131073)  ��Ϣ����ʧ��
    err_msg_sendtimeout                     = 0x00020002,       //(131074)  ��Ϣ���ͳ�ʱ
 
    err_av_unlogin                          = 0x00030001,       //(196609)  δ��¼���������������Ƶ����
 
    err_ft_transfer_failed                  = 0x00040001,       //(262145)  �ļ�����(����/����)ʧ��
    err_ft_file_too_large                   = 0x00040002,       //(262146)  ���͵��ļ�̫��
    //...
};

CXX_EXTERN_END


#endif // __TX_SDK_COMMON_DEF_H__
