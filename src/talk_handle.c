#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "ring_queue.h"
#include "common.h"
#include "pcm_dev.h"
#include "amr_encode.h"
#include "amr_decode.h"
#include "TXAudioVideo.h"
#include "msg_handle.h"
#include "talk_handle.h"



#undef  	DBG_ON
#undef  	FILE_NAME
#define 	DBG_ON  	(0x01)
#define 	FILE_NAME 	"talk_handle:"




typedef struct talk_handle
{

	pthread_mutex_t mutex_talk;
	pthread_cond_t cond_talk;
	ring_queue_t talk_msg_queue;
    volatile unsigned int talk_msg_num;

	pthread_mutex_t mutex_play;
	pthread_cond_t cond_play;
	ring_queue_t play_msg_queue;
    volatile unsigned int play_msg_num;
	

	void * decode_handle;
	void * da_handle;


}talk_handle_t;



static talk_handle_t * talk = NULL;



int talk_push_net_data(void * data )
{

	int ret = 1;
	int i = 0;

	talk_handle_t * handle = (talk_handle_t*)talk;
	if(NULL == handle || NULL== data)
	{
		dbg_printf("check the param ! \n");
		return(-1);
	}

	pthread_mutex_lock(&(handle->mutex_talk));

	for (i = 0; i < 1000; i++)
	{
	    ret = ring_queue_push(&handle->talk_msg_queue, data);
	    if (ret < 0)
	    {
	        usleep(i);
	        continue;
	    }
	    else
	    {
	        break;
	    }	
	}
    if (ret < 0)
    {
		pthread_mutex_unlock(&(handle->mutex_talk));
		return (-2);
    }
    else
    {
		volatile unsigned int *task_num = &handle->talk_msg_num;
    	fetch_and_add(task_num, 1);
    }
    pthread_mutex_unlock(&(handle->mutex_talk));
	pthread_cond_signal(&(handle->cond_talk));
	return(0);
}




static void * talk_process_net_data_pthread(void * arg)
{


	talk_handle_t * handle = (talk_handle_t * )arg;
	if(NULL == handle)
	{
		dbg_printf("please check the param ! \n");
		return(NULL);
	}
	int ret = -1;
	int is_run = 1;
	talk_net_data_t * net_data = NULL;
	void * amr_decode = handle->decode_handle;
	if(NULL == amr_decode)
	{
		dbg_printf("amr_decode is null \n");
		return(NULL);
	}
	
	while(is_run)
	{
        pthread_mutex_lock(&(handle->mutex_talk));
        while (0 == handle->talk_msg_num)
        {
            pthread_cond_wait(&(handle->cond_talk), &(handle->mutex_talk));
        }
		ret = ring_queue_pop(&(handle->talk_msg_queue), (void **)&net_data);
		pthread_mutex_unlock(&(handle->mutex_talk));
		
		volatile unsigned int *handle_num = &(handle->talk_msg_num);
		fetch_and_sub(handle_num, 1);  
		if(ret != 0 || NULL == net_data)continue;

		int out_length = 0;
		unsigned char buff[4096];

		talk_raw_data_t * raw_data = calloc(1,sizeof(*raw_data));
		if(NULL == raw_data)
		{
			usleep(100*1000);
			continue;
		}
		raw_data->length = amr_decode_data(amr_decode,net_data->data,net_data->length,raw_data->data,TALK_RAW_DATA_SIZE);
		dbg_printf("the input length is == %d the out length is %d\n",net_data->length,out_length);
		if(NULL != net_data)
		{
			free(net_data);
			net_data = NULL;
		}

		if(raw_data->length <= 0)
		{
			free(raw_data);
			raw_data = NULL;
		}

				

	}



	return(NULL);




}

static void * talk_play_raw_data_pthread(void * arg )
{

	talk_handle_t * handle = (talk_handle_t * )arg;
	if(NULL == handle)
	{
		dbg_printf("please check the param ! \n");
		return(NULL);
	}
	int ret = -1;
	int is_run = 1;
	talk_net_data_t * net_data = NULL;
	void * amr_decode = handle->decode_handle;
	if(NULL == amr_decode)
	{
		dbg_printf("amr_decode is null \n");
		return(NULL);
	}
	
	while(is_run)
	{
        pthread_mutex_lock(&(handle->mutex_talk));
        while (0 == handle->talk_msg_num)
        {
            pthread_cond_wait(&(handle->cond_talk), &(handle->mutex_talk));
        }
		ret = ring_queue_pop(&(handle->talk_msg_queue), (void **)&net_data);
		pthread_mutex_unlock(&(handle->mutex_talk));
		
		volatile unsigned int *handle_num = &(handle->talk_msg_num);
		fetch_and_sub(handle_num, 1);  
		if(ret != 0 || NULL == net_data)continue;

		int out_length = 0;
		unsigned char buff[4096];

		talk_raw_data_t * raw_data = calloc(1,sizeof(*raw_data));
		if(NULL == raw_data)
		{
			usleep(100*1000);
			continue;
		}
		raw_data->length = amr_decode_data(amr_decode,net_data->data,net_data->length,raw_data->data,TALK_RAW_DATA_SIZE);
		dbg_printf("the input length is == %d the out length is %d\n",net_data->length,out_length);
		if(NULL != net_data)
		{
			free(net_data);
			net_data = NULL;
		}

		if(raw_data->length <= 0)
		{
			free(raw_data);
			raw_data = NULL;
		}

				

	}
	

	return(NULL);
}





int talk_handle_start_up(void)
{

	int ret = -1;
	if(NULL != talk)
	{
		dbg_printf("talk has been init !\n");
		return(-1);
	}

	talk = calloc(1,sizeof(*talk));
	if(NULL == talk)
	{
		dbg_printf("calloc is fail !\n");
		return(-1);
	}

	ret = ring_queue_init(&talk->talk_msg_queue, 1024);
	if(ret < 0 )
	{
		dbg_printf("ring_queue_init  fail \n");
		return(-1);
	}
    pthread_mutex_init(&(talk->mutex_talk), NULL);
    pthread_cond_init(&(talk->cond_talk), NULL);
	talk->talk_msg_num= 0;


	ret = ring_queue_init(&talk->play_msg_queue, 1024);
	if(ret < 0 )
	{
		dbg_printf("ring_queue_init  fail \n");
		return(-1);
	}
    pthread_mutex_init(&(talk->mutex_play), NULL);
    pthread_cond_init(&(talk->cond_play), NULL);
	talk->play_msg_num= 0;

	

	talk->decode_handle = amr_decode_open();
	
	



	pthread_t talk_process_pthred;
	pthread_create(&talk_process_pthred, NULL, talk_process_net_data_pthread, talk);
	pthread_detach(talk_process_pthred);


	pthread_t talk_play_pthred;
	pthread_create(&talk_play_pthred, NULL, talk_play_raw_data_pthread, talk);
	pthread_detach(talk_play_pthred);



	
	return(0);
}