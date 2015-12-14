#ifndef __IPCAMERA_H__
#define __IPCAMERA_H__

#include "TXSDKCommonDef.h"
#include "TXDeviceSDK.h"

CXX_EXTERN_BEGIN


/////////////////////////////////////////////////////////////////
//
//         ipcamera������ؽӿ�: �����ȵ�������̨����
//
/////////////////////////////////////////////////////////////////


// ��Ƶ������
enum definition {
	def_low		= 1,	//��
	def_middle	= 2,	//��
	def_high	= 3,	//��
};

// ��̨(PTZ)��ת����
enum rotate_direction {
	rotate_direction_left	= 1,	//��
	rotate_direction_right	= 2,	//��
	rotate_direction_up		= 3,	//��
	rotate_direction_down	= 4,	//��
};

// ��̨(PTZ)��ת�Ƕȷ�Χ
enum rotate_degree {
	//ˮƽ�Ƕȷ�Χ
	rotate_degree_h_min = 0,
	rotate_degree_h_max = 360,

	//��ֱ�Ƕȷ�Χ
	rotate_degree_v_min = 0,
	rotate_degree_v_max = 180,
};

// ipcamera�����ӿڣ������ȵ���������
typedef struct _tx_ipcamera_notify {
	/**
	 * �ӿ�˵��:	ipcamera�����ȵ����ص�,���ú󷵻�����ͷ�ĵ�ǰ�ֱ���
	 * ����˵��:	cur_definition����ʱ������û���õ�
	 */
	int (*on_set_definition)(int definition, char *cur_definition, int cur_definition_length);

	/**
	 * �ӿ�˵����ipcamera��̨���ƻص�
	 */
	int (*on_control_rotate)(int rotate_direction, int rotate_degree);
} tx_ipcamera_notify;


/**
 * �ӿ�˵���� ����ipcamera��صĻص�
 */
SDK_API int tx_ipcamera_set_callback(tx_ipcamera_notify *notify);



/////////////////////////////////////////////////////////////////
//
//         ipcamera������ؽӿ�: ��ʷ��Ƶ�ؿ�
//
/////////////////////////////////////////////////////////////////

/**
  *  QQ������ʷ��Ƶ�ؿ�������
  *
  *      |-----------|                                        |��������������������������|
  *      |           |    <---- 1.on_fetch_history_video --   |             |
  *      |  DEVICE   |                                        |     APP     |
  *      |           |    <---- 2.on_play_history_video ----  |             |
  *      |           |                                        |             |
  *      |-----------|                                        |��������������������������|
  *
  *    ��Ҫ�������Ҫʹ����ʷ��Ƶ�ؿ����ܣ�������iot��̨�������Ƿ�֧����ʷ��Ƶ�ؿ������ܣ�����id��200021���������ֻ�QQ������ʾ��ʷ��Ƶ��ʱ����
  *
  *    step1. �ֻ�QQ����Ƶ���ӳɹ�֮�����豸��ѯ��ʷ��Ƶ��¼����������ʷ��Ƶ��ʱ���ᣬ������Ҫ�豸���ش洢��ʷ��Ƶ���ļ����ļ���Ӧ��ʱ���
  *    step2. �û����ֻ�QQ��ѡ����ʷ��Ƶ���ֻ�QQ���豸���Ͳ�����ʷ��Ƶ�����Ƶ������Ȼͨ��tx_set_video_data/tx_set_audio_data
  *           ���ͣ�ֻ���л�����Դ����ʷ��Ƶ���ɡ�ע�⣺��Ҫtx_set_video_data������nTimeStamps������������Ϊ��λ�ĵ�ǰ֡��ʱ��
  *
  *  ��Q����ʷ��Ƶʱ����ʾ��ͼ:
  *           ��ʷ��ƵƬ��2                ��ʷ��ƵƬ��1                  ʵʱ��Ƶ
  *      ��----------------------------------------------------------------|
  *      ��   ||||||||||||||              ||||||||||||||                   |
  *      ��   ||||||||||||||              ||||||||||||||                   |
  *      ��----------------------------------------------------------------|
  *           10:30     11:00             12:00     12:30               14:00
  */

/**
 * ��ʷ��ƵƬ�ε�ʱ������
 */
typedef struct _tx_history_video_range
{
	unsigned int start_time;     //��ƵƬ�ο�ʼʱ��
	unsigned int end_time;       //��ƵƬ�ν���ʱ��
} tx_history_video_range;

typedef struct _tx_history_video_notify
{
	/**
	 * �ӿ�˵��: ��ѯ��ʷ��Ƶ��Ϣ step1
	 * param: last_time   �ֻ�QQ�ֶ���ȡ��ʷ��Ƶ��Ϣ��last_timeΪʱ����ĩ�˵�ʱ��(unixʱ�����������ʱ����ʾ��ͼ�е�2:30
	 * param: max_count   �����ʱ��������Ŀ
	 * param: count       ʵ�ʷ��ص�������Ŀ
	 * param: range_list  sdk��̬������ڴ�ռ䣬�ӿ�ʵ������Ҫ����������д�����У���sdk�����ͷš�һ��Ҫ��ʵ�ʷ��ص���Ŀcountƥ�䣬��������δ֪����
	 */
	void (*on_fetch_history_video)(unsigned int last_time, int max_count, int *count, tx_history_video_range * range_list);

	/**
	 * �ӿ�˵��: ������ʷ��Ƶ step2
	 * param: play_time  ��ʷ��Ƶ��ʼ���ŵ���ʼʱ��(unixʱ���)������0��ʾ����ʷ��Ƶ�����л���ʵʱ��Ƶ����
	 */
	void (*on_play_history_video)(unsigned int play_time, unsigned long long base_time);
} tx_history_video_notify;

/**
 * ʹ����ʷ��Ƶ������Ҫ���豸����֮����г�ʼ��������ص�����
 * param: notify �豸�ص�֪ͨ
 */
SDK_API void tx_init_history_video_notify(tx_history_video_notify *notify);

CXX_EXTERN_END

#endif // __IPCAMERA_H__
