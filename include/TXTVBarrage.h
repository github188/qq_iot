#ifndef __TX_TV_BARRAGE_H__
#define __TX_TV_BARRAGE_H__

#include "TXDeviceSDK.h"

CXX_EXTERN_BEGIN

enum tx_barrage_msg_element_type
{
    element_none            = 0,
    element_text            = 1,        // �ı���Ϣ
    element_face            = 2,        // ����
    element_image           = 3,        // ͼƬ
    element_audio           = 4,        // ������Ϣ
};

// �ı���Ϣ
typedef struct tag_tx_text_msg_element
{
    char *              msg_text;       // '\0'��β���ַ���
}tx_text_msg_element;

// ������Ϣ
typedef struct tag_tx_face_msg_element
{
    unsigned int        face_index;
}tx_face_msg_element;

// ͼƬ
typedef struct tag_tx_image_msg_element
{
    char *              image_guid;
    int                 guid_length;
    char *              image_url;      // ԭͼ���ص�ַ '\0'��β���ַ���
    char *              image_thumb_url;// ����ͼ���ص�ַ '\0'��β���ַ���
}tx_image_msg_element;

// ����
typedef struct tag_tx_audio_msg_element
{
    unsigned int        voice_switch;
    char *              audio_msg_url;  // ������Ϣ������url '\0'��β���ַ���
}tx_audio_msg_element;

typedef struct tag_tx_barrage_msg_element
{
    int                         msg_ele_type;
    void *                      msg_ele_point;
}tx_barrage_msg_element;

typedef struct tag_tx_barrage_msg
{
    unsigned long long          group_id;           // Ⱥid
    unsigned long long          from_target_id;     // ��Ϣ������id
    unsigned int                from_target_appid;  // ��Ϣ������appid
    unsigned int                from_target_instid; // ��Ϣ������instid
    char *                      group_name;         // Ⱥ���֣���'\0'��β���ַ���
    char *                      from_nick;          // ������nick����'\0'��β���ַ���
    char *                      from_group_card;    // ������Ⱥ��Ƭ����'\0'��β���ַ���
    char *                      from_head_url;      // �����ߵ�ͷ��url����'\0'��β���ַ���
    tx_barrage_msg_element *    msg_ele_array;      // ��ϢԪ������
    int                         msg_ele_count;      // ��ϢԪ���������
}tx_barrage_msg;


typedef struct tag_tx_barrage_notify
{
	//�յ�Ⱥ��Ϣ
	void (*on_receive_barrage_msg)(tx_barrage_msg * pMsg);

}tx_barrage_notify;


//���õ�Ļ��صĻص�
SDK_API void tx_set_barrage_notify(tx_barrage_notify * notify);

CXX_EXTERN_END

#endif // __TX_TV_BARRAGE_H__
