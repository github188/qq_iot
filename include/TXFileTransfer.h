#ifndef __TX_FILE_TRANSFER_H__
#define __TX_FILE_TRANSFER_H__

#include "TXSDKCommonDef.h"
#include "TXOldInf.h"

////////////////////////////////////////////////////////////////////////////
//  �ļ�����ӿ�
////////////////////////////////////////////////////////////////////////////


CXX_EXTERN_BEGIN

/**
 * Ŀǰ�Ѿ���ʹ�õ�ҵ������
 */
#define BUSINESS_NAME_IMAGE_MSG             "ImgMsg"                  // �����ֻ�QQ��ͼƬ��Ϣ
#define BUSINESS_NAME_AUDIO_MSG             "AudioMsg"                // �����ֻ�QQ����������
#define BUSINESS_NAME_VIDEO_MSG             "VideoMsg"                // �����ֻ�QQ����Ƶ����

#define BUSINESS_NAME_NAS_DEVPUSHFILE       "7000-NASDevPushFile"     // �豸(NAS)���ֻ�QQ�����ļ�
#define BUSINESS_NAME_NAS_DEVPUSHTHUMB      "7001-NASDevPushThumb"    // �豸(NAS)���ֻ�QQ��������ͼ

#define BUSINESS_NAME_NAS_FILE_MUSIC        "8000-NASDevMusicFile"    // ��Q���豸������
#define BUSINESS_NAME_NAS_FILE_VIDEO        "8001-NASDevVideoFile"    // ��Q���豸����Ƶ(�ϴ���ļ�)
#define BUSINESS_NAME_NAS_FILE_DOC          "8002-NASDevDocumentFile" // ��Q���豸���ĵ�
#define BUSINESS_NAME_NAS_FILE_COMMON       "8003-NASDevCommonFile"   // ��Q���豸�������ļ�

// ... ���������

/**
 * ������������ ��tx_file_transfer_info -> transfer_type��
 */
enum tx_file_transfer_type
{
    transfet_type_none       = 0,   
    transfer_type_upload     = 1,   // ��ǰ�������ϴ�����
    transfer_type_download   = 2,   // ��ǰ��������������

    transfer_type_c2c_in     = 3,   // �� SDK �ڲ�ʹ��
    transfer_type_c2c_out    = 4,   // �� SDK �ڲ�ʹ��
};

/**
 * �����ļ����� ��tx_file_transfer_info -> file_type��
 */
enum tx_file_transfer_filetype
{
    transfer_filetype_image = 1,    //ͼƬ�ļ�
    transfer_filetype_video = 2,    //��Ƶ�ļ�
    transfer_filetype_audio = 3,    //�����ļ�
    transfer_filetype_other = 4,    //�����ļ�
};


/**
 * ����ͨ������ ��tx_file_transfer_info -> channel_type��
 */
enum tx_file_transfer_channeltype
{
    // FTN �ļ�����ͨ��, ��ͨ���ص����£�
    //��1����ȫ�Լ��ߣ�����ǿ�Ȱ�ȫУ�飬�޵�¼״̬�޷�����
    //��2���ļ����������֧���ļ���СΪ 4G
    //��3�����ϴ����ƣ�ͬһ�˺� 7 ��������ϴ� 5000 ���ļ������ڴ˻ᱻ�������ܾ�
    transfer_channeltype_FTN     =   1,   

    // С�ļ�����ͨ��, ��ͨ���ص����£�
    //��1��רΪPPT�������ţ�ͼƬ��С�ļ������������֧���ļ���С25M
    //��2����Ȩ��У�飬��url�Ϳ������أ���ȫǿ�Ƚϵ�
    transfer_channeltype_MINI    =   2,   
};



/**
 * ������Ϣ
 */
typedef struct tag_tx_file_transfer_info
{
    char                file_path[1024];    // �ļ�����·��                                    ��ͨ���ֶΣ�
    char                file_key[512];      // �ļ��ĺ�̨����                                  ��ͨ���ֶΣ�
    int                 key_length;

    char *              buffer_raw;			// ���ص���Buffer
    unsigned long long  buffer_raw_len;
    char                buffer_key[512];	// �ϴ�Buffer��õ��ĺ�̨����
    int                 buffer_key_len;

    char *              buff_with_file;     // C2C�����ļ��������Զ���BUFFER     �������� tx_send_file_to��
    int                 buff_length;

    char                bussiness_name[64]; // ����ȷ������ or ���յ��ļ���ҵ�񳡾�            ��ͨ���ֶΣ�
                                            // �����յ�һ���ļ�ʱ������ֶ�����ָ���ļ����������ԣ����Ǹ���ӡ���Ĵ���ӡ�ļ�
                                            // �Ѿ������ business name ����ͷ�ļ��ʼ�ĺ궨��
    
    unsigned long long  file_size;          // �ļ���С                                        ��ͨ���ֶΣ�
    int                 channel_type;       // ͨ�����ͣ�tx_file_transfer_channeltype          ��ͨ���ֶΣ�
    int                 file_type;          // �ļ����ͣ�tx_file_transfer_filetype             ��ͨ���ֶΣ�
    int                 transfer_type;      // upload  download  c2c_in  c2c_out
}tx_file_transfer_info;


/**
* ֪ͨ���ص�
*/
typedef struct tag_tx_file_transfer_notify
{
    // �������
    // transfer_progress �� �ϴ� ���ؽ���
    // max_transfer_progress �� ���ȵ����ֵ�� transfer_progress/max_transfer_progress ���㴫��ٷֱ�
    void (*on_transfer_progress)(unsigned long long transfer_cookie, unsigned long long transfer_progress, unsigned long long max_transfer_progress);

    // ������
    // ԭ�����յ����֪ͨ����ȥ tx_query_transfer_info ���������ֱ֪ͨ�Ӵ���tx_file_transfer_info
    // tx_file_transfer_info�ṹ��������ļ�����·�������ؾ����ļ��ı���·��
    void (*on_transfer_complete)(unsigned long long transfer_cookie, int err_code, tx_file_transfer_info* tran_info);

    // �յ�C2C transfer֪ͨ
    void (*on_file_in_come)(unsigned long long transfer_cookie, const tx_ccmsg_inst_info * inst_info, const tx_file_transfer_info * tran_info);

}tx_file_transfer_notify;


/**
* ��ʼ�����ļ�
*   notify : �ص�
*   path_recv_file : �����ļ���Ŀ¼
*/
SDK_API int tx_init_file_transfer(tx_file_transfer_notify notify, char * path_recv_file);


/**
* �ϴ��ļ�
* channeltype : ����ͨ�����ͣ�ȡֵ��Χ��tx_file_transfer_channeltype���豸��Q֮���С�ļ�ͨ����δ��ȫ�ſ����豸�ϴ���ʹ��transfer_channeltype_FTN(1)��
* filetype : �����ļ����ͣ�ȡֵ��Χ��tx_file_transfer_filetype
*/
SDK_API int tx_upload_file(int channeltype, int filetype, char * file_path, unsigned long long * transfer_cookie);


/**
* �����ļ�
* channeltype : ����ͨ�����ͣ�ȡֵ��Χ��tx_file_transfer_channeltype���豸��Q֮���С�ļ�ͨ����δ��ȫ�ſ����豸�ϴ���ʹ��transfer_channeltype_FTN(1)��
* filetype : �����ļ����ͣ�ȡֵ��Χ��tx_file_transfer_filetype
*/
SDK_API int tx_download_file(int channeltype, int filetype, char * file_key, int key_length, unsigned long long * transfer_cookie);


/**
* �����ļ���������
* buff_with_file & bussiness_name : ���͵��Զ�ʱ���Զ˿ɸ���bussiness_name���Խ��յ����ļ�����ͬ�Ĵ���buff_with_file����Я��������������Ϣ
*/
SDK_API int tx_send_file_to(unsigned long long target_id, char * file_path, unsigned long long * transfer_cookie, char * buff_with_file, int buff_length, char * bussiness_name);
// SDK_API int tx_send_file_to_ex(const tx_ccmsg_inst_info * inst_info, char * file_path, unsigned long long * transfer_cookie, char * buff_with_file, int buff_length, char * bussiness_name);


/**
* ȡ������
*/
SDK_API int tx_cancel_transfer(unsigned long long transfer_cookie);

/**
 * ע��bussiness_name���˻ص�
 */
SDK_API int tx_reg_file_transfer_filter(char * bussiness_name, tx_file_transfer_notify notify);

/**
* ��ȡС�ļ�ͨ������url
* fileId��ͨ��С�ļ�ͨ���ϴ�Դ�ļ��õ����ļ��������Զ˷��͹�����
* fileType���ļ����ͣ�Ҳ�ǶԶ˷��͹����ġ�
* downloadUrl�����Ρ�ʹ���߷���ռ䣬�ֽڴ�С����Ϊ400�����ڱ����׼http�����ļ������ӵ�ַ������ƽ5��28�Ų�֧��С�ļ�ͨ����׼http���أ�
*/
SDK_API int tx_get_minidownload_url(char* fileId, int fileType, char* downloadUrl);

CXX_EXTERN_END

#endif // __TX_FILE_TRANSFER_H__
