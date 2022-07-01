/**
 *
 * 
 * media_player.c. The file is for media player, the play action include
 * stop/play/pause/seek/fast forward/fast backward/slow forward/slow backward/step/
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ffplayer.h>
#include <hcuapi/dis.h>
#include <hcuapi/avsync.h>
#include <hcuapi/snd.h>
#include <sys/ioctl.h>
#include "com_api.h"
#include "media_player.h"
#include "os_api.h"

static int m_ff_speed[] = {1, 2, 4, 8, 16, 24, 32};
static int m_fb_speed[] = {1, -2, -4, -8, -16, -24, -32};
static float m_sf_speed[] = {1, 1/2, 1/4, 1/8, 1/16, 1/24};
static float m_sb_speed[] = {1, -1/2, -1/4, -1/8, -1/16, -1/24};
static bool m_closevp = true,  m_fillblack = false;

media_handle_t *media_open(media_type_t type)
{
	media_handle_t *media_hld = (media_handle_t*)malloc(sizeof(media_handle_t));

	memset(media_hld, 0, sizeof(media_handle_t));
	media_hld->type = type;
	media_hld->state = MEDIA_STOP;
	media_hld->msg_id = INVALID_ID;

	if (MEDIA_TYPE_PHOTO == type){
		media_hld->time_gap = 2000; //2 seconds interval for next slide show
	}
	media_hld->loop_type = PLAY_LIST_NONE;
	pthread_mutex_init(&media_hld->api_lock, NULL);

	return media_hld;
}

void media_close(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	pthread_mutex_destroy(&media_hld->api_lock);
	free((void*)media_hld);
 }

int media_play(media_handle_t *media_hld, const char *media_src)
{
	ASSERT_API(media_hld && media_src);

    HCPlayerInitArgs player_args;

	pthread_mutex_lock(&media_hld->api_lock);
	strncpy(media_hld->play_name, media_src, MAX_FILE_NAME-1);

	printf("%s(), line:%d. play: %s.\n", __FUNCTION__, __LINE__, media_src);
	memset(&player_args, 0, sizeof(player_args));
    player_args.uri = media_src;
	player_args.msg_id = media_hld->msg_id;
	player_args.user_data = media_hld;
	player_args.sync_type = 2;
	if (MEDIA_TYPE_MUSIC == media_hld->type){
		player_args.play_attached_file = 1;
	}
    media_hld->player = hcplayer_create(&player_args);
    if (!media_hld->player){
        printf("hcplayer_create() fail!\n");
        pthread_mutex_unlock(&media_hld->api_lock);
        return API_FAILURE;
    }
    hcplayer_play(media_hld->player);

    media_hld->state = MEDIA_PLAY;
    media_hld->speed = 0;

	pthread_mutex_unlock(&media_hld->api_lock);
	return API_SUCCESS;
}

#define SOUND_DEV	"/dev/sndC0i2so"
#define DEFAULT_VOL	50
static uint8_t m_vol_back = DEFAULT_VOL;
int media_mute(bool mute)
{
	int snd_fd = -1;

	snd_fd = open(SOUND_DEV, O_WRONLY);
	if (snd_fd < 0) {
		printf ("open snd_fd %d failed\n", snd_fd);
		return API_FAILURE;
	}

	uint8_t volume = 0;
	if (mute){
		ioctl(snd_fd, SND_IOCTL_GET_VOLUME, &m_vol_back);
		if (0 == m_vol_back)
			m_vol_back = DEFAULT_VOL;

		ioctl(snd_fd, SND_IOCTL_SET_VOLUME, &volume);
		ioctl(snd_fd, SND_IOCTL_SET_MUTE, mute);
	} else {
		volume = m_vol_back;

		ioctl(snd_fd, SND_IOCTL_SET_MUTE, mute);
		ioctl(snd_fd, SND_IOCTL_SET_VOLUME, &volume);
	}

	close(snd_fd);

	printf("mute is %d, vol: %d\n", mute, volume);
	return API_SUCCESS;
}

int media_set_vol(uint8_t volume)
{
	int snd_fd = -1;

	snd_fd = open(SOUND_DEV, O_WRONLY);
	if (snd_fd < 0) {
		printf ("open snd_fd %d failed\n", snd_fd);
		return API_FAILURE;
	}

	ioctl(snd_fd, SND_IOCTL_SET_VOLUME, &volume);
	volume = 0;
	ioctl(snd_fd, SND_IOCTL_GET_VOLUME, &volume);
	printf("current volume is %d\n", volume);

	close(snd_fd);
	return API_SUCCESS;
}

int media_vol_up()
{
	/*
	uint8_t volume;
	int snd_fd = -1;

	snd_fd = open(SOUND_DEV, O_WRONLY);
	if (snd_fd < 0) {
		printf ("open snd_fd %d failed\n", snd_fd);
		return API_FAILURE;
	}

	ioctl(snd_fd, SND_IOCTL_GET_VOLUME, &volume);
	volume += 

	ioctl(snd_fd, SND_IOCTL_SET_VOLUME, &volume);
	volume = 0;
	printf("current volume is %d\n", volume);

	close(snd_fd);
	*/
	return API_SUCCESS;
}



int media_stop(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	pthread_mutex_lock(&media_hld->api_lock);

	hcplayer_stop2(media_hld->player, m_closevp, m_fillblack);
	media_hld->state = MEDIA_STOP;
	media_hld->speed = 0;
	pthread_mutex_unlock(&media_hld->api_lock);
	return API_SUCCESS;
}

int media_pause(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);

	pthread_mutex_lock(&media_hld->api_lock);
	hcplayer_pause(media_hld->player);
	media_hld->state = MEDIA_PAUSE;
	media_hld->speed = 0;
	pthread_mutex_unlock(&media_hld->api_lock);
	return API_SUCCESS;
}

int media_resume(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	pthread_mutex_lock(&media_hld->api_lock);
	hcplayer_resume(media_hld->player);
	media_hld->state = MEDIA_PLAY;
	media_hld->speed = 0;
	pthread_mutex_unlock(&media_hld->api_lock);
	return API_SUCCESS;
}

int media_seek(media_handle_t *media_hld, int time_sec)
{
	ASSERT_API(media_hld);

	pthread_mutex_lock(&media_hld->api_lock);
	hcplayer_seek(media_hld->player, time_sec * 1000);
	media_hld->state = MEDIA_PLAY;
	media_hld->speed = 0;
	pthread_mutex_unlock(&media_hld->api_lock);
	return API_SUCCESS;
}

//1x, 2x, 4x, 8x, 16x, 24x, 32x
int media_fastforward(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	int speed;
	int speed_cnt;

	pthread_mutex_lock(&media_hld->api_lock);
	speed_cnt = sizeof(m_ff_speed)/sizeof(m_ff_speed[0]);

	if (media_hld->state != MEDIA_FF){
		speed = 1;
	}else {
		speed = media_hld->speed;
		speed += 1;
		speed = speed % speed_cnt;
	}
	printf("%s(), line:%d. speed: %d\n", __FUNCTION__, __LINE__, m_ff_speed[speed]);
	hcplayer_set_speed_rate(media_hld->player, m_ff_speed[speed]);
	media_hld->speed = speed;
	if (0 == speed) //normal play
		media_hld->state = MEDIA_PLAY;
	else
		media_hld->state = MEDIA_FF;

	pthread_mutex_unlock(&media_hld->api_lock);

	return API_SUCCESS;
}

//1x, -2x, -4x, -8x, -16x, -24x, -32x
int media_fastbackward(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	int speed;
	int speed_cnt;

	pthread_mutex_lock(&media_hld->api_lock);
	speed_cnt = sizeof(m_fb_speed)/sizeof(m_fb_speed[0]);

	if (media_hld->state != MEDIA_FB){
		speed = 1;	
	}else{
		speed = media_hld->speed;
		speed += 1;
		speed = speed % speed_cnt;
	}
	printf("%s(), line:%d. speed: %d\n", __FUNCTION__, __LINE__, m_fb_speed[speed]);
	hcplayer_set_speed_rate(media_hld->player, m_fb_speed[speed]);
	media_hld->speed = speed;
	if (0 == speed) //normal play
		media_hld->state = MEDIA_PLAY;
	else
		media_hld->state = MEDIA_FB;

	pthread_mutex_unlock(&media_hld->api_lock);

	return API_SUCCESS;
}

//1x, 1/2, 1/4, 1/8, 1/16, 1/24
int media_slowforward(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	int speed;
	int speed_cnt;

	pthread_mutex_lock(&media_hld->api_lock);
	speed_cnt = sizeof(m_sf_speed)/sizeof(m_sf_speed[0]);

	if (media_hld->state != MEDIA_SF){
		speed = 1;
	} else {
		speed = media_hld->speed;
		speed += 1;
		speed = speed % speed_cnt;
	}
	printf("%s(), line:%d. speed: %f\n", __FUNCTION__, __LINE__, m_sf_speed[speed]);
	hcplayer_set_speed_rate(media_hld->player, m_sf_speed[speed]);
	media_hld->speed = speed;
	if (0 == speed) //normal play
		media_hld->state = MEDIA_PLAY;
	else
		media_hld->state = MEDIA_SF;


	pthread_mutex_unlock(&media_hld->api_lock);

	return API_SUCCESS;
}

//1x, -1/2, -1/4, -1/8, -1/16, -1/24
int media_slowbackward(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	int speed;
	int speed_cnt;

	pthread_mutex_lock(&media_hld->api_lock);
	speed_cnt = sizeof(m_sb_speed)/sizeof(m_sb_speed[0]);

	if (media_hld->state != MEDIA_SB){
		speed = 1;	
	}else{
		speed = media_hld->speed;
		speed += 1;
		speed = speed % speed_cnt;
	}
	printf("%s(), line:%d. speed: %f\n", __FUNCTION__, __LINE__, m_sb_speed[speed]);
	hcplayer_set_speed_rate(media_hld->player, m_sb_speed[speed]);
	media_hld->speed = speed;
	if (0 == speed) //normal play
		media_hld->state = MEDIA_PLAY;
	else
		media_hld->state = MEDIA_SB;


	pthread_mutex_unlock(&media_hld->api_lock);

	return API_SUCCESS;
}

uint32_t media_get_playtime(media_handle_t *media_hld)
{
	uint32_t play_time;
	ASSERT_API(media_hld);
	pthread_mutex_lock(&media_hld->api_lock);
	play_time = (uint32_t)(hcplayer_get_position(media_hld->player)/1000);
	media_hld->play_time = play_time;
	printf("play time %d\n", play_time);
	pthread_mutex_unlock(&media_hld->api_lock);
	return play_time;
}

uint32_t media_get_totaltime(media_handle_t *media_hld)
{
	uint32_t total_time;
	ASSERT_API(media_hld);
	total_time = (uint32_t)(hcplayer_get_duration(media_hld->player)/1000);
	media_hld->play_time = total_time;
	return total_time;
}


media_state_t media_get_state(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	return media_hld->state;
}

uint8_t media_get_speed(media_handle_t *media_hld)
{
	ASSERT_API(media_hld);
	return media_hld->speed;
}