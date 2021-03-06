#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h> 
#include <time.h>
#include <dirent.h>
#include "ring_queue.h"
#include "msg_handle.h"
#include "queue.h"
#include "common.h"
#include "TXIPCAM.h"
#include "monitor_dev.h"
#include "video_record.h"
#include "video_stream.h"
#include "fs_managed.h"
#include "voice_handle.h"


#undef  	DBG_ON
#undef  	FILE_NAME
#define 	DBG_ON  	(0x01)
#define 	FILE_NAME 	"video_record:"


struct  record_file_node 
{
	unsigned long  file_name;
	unsigned long file_size;
    tx_history_video_range rang;
    TAILQ_ENTRY(record_file_node) links;  
};



typedef struct record_video_handle
{

	pthread_mutex_t mutex_record;
	pthread_cond_t cond_record;
	ring_queue_t record_msg_queue;
	volatile unsigned int record_msg_num;
	unsigned int iframe_counts;
	char * record_file_name;

	FILE * index_fd;
	FILE * data_fd;

	pthread_mutex_t     record_file_mutex;
	unsigned int record_file_counts;
	TAILQ_HEAD(record_queue,record_file_node)record_file_queue;


	pthread_mutex_t mutex_replay_video;
	pthread_cond_t cond_replay_video;
	ring_queue_t replay_msg_queue_video;
	volatile unsigned int replay_msg_num_video;
	volatile unsigned int need_change_video;


	pthread_mutex_t mutex_replay_voice;
	pthread_cond_t cond_replay_voice;
	ring_queue_t replay_msg_queue_voice;
	volatile unsigned int replay_msg_num_voice;
	volatile unsigned int need_change_voice;


	

	struct  record_file_node cur_file_node;


}record_video_handle_t;



static  record_video_handle_t * record_handle = NULL;


static int is_digital(const char * file_name)
{
	int i = 0;
	int ret = 1;
	if(NULL == file_name)return (ret);
	for(i=0;i<strlen(file_name);++i)
	{
        if( file_name[i] >= '0' && file_name[i] <='9')
        {
			continue;
		}   
		else
		{
			ret = 0;
			break;
		}
	}
	
	return(ret);
}


static unsigned long record_get_file_size(const char * file_path)
{

	int ret = -1;
	if(NULL == file_path)
	{
		dbg_printf("the path is null,please check it ! \n");
		return(0);
	}
	struct stat file_stat;
	memset(&file_stat,0,sizeof(struct stat));
	ret = stat(file_path, &file_stat);
	if(ret < 0 )return(0);
	return(file_stat.st_size);
	
}

static char * record_new_file(void)
{

	int ret = -1;
	if(0 == mmc_get_status())
	{
		dbg_printf("the mmc is out ! \n");
		return(NULL);
	}

	
	unsigned int time_value = (unsigned int)time(NULL);
	char * new_file_name = calloc(1,sizeof(char)*32);
	if(NULL == new_file_name)
	{
		dbg_printf("calloc is fail ! \n");
		return(NULL);
	}
	snprintf(new_file_name,31,"%d",time_value);

	DIR *test_dir = NULL;
	if(NULL == (test_dir=opendir(RECORD_PATH)))
	{
		ret = mkdir(RECORD_PATH, 0777); 
		if(0 != ret)
		{
			dbg_printf("mkdir is fail ! \n");
			goto fail;
		}
	}
	else
	{
		closedir(test_dir);
		test_dir = NULL;
	}
	
	int fd = -1;
	char buff[64] = {0};
	memset(buff,'\0',64);
	snprintf(buff,64,"%s%s",RECORD_PATH,"/");
	strcat(buff,new_file_name);
	
	fd = open(buff,O_CREAT | O_TRUNC | O_RDWR,0777);
	if(fd < 0 )
	{
		dbg_printf("open file fail ! \n");
		goto fail;
	}
	else
	{
		close(fd);
		fd = -1;
	}
	fs_hangle_file(FILE_RECORD,FILE_NEW,NULL);

	return(new_file_name);
	
fail:

	if(NULL != new_file_name)
	{
		free(new_file_name);
		new_file_name = NULL;
	}

	return(NULL);

}



void  record_fetch_history(unsigned int last_time, int max_count, int *count, void * range )
{
	record_video_handle_t * handle = record_handle;
	unsigned int last_time_zone = last_time + 8*3600;
	int count_video = 0;
	tx_history_video_range * range_list = (tx_history_video_range * )range;
	if(NULL == handle)
	{
		dbg_printf("handle has not been init ! \n");
		return;
	}
	
	struct  record_file_node  * record_node = NULL;
	pthread_mutex_lock(&handle->record_file_mutex);
    TAILQ_FOREACH(record_node,&handle->record_file_queue,links)
    {
     	if(record_node->file_name <= last_time_zone)
     	{

			range_list[count_video].start_time = record_node->rang.start_time - 8*3600;
			range_list[count_video].end_time = record_node->rang.end_time - 8*3600;
     		count_video += 1;
			if(count_video >= max_count-1)break;

		}
    }
	pthread_mutex_unlock(&handle->record_file_mutex);

	tx_history_video_range * cur_range = &handle->cur_file_node.rang;
	//if((cur_range->start_time - 8*3600 > 0) && (cur_range->end_time - 8*3600 > 0))
	if(cur_range->end_time > cur_range->start_time)
	{
		dbg_printf("come here1!\n");
		range_list[count_video].start_time = cur_range->start_time - 8*3600;
		range_list[count_video].end_time = cur_range->end_time - 8*3600;
		*count  = count_video+1;
	}
	else
	{
		dbg_printf("come here2!\n");
		*count  = count_video;	
	}

	return;
	



}


static void * record_get_file_info(const char * file_name)
{
	if(NULL == file_name)return(NULL);
	FILE * precord = NULL;
	char buff[64] = {0};
	memset(buff,'\0',64);
	snprintf(buff,64,"%s%s",RECORD_PATH,"/");
	strcat(buff,file_name);

	
	precord = fopen(buff, "r");
	if(NULL == precord)
	{
		dbg_printf("fopen fail ! \n");
		return(NULL);
	}

	unsigned int read_counts = 0;
	unsigned long time_begin = 0;
	unsigned long time_end = 0;
	video_iframe_index_t * iframe = NULL;
	video_iframe_index_t * pre_iframe = NULL;
	char index_buff[DATA_FILE_OFFSET]={0};
	read_counts = fread(index_buff,1,DATA_FILE_OFFSET,precord);

	iframe = (video_iframe_index_t*)index_buff;
	if(0 != strcmp(iframe->magic,RECORD_MAGINC))
	{
		dbg_printf("this is not a right file ! \n"); 
		fs_hangle_file(FILE_RECORD,FILE_DEL,buff);
		goto fail;
	}

	time_begin = iframe->time_stamp;

	pre_iframe = iframe;
	iframe += 1;
	while(0 == strcmp(iframe->magic,RECORD_MAGINC))
	{
		pre_iframe = iframe;
		iframe += 1;
	}

	time_end = pre_iframe->time_stamp;

	struct  record_file_node  * record_node = calloc(1,sizeof(struct  record_file_node));
	if(NULL == record_node)
	{
		dbg_printf("calloc is fail ! \n");
		goto fail;
	}
	
	tx_history_video_range * new_range = &record_node->rang;

	

	record_node->file_size = record_get_file_size(buff);
	record_node->file_name = atol(file_name);
	new_range->start_time = time_begin;
	new_range->end_time = time_end;

	if(NULL != precord)
	{
		fclose(precord);
		precord = NULL;
	}

	return(record_node);

	
fail:

	if(NULL != precord)
	{
		fclose(precord);
		precord = NULL;
	}

	return(NULL);

}





static int  record_set_replay_offset(void * * file,const char * file_name,unsigned int play_time)
{
	if(NULL == file_name)
	{
		dbg_printf("the file is null ! \n");
		return(-1);
	}

	FILE * pfile = (FILE *)*file;
		
	if(NULL != pfile)
	{
		fclose(pfile);
		pfile = NULL;
	}
	 dbg_printf("the file name is %s \n",file_name);
	char buff[64] = {0};
	memset(buff,'\0',64);
	snprintf(buff,64,"%s%s",RECORD_PATH,"/");
	strcat(buff,file_name);
	
	*(FILE * *)file = fopen(buff, "r");
	if(NULL == *(FILE * *)file)
	{
		dbg_printf("fopen fail ! \n");
		return(-1);
	}

	unsigned int read_counts = 0;
	int is_find = 0;
	video_iframe_index_t * iframe = NULL;
	video_iframe_index_t * pre_iframe = NULL;
	char index_buff[DATA_FILE_OFFSET]={0};
	read_counts = fread(index_buff,1,DATA_FILE_OFFSET,*(FILE * *)file);

	iframe = (video_iframe_index_t*)index_buff;
	pre_iframe = iframe;
	while(0 == strcmp(iframe->magic,RECORD_MAGINC))
	{
		if(iframe->time_stamp > play_time)
		{
			is_find = 1;	
			break;
		}
		pre_iframe = iframe;
		iframe += 1;
	}
	if(0 == is_find)
	{
		dbg_printf("not find it !\n");
		fclose(*(FILE * *)file);
		* file = NULL;
		return(-1);
	}
	fseek(*(FILE * *)file,pre_iframe->offset,SEEK_SET);

	return(0);

}


static int record_scan_files(void)
{
	
	int ret = -1;
	DIR *record_dp = NULL;
	struct dirent * record_dirp = NULL;
	char * ptr = NULL;
	struct  record_file_node * record_node = NULL;
	if(NULL == record_handle)
	{
		dbg_printf("record_handle not been init ! \n");
		return(-1);
	}
	record_dp = opendir(RECORD_PATH);
	if(NULL == record_dp )
	{
		dbg_printf("open the dir fail \n");
		return (-1);
	}
	while((record_dirp =readdir(record_dp)) != NULL )
	{
		if(record_dirp->d_type != DT_REG )continue;
		ret = is_digital(record_dirp->d_name);
		if(0 == ret)continue;

		record_node = record_get_file_info(record_dirp->d_name);
		if(NULL == record_node)continue;
		TAILQ_INSERT_HEAD(&record_handle->record_file_queue,record_node,links);

	#if 1
	struct tm *now;
	now = localtime((time_t*)&record_node->rang.start_time);
	dbg_printf("start_time==%02d-%02d-%02d  %02d:%02d:%02d\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday,now->tm_hour, now->tm_min, now->tm_sec);

	struct tm *now1;
	now1 = localtime((time_t*)&record_node->rang.end_time);
	dbg_printf("end_time==%02d-%02d-%02d  %02d:%02d:%02d\n", now1->tm_year+1900, now1->tm_mon+1, now1->tm_mday,now1->tm_hour, now1->tm_min, now1->tm_sec);
	#endif

		record_node = NULL;
	}
	
	closedir(record_dp);
	record_dp = NULL;

	return(0);
}


int record_reinit_handle(void)
{

	
	record_video_handle_t * handle = (record_video_handle_t*)record_handle;
	if(NULL == handle)
	{
		dbg_printf("the handle has not be init! \n");
		return(-1);
	}

	char buff[64] = {0};
	if(NULL != handle->record_file_name)
	{

		struct  record_file_node * record_node = calloc(1,sizeof(*record_node));
		if(NULL == record_node)
		{
			dbg_printf("calloc is fail ! \n");
			return(-1);
		}

		memset(buff,'\0',64);
		snprintf(buff,64,"%s%s",RECORD_PATH,"/");
		strcat(buff,handle->record_file_name);
		record_node->file_size = record_get_file_size(buff);
		tx_history_video_range * new_range = &record_node->rang;
		record_node->file_name = atol(handle->record_file_name);
		new_range->start_time = record_node->file_name;
		new_range->end_time = (unsigned int)time(NULL);
		TAILQ_INSERT_HEAD(&handle->record_file_queue,record_node,links);
		free(handle->record_file_name);
		handle->record_file_name = NULL;
	}
	
	handle->iframe_counts = 0;
	if(NULL != handle->data_fd)
	{
		fclose(handle->data_fd);
		handle->data_fd = NULL;
	}
	if(NULL != handle->index_fd)
	{
		fclose(handle->index_fd);
		handle->index_fd = NULL;
	}
	return(0);

}


int  record_process_data(void * data)
{
	if(0 == mmc_get_status())
	{
		dbg_printf("the mmc is out ! \n");
		return(-1);
	}
	char buff[64] = {0};
	
	record_video_handle_t * handle = (record_video_handle_t*)record_handle;
	if(NULL == handle)
	{
		dbg_printf("handle is not init ! \n");
		return(-1);
	}
	tx_history_video_range * range = &handle->cur_file_node.rang;
	if(NULL == handle->record_file_name || handle->iframe_counts >= IFRAME_COUNTS_MAX)
	{
		if(NULL != handle->record_file_name)
		{

			struct  record_file_node * record_node = calloc(1,sizeof(*record_node));
			if(NULL == record_node)
			{
				dbg_printf("calloc is fail ! \n");
				return(-1);
			}

			memset(buff,'\0',64);
			snprintf(buff,64,"%s%s",RECORD_PATH,"/");
			strcat(buff,handle->record_file_name);
			record_node->file_size = record_get_file_size(buff);
			tx_history_video_range * new_range = &record_node->rang;
			record_node->file_name = atol(handle->record_file_name);
			new_range->start_time = record_node->file_name;
			new_range->end_time = (unsigned int)time(NULL);
			TAILQ_INSERT_HEAD(&handle->record_file_queue,record_node,links);
	

			free(handle->record_file_name);
			handle->record_file_name = NULL;
		}
		handle->record_file_name = 	record_new_file();
		if(NULL == handle->record_file_name)return(-1);
		handle->iframe_counts = 0;

		handle->cur_file_node.file_name = atol(handle->record_file_name);
		range->start_time = (unsigned int)time(NULL);


		if(NULL != handle->data_fd)
		{
			fclose(handle->data_fd);
			handle->data_fd = NULL;
		}
		if(NULL != handle->index_fd)
		{
			fclose(handle->index_fd);
			handle->index_fd = NULL;
		}
	}

	if(NULL == handle->data_fd)
	{
		memset(buff,'\0',64);
		snprintf(buff,64,"%s%s",RECORD_PATH,"/");
		strcat(buff,handle->record_file_name);
	
		handle->data_fd = fopen(buff,"w+");
		if(NULL == handle->data_fd)return(-1);
		fseek(handle->data_fd,DATA_FILE_OFFSET,SEEK_SET);

	}

	if(NULL == handle->index_fd)
	{
		memset(buff,'\0',64);
		snprintf(buff,64,"%s%s",RECORD_PATH,"/");
		strcat(buff,handle->record_file_name);

		handle->index_fd = fopen(buff,"w+");
		if(NULL == handle->index_fd)return(-1);
		fseek(handle->index_fd,INDEX_FILE_OFFSET,SEEK_SET);
	}


	record_data_type_t record_type = *(record_data_type_t*)data;
	if(RECORD_VIDEO_DATA == record_type)
	{
		video_data_t * video = (video_data_t*)data;
		if(0 == video->nFrameType)
		{
			video_iframe_index_t index;
			memset(&index,'\0',sizeof(index));
			fflush(handle->index_fd);
			//fflush(handle->data_fd);
			index.offset = ftell(handle->data_fd);

			index.time_stamp = (unsigned int)time(NULL);
			memmove(index.magic,RECORD_MAGINC,strlen(RECORD_MAGINC));
			fwrite((void*)&index,1,sizeof(index),handle->index_fd);
			handle->iframe_counts += 1;

			range->end_time = index.time_stamp;

		}
		video_node_header_t head = {0};
		head.total_size = sizeof(video_data_t) - (VIDEO_DATA_MAX_SIZE-video->nEncDataLen);
		head.check_flag = RECORD_VIDEO_DATA;
		fwrite((void*)&head,1,sizeof(head),handle->data_fd);
		fwrite((void*)video,1,head.total_size,handle->data_fd);
		video->status = 0;
		
	}
	else
	{
		voice_data_t * voice = (voice_data_t*)data;	
		video_node_header_t head = {0};
		head.total_size =sizeof(voice_data_t)-(ENCODE_AMR_SIZE-voice->data_length);
		head.check_flag = RECORD_VOICE_DATA;
		fwrite((void*)&head,1,sizeof(head),handle->data_fd);
		fwrite((void*)voice,1,head.total_size,handle->data_fd);
		if(NULL != voice)
		{
			free(voice);
			voice = NULL;
		}
	}



	return(0);
}




int record_push_record_data(void * data )
{

	int ret = 1;
	int i = 0;
	if(0 == mmc_get_status())return(-1);

	record_video_handle_t * handle = (record_video_handle_t*)record_handle;
	if(NULL == handle || NULL== data)
	{
		dbg_printf("check the param ! \n");
		return(-1);
	}

	pthread_mutex_lock(&(handle->mutex_record));

	for (i = 0; i < 1000; i++)
	{
	    ret = ring_queue_push(&handle->record_msg_queue, data);
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
		pthread_mutex_unlock(&(handle->mutex_record));
		return (-2);
    }
    else
    {
		volatile unsigned int *task_num = &handle->record_msg_num;
    	fetch_and_add(task_num, 1);
    }
    pthread_mutex_unlock(&(handle->mutex_record));
	pthread_cond_signal(&(handle->cond_record));
	return(0);
}


static void * record_record_pthread(void * arg)
{
	if(NULL == arg)
	{
		dbg_printf("please check the param ! \n");
		return(NULL);
	}
	
	record_video_handle_t * handle = (record_video_handle_t*)arg;
	int ret = -1;
	int is_run = 1;
	void *	record_data = NULL;

	while(is_run)
	{

        pthread_mutex_lock(&(handle->mutex_record));
        while (0 == handle->record_msg_num)
        {
            pthread_cond_wait(&(handle->cond_record), &(handle->mutex_record));
        }
		ret = ring_queue_pop(&(handle->record_msg_queue), (void **)&record_data);
		pthread_mutex_unlock(&(handle->mutex_record));
		
		volatile unsigned int * num = &(handle->record_msg_num);
		fetch_and_sub(num, 1); 

		if(ret != 0 || NULL == record_data)continue;

		record_process_data(record_data);

	}

	return(NULL);

}


int record_replay_send_stop(void)
{
	record_video_handle_t * handle = (record_video_handle_t*)record_handle;
	if(NULL == handle)
	{
		dbg_printf("the handle is null ! \n");
		return (-1);
	}
	pthread_mutex_lock(&(handle->mutex_replay_video));
	handle->need_change_video = 1;
	pthread_mutex_unlock(&(handle->mutex_replay_video));
	

	return(0);


}

int record_push_replay_video_data(unsigned int play_time, unsigned long long base_time)
{

	int ret = 1;
	int i = 0;
	
	video_replay_info_t * data = NULL;
	record_video_handle_t * handle = (record_video_handle_t*)record_handle;
	if(NULL == handle)
	{
		dbg_printf("check the param ! \n");
		return(-1);
	}
	if(0 == mmc_get_status())return(-1);
	struct  record_file_node  * record_node = NULL;
	tx_history_video_range * range = NULL;
	
	play_time += 8*3600;
	dbg_printf("play_time====%ld\n",play_time);

	tx_history_video_range * cur_range = &handle->cur_file_node.rang;
	if(NULL  != cur_range)
	{
		if(cur_range->start_time <= play_time && cur_range->end_time >= play_time)
		{
			 data = calloc(1,sizeof(*data));
			 if(NULL == data )
			 {
				dbg_printf("calloc is fail!\n");
				return(-1);
			 }
			 asprintf(&data->file_name,"%d",handle->cur_file_node.file_name);
			
			 data->base_time = base_time;
			 data->play_time = play_time;
		}
	}

	if(NULL == data)
	{
		pthread_mutex_lock(&handle->record_file_mutex);
	    TAILQ_FOREACH(record_node,&handle->record_file_queue,links)
	    {
	    	range = (tx_history_video_range *)&record_node->rang;

			dbg_printf("start_time====%ld\n",range->start_time);
			dbg_printf("end_time====%ld\n",range->end_time);
			if(range->start_time <= play_time && range->end_time >= play_time)
			{
				 data = calloc(1,sizeof(*data));
				 if(NULL == data )break;
				 asprintf(&data->file_name,"%d",record_node->file_name);
				
				 data->base_time = base_time;
				 data->play_time = play_time;
				 break;
			}
	    }
		pthread_mutex_unlock(&handle->record_file_mutex);

	}


	if(NULL == data)
	{
		dbg_printf("not find  dest file ! \n");
		return(-1);
	}


	pthread_mutex_lock(&(handle->mutex_replay_video));

	for (i = 0; i < 1000; i++)
	{
	    ret = ring_queue_push(&handle->replay_msg_queue_video, data);
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
		pthread_mutex_unlock(&(handle->mutex_replay_video));
		return (-2);
    }
    else
    {
		volatile unsigned int *task_num = &handle->replay_msg_num_video;
    	fetch_and_add(task_num, 1);
		handle->need_change_video = 1;
    }
    pthread_mutex_unlock(&(handle->mutex_replay_video));
	pthread_cond_signal(&(handle->cond_replay_video));
	return(0);
}




static int record_push_replay_voice_data(void * data )
{

	int ret = 1;
	int i = 0;
	if(0 == mmc_get_status())return(-1);

	record_video_handle_t * handle = (record_video_handle_t*)record_handle;
	if(NULL == handle || NULL== data)
	{
		dbg_printf("check the param ! \n");
		return(-1);
	}

	pthread_mutex_lock(&(handle->mutex_replay_voice));

	for (i = 0; i < 1000; i++)
	{
	    ret = ring_queue_push(&handle->replay_msg_queue_voice, data);
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
		pthread_mutex_unlock(&(handle->mutex_replay_voice));
		return (-2);
    }
    else
    {
		volatile unsigned int *task_num = &handle->replay_msg_num_voice;
    	fetch_and_add(task_num, 1);
    }
    pthread_mutex_unlock(&(handle->mutex_replay_voice));
	pthread_cond_signal(&(handle->cond_replay_voice));
	return(0);
}


static void * record_replay_video_pthread(void * arg)
{
	if(NULL == arg)
	{
		dbg_printf("please check the param ! \n");
		return(NULL);
	}
	
	record_video_handle_t * handle = (record_video_handle_t*)arg;
	int ret = -1;
	int is_run = 1;
	FILE * cur_file;
	int read_counts = 0;
	video_node_header_t data_head = {0};
	video_data_t	data_send = {0};
	voice_data_t	voice_send = {0};
	int time_offset = 0;
	unsigned int pre_time_stamps = 0;
	unsigned int cur_time_stamps = 0;
	
	video_replay_info_t *	replay_video = NULL;
	unsigned long nGopIndex = 0;
	unsigned long nFrameIndex = 0;
	unsigned long nTotalIndex = 0;
	while(is_run)
	{
        pthread_mutex_lock(&(handle->mutex_replay_video));
        while (0 == handle->replay_msg_num_video)
        {
            pthread_cond_wait(&(handle->cond_replay_video), &(handle->mutex_replay_video));
        }
		ret = ring_queue_pop(&(handle->replay_msg_queue_video), (void **)&replay_video);
		pthread_mutex_unlock(&(handle->mutex_replay_video));
		
		volatile unsigned int * num = &(handle->replay_msg_num_video);
		fetch_and_sub(num, 1); 

		if(ret != 0 || NULL == replay_video)continue;

		ret = record_set_replay_offset((void * * )&cur_file,replay_video->file_name,replay_video->play_time);
		if(0 != ret || NULL == cur_file)
		{

			dbg_printf("record_set_replay_offset fail !\n");
			goto clean;
		}
		else
		{
			dbg_printf("i find the offset ! \n");
			
			
		}


		video_send_video_stop();
		handle->need_change_video = 0;

		
		while(0 == feof(cur_file))
		{
			fread((void*)&data_head,1,sizeof(data_head),cur_file);
			if(RECORD_VIDEO_DATA == data_head.check_flag)break;
			fseek(cur_file,data_head.total_size,SEEK_CUR);

		}
		
		fread((void*)&data_send,1,data_head.total_size,cur_file);
		pre_time_stamps = data_send.nTimeStamps;
		cur_time_stamps = replay_video->play_time*1000 - replay_video->base_time-8*3600*1000;

		
		if(I_FRAME == data_send.nFrameType)
		{
			nGopIndex = (nGopIndex+1)%65535;	
			nFrameIndex = 0;
		}
		else
		{
			nFrameIndex ++;
		}
		nTotalIndex = (nTotalIndex+1)%65535;
		tx_set_video_data(data_send.data,data_send.nEncDataLen,data_send.nFrameType,cur_time_stamps/*data_send.nTimeStamps*/,nGopIndex,nFrameIndex,nTotalIndex,30);


		voice_replay_info_t * voice_replay = calloc(1,sizeof(*voice_replay));
		if(NULL != voice_replay)
		{
			asprintf(&voice_replay->file_name,"%s",replay_video->file_name);
			voice_replay->offset = ftell(cur_file);
			ret = record_push_replay_voice_data(voice_replay);
			if(0 != ret)
			{
				free(voice_replay);
				voice_replay = NULL;
			}

		}
		
		
		while((0 == handle->need_change_video) && (0 == feof(cur_file)))
		{

			fread((void*)&data_head,1,sizeof(data_head),cur_file);
			if(RECORD_VIDEO_DATA == data_head.check_flag)
			{
				fread((void*)&data_send,1,data_head.total_size,cur_file);
				time_offset = data_send.nTimeStamps-pre_time_stamps;
				pre_time_stamps = data_send.nTimeStamps;

				if(time_offset>0)
				{
					usleep(time_offset*1000);
				}
				if(I_FRAME == data_send.nFrameType)
				{
					nGopIndex = (nGopIndex+1)%65535;	
					nFrameIndex = 0;
				}
				else
				{
					nFrameIndex ++;
				}
				nTotalIndex = (nTotalIndex+1)%65535;
				cur_time_stamps = cur_time_stamps + time_offset;
				tx_set_video_data(data_send.data,data_send.nEncDataLen,data_send.nFrameType,cur_time_stamps/*data_send.nTimeStamps*/,nGopIndex,nFrameIndex,nTotalIndex,30);

			}
			else
			{
				fseek(cur_file,data_head.total_size,SEEK_CUR);
			}
			
		}


clean:
		if(NULL != replay_video)
		{
			if(NULL != replay_video->file_name)
			{
				free(replay_video->file_name);
				replay_video->file_name = NULL;
			}

			free(replay_video);
			replay_video = NULL;
		}

		if(NULL != cur_file)
		{
			fclose(cur_file);
			cur_file = NULL;
		}

		handle->need_change_voice = 1;

		
	}

	return(NULL);

}



static void * record_replay_voice_pthread(void * arg)
{
	if(NULL == arg)
	{
		dbg_printf("please check the param ! \n");
		return(NULL);
	}
	
	record_video_handle_t * handle = (record_video_handle_t*)arg;
	int ret = -1;
	int is_run = 1;

	int read_counts = 0;
	video_node_header_t data_head = {0};
	voice_data_t	voice_send = {0};
	char buff[64] = {0};
	int time_offset = 0;
	unsigned int pre_time_stamps = 0;
	unsigned int cur_time_stamps = 0;
	voice_replay_info_t *	replay_voice = NULL;
	FILE * voice_file = NULL;
	while(is_run)
	{
        pthread_mutex_lock(&(handle->mutex_replay_voice));
        while (0 == handle->replay_msg_num_voice)
        {
            pthread_cond_wait(&(handle->cond_replay_voice), &(handle->mutex_replay_voice));
        }
		ret = ring_queue_pop(&(handle->replay_msg_queue_voice), (void **)&replay_voice);
		pthread_mutex_unlock(&(handle->mutex_replay_voice));
		
		volatile unsigned int * num = &(handle->replay_msg_num_voice);
		fetch_and_sub(num, 1); 

		if(ret != 0 || NULL == replay_voice)continue;

		memset(buff,'\0',64);
		snprintf(buff,64,"%s%s",RECORD_PATH,"/");
		strcat(buff,replay_voice->file_name);
		voice_file = fopen(buff, "r");
		if(NULL == voice_file)
		{
			dbg_printf("fopen the file fail ! \n");
			goto clean;
		}
		else
		{
			dbg_printf("the voice file is %s\n",replay_voice->file_name);

		}

		voice_send_net_stop();
		
		fseek(voice_file,replay_voice->offset,SEEK_SET);
		while(0 == feof(voice_file))
		{
			fread((void*)&data_head,1,sizeof(data_head),voice_file);
			if(RECORD_VOICE_DATA == data_head.check_flag)break;
			fseek(voice_file,data_head.total_size,SEEK_CUR);

		}

		fread((void*)&voice_send,1,data_head.total_size,voice_file);
		pre_time_stamps = voice_send.time_sample;
		voice_fill_net_data(voice_send.data,voice_send.data_length);
		handle->need_change_voice = 0;
		while((0 == handle->need_change_voice)  && (0 == feof(voice_file)))
		{
			fread((void*)&data_head,1,sizeof(data_head),voice_file);
			if(RECORD_VOICE_DATA == data_head.check_flag)
			{
				fread((void*)&voice_send,1,data_head.total_size,voice_file);
				time_offset = voice_send.time_sample-pre_time_stamps;
				pre_time_stamps = voice_send.time_sample;

				time_offset -= 5;
				if(time_offset>0)
				{
					usleep(time_offset*1000);
				}
				voice_fill_net_data(voice_send.data,voice_send.data_length);
			}
			else
			{
				fseek(voice_file,data_head.total_size,SEEK_CUR);
			}
		}

		dbg_printf("i out of it !\n");

clean:
		if(NULL != replay_voice)
		{
			if(NULL != replay_voice->file_name)
			{
				free(replay_voice->file_name);
				replay_voice->file_name = NULL;
			}

			free(replay_voice);
			replay_voice = NULL;
		}

		if(NULL != voice_file)
		{
			fclose(voice_file);
			voice_file = NULL;
		}

	}

	return(NULL);

}




int record_manage_files(void)
{


	unsigned long free_size = 0;
	char buff[128] = {0};
	struct  record_file_node * node  = NULL;
	record_video_handle_t * handle = record_handle;
	if(NULL == handle)
	{
		dbg_printf("handle is null ! \n");
		return(-1);
	}
	if(0 == mmc_get_status())
	{
		dbg_printf("the mmc is out ! \n");
		return(-1);
	}

	free_size = mmc_get_free(RECORD_PATH);
	if(free_size < 0)
	{
		dbg_printf("mmc_get_free is fail ! \n");
		return(-1);
	}

	dbg_printf("mmc_free_size===%d\n",free_size);
	while(free_size < RECORD_FREE_SIZE)
	{
		pthread_mutex_lock(&handle->record_file_mutex);
		node = TAILQ_LAST(&handle->record_file_queue,record_queue);
		if(NULL != node)
		{
			TAILQ_REMOVE(&handle->record_file_queue,node,links);
		}
		pthread_mutex_unlock(&handle->record_file_mutex);

		if(NULL != node)
		{
			free_size += node->file_size/(1024*1024);
			memset(buff,'\0',128);
			snprintf(buff,128,"rm -rf %s%s%d",RECORD_PATH,"/",node->file_name);
			system(buff);
			free(node);
			node = NULL;
			dbg_printf("the del file name is %s \n",buff);
		}
		else
		{
			if(free_size < RECORD_FREE_SIZE)
			{
				video_record_video_stop();
				record_reinit_handle();
			}
			break;
		}

	}
	
	return(0);
}




int record_handle_center_up(void)
{

	int ret = -1;
	if(NULL != record_handle)
	{
		dbg_printf("record_handle has been init ! \n");
		return(-1);
	}

	record_handle = calloc(1,sizeof(*record_handle));
	if(NULL == record_handle)
	{
		dbg_printf("calloc is fail !\n");
		return(-1);
	}

	record_handle->iframe_counts = 0;
	record_handle->record_file_name = NULL;
	record_handle->data_fd = NULL;
	record_handle->index_fd = NULL;
	
	ret = ring_queue_init(&record_handle->record_msg_queue, 1024);
	if(ret < 0 )
	{
		dbg_printf("ring_queue_init  fail \n");

	}
    pthread_mutex_init(&(record_handle->mutex_record), NULL);
    pthread_cond_init(&(record_handle->cond_record), NULL);
	record_handle->record_msg_num = 0;

	pthread_mutex_init(&(record_handle->record_file_mutex), NULL);
	TAILQ_INIT(&record_handle->record_file_queue);


	ret = ring_queue_init(&record_handle->replay_msg_queue_video, 128);
	if(ret < 0 )
	{
		dbg_printf("ring_queue_init  fail \n");

	}
    pthread_mutex_init(&(record_handle->mutex_replay_video), NULL);
    pthread_cond_init(&(record_handle->cond_replay_video), NULL);
	record_handle->replay_msg_num_video = 0;



	ret = ring_queue_init(&record_handle->replay_msg_queue_voice, 128);
	if(ret < 0 )
	{
		dbg_printf("ring_queue_init  fail \n");

	}
    pthread_mutex_init(&(record_handle->mutex_replay_voice), NULL);
    pthread_cond_init(&(record_handle->cond_replay_voice), NULL);
	record_handle->replay_msg_num_voice = 0;
	record_handle->need_change_voice = 0;


	
	

	record_scan_files();
	memset(&record_handle->cur_file_node,0,sizeof(struct  record_file_node));
	
	pthread_t record_pthid;
	ret = pthread_create(&record_pthid,NULL,record_record_pthread,record_handle);
	pthread_detach(record_pthid);

	pthread_t replay_pthid;
	ret = pthread_create(&replay_pthid,NULL,record_replay_video_pthread,record_handle);
	pthread_detach(replay_pthid);

	pthread_t replay_voice_pthid;
	ret = pthread_create(&replay_voice_pthid,NULL,record_replay_voice_pthread,record_handle);
	pthread_detach(replay_voice_pthid);
	
	return(0);
}


