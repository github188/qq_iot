#ifndef __TX_DEVICE_SDK_H__
#define __TX_DEVICE_SDK_H__

#include "TXAudioVideo.h"
#include "TXMsg.h"
#include "TXDataPoint.h"
#include "TXFileTransfer.h"

CXX_EXTERN_BEGIN

enum tx_test_mode_enum
{
    test_mode_default           = 0, // ������ʽ����
    test_mode_test_env          = 1, // ��¼�����Ի���
};

enum tx_network_type
{
	network_type_none			= 0,
	network_type_wifi			= 1, // wifi�����Լ������ƶ�����ͨ�����š����֮�������
	network_type_mobile			= 2, // �ƶ�����
	network_type_unicom			= 3, // ��ͨ����
	network_type_telecom		= 4, // ��������
	network_type_hongkong		= 5, // ���
};

//�豸��Ϣ����Ҫ�ڳ�ʼ���豸ʱ�ṩ��һЩӲ�������Ϣ���������ϵͳ��ip��ַ�ȵ�
typedef struct _tx_device_info
{
    //����ϵͳ��Ϣ
    char *                      os_platform;                //����ϵͳ����:iOS,Android,Synbian��    ��'\0'��β���ַ���

    //������Ϣ
    int                         network_type;				//�豸��ǰ���ӵ����磬�����ö�ٱ���tx_network_type��ȡֵ
                                                            //SDK����ݲ�ͬ�������������Ӷ�Ӧ��ip���������Ż�����

    //Ӳ����Ϣ
    char *                      device_name;                //�豸����,���Ȳ�����32�ֽ�    ��'\0'��β���ַ���
    char *                      device_serial_number;       //�豸���к�,���Ȳ�����16�ֽ�    ��'\0'��β���ַ���
    char *                      device_license;             //�豸LICENSE    ��'\0'��β���ַ���
    int                         product_version;            //�豸�汾

    //��ͨServer�������Ϣ
    int                         product_id;                 //ÿһ�����̵�ÿһ�����͵��豸��Ӧ��һ��id
    char *                      server_pub_key;             //�豸��½/ע��ʹ�õļ���server��Կ    ��'\0'��β���ַ���
    unsigned int                test_mode;                  //���Ի�����־λ
} tx_device_info;

//�豸������Ϣ
typedef struct tag_tx_binder_info
{
    int                 type;			//��������
    unsigned long long  tinyid;			//����tinyid
    unsigned long long  uin;			//����uin
    char                nick_name[128];	//�����ǳ�
    int                 gender;			//�����Ա�
    char                head_url[1024];	//����ͷ��url
}tx_binder_info;

//�豸֪ͨ����¼������״̬����Ϣ����ص��¼�֪ͨ
typedef struct _tx_device_notify
{
    // Login complete callback
    void (*on_login_complete)(int error_code);

    // Online status changed ---- statusȡֵΪ��11 ���ߡ�21 ����
    void (*on_online_status)(int old_status,  int new_status);

    // binder list change callback
    void (*on_binder_list_change)(int error_code, tx_binder_info * pBinderList, int nCount);
} tx_device_notify;


//SDK��ʼ��Ŀ¼��SDK������ЩĿ¼��д�ļ���������Ҫͬʱָ��ÿ��Ŀ¼�¿�д�洢�ռ�Ĵ�С(��λ���ֽ�)��SDK�ᱣ֤�����豸д��.
//������Ŀ¼�е�system_path�����ṩ��app_path�����ṩ���Է���׷�����⣻temp_path�����ṩ
//������Ŀ¼ȡֵ������ͬ�������ͬ����Ҫע��������ÿ��Ŀ¼��д�洢�ռ�Ĵ�С
typedef struct _tx_init_path
{
    //SDK���ڸ�Ŀ¼��д�뱣֤�������б����������Ϣ��
    //SDK�Ը�Ŀ¼�Ĵ洢�ռ�Ҫ����С��С��10K�������С��100K����SDKд��������٣���ȡ�����϶�
    char *                  system_path;
    unsigned int            system_path_capicity;

    //SDK���ڸ�Ŀ¼��д�����й����е��쳣������Ϣ
    //SDK�Ը�Ŀ¼�Ĵ洢�ռ�Ҫ��ϴ���С��С��300K�������С��1M����SDKд������϶࣬��ȡ��������
    char *                  app_path;
    unsigned int            app_path_capicity;

    //SDK���ڸ�Ŀ¼��д����ʱ�ļ�
    char *                  temp_path;
    unsigned int            temp_path_capicity;
} tx_init_path;


//�豸��������
enum tx_binder_type
{
	binder_type_unknown         = 0,
	binder_type_owner           = 1, //����
	binder_type_sharer          = 2, //������
};

//�豸�����Ա�
enum tx_binder_gender
{
	binder_gender_unknown       = -1,
	binder_gender_male          = 0, //��
	binder_gender_female        = 1, //Ů
};


/**
* �ӿ�˵������ʼ���豸SDK
*/
SDK_API int tx_init_device(tx_device_info *info, tx_device_notify *notify, tx_init_path* init_path);

/**
*
*/
SDK_API int tx_ack_app(unsigned int ip, unsigned int port);

/**
* �ӿ�˵�����˳������豸SDK����߼�
*/
SDK_API int tx_exit_device();


/**
 * �ӿ�˵��:��ȡSDK�汾��
 * ����˵��: main_ver     :  ���汾��
 *          sub_ver      :  �Ӱ汾��
 *          build_no     :  �������
 *
 */
SDK_API int tx_get_sdk_version(unsigned int *main_ver, unsigned int *sub_ver, unsigned int *build_no);


/**
 * �ӿ�˵��:��������дlog�Ļص�
 * �ص���������˵����
 *         level   log���� ȡֵ�� 0 ���ش���1 ����2 ���棻3 ��ʾ��4 ����
 *         module  ģ��
 *         line    �к�
 *         message log����
 */
typedef void (*tx_log_func)(int level, const char* module, int line, const char* message);
SDK_API void tx_set_log_func(tx_log_func log_func);


/**
* �ӿ�˵������ȡ�豸�����б�
*/
typedef void (*on_get_binder_list_result)(tx_binder_info * pBinderList, int nCount);
SDK_API int tx_get_binder_list(tx_binder_info * pBinderList, int* nCount, on_get_binder_list_result callback);


/**
* �ӿ�˵������ȡ�豸DIN
*/
SDK_API unsigned long long tx_get_self_din();


/**
* �ӿ�˵�����豸���µ�¼
*/
SDK_API int tx_device_relogin();


/**
 * �ӿ�˵��������豸�����еİ󶨹�ϵ��������豸������δ�󶨣�δ��½״̬
 */
typedef void (*on_erase_all_binders)(int error_code);
SDK_API int tx_erase_all_binders(on_erase_all_binders callback);


/**
 * �ӿ�˵��: ��ȡ��Ѷ�Ʒ�������׼Уʱʱ��
 * retvalue: ������32λ������Уʱʱ�䣬�ڲ�ʹ��monotonicʱ����Ϊ�ۼ����������ܺ�����ԭУʱ�߼���Ӱ��
 *           ���û�е�¼�ɹ�����˽ӿ�ֻ���� 0
 */
SDK_API int tx_get_server_time();



CXX_EXTERN_END

#endif // __TX_DEVICE_SDK_H__
