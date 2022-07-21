#include "wpa_wifi_internal.h"
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wpa_tools.h"

#define DEBUG 0
#define TRY_ON_INIT_FAIL 2

int wpa_enable_reset_config_on_init_fail = 1;

static inline int str_match(const char* a, const char* b) {
	return strncmp(a, b, strlen(b)) == 0;
}

static void on_wifi_scan_finished(enum WifiEventType event_type,
                                  const char* raw_msg, size_t raw_msg_len,
                                  void* userdata) {
	struct wpa_ctrl*ctrl = (struct wpa_ctrl*)userdata;

	if (event_type == WIFI_EVENT_SCAN_FINISHED) {
		if (DEBUG) printf("[%s, %d] scan finished.\n", __FUNCTION__, __LINE__);
		wpa_wifi_get_infos_start(ctrl);
	}
}

int wpa_run_supplicant(char *ifname) {
	int ret = 0;
	char cmd[256];

	snprintf(cmd, sizeof(cmd), WPA_SUPPLICANT_BIN " -D" WPA_CTL_DRIVER " -i %s -c " WPA_CONF " -B", ifname);
	if (DEBUG) printf(cmd);
	system(cmd);
	return ret;
}

int wpa_stop_supplicant(void *ctrl) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;
	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "TERMINATE");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (str_match(reply, "FAIL")) {
			ret = -3;
		}

		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_ctrl_deinit(void *ctrl) {
	int ret = 0;
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;
	if (mctrl) {
		wpa_ctrl_close(mctrl);
		mctrl = NULL;
		wpa_wifi_event_callback_rm(WPA_SCAN_RESULTS_CALLBACK_ID);
	}
	return ret;
}

void *wpa_ctrl_init(char *ifname) {
	int ret = -1;
	int retry = 0;
	struct wpa_ctrl * mctrl = NULL;
	char open_buf[64];

	wpa_ctrl_init_retry:

	snprintf(open_buf, sizeof(open_buf)-1, WPA_CTL_PATH "/""%s", ifname);
	mctrl = wpa_ctrl_open(open_buf);
	if (mctrl) {
		ret = 0;
		if (ret) {
			wpa_ctrl_deinit(mctrl);
			return NULL;
		}
	} else {
		// if wpa_ctrl_open failed, wpa_supplicant may not run,
		// try to run wpa_supplicant first, and if it still fails to
		// run wpa_supplicant, then reset config and try again.
		if (retry++ < TRY_ON_INIT_FAIL) {
			if (retry > 1 && wpa_enable_reset_config_on_init_fail) {
				wpa_reset_config();
			}
			wpa_run_supplicant(ifname);
			sleep(2);
			goto wpa_ctrl_init_retry;
		} else {
			printf("[%s, %d] wpa_ctrl_open(%s) failed.\n", __FUNCTION__, __LINE__, open_buf);
			return NULL;
		}
	}
	wpa_wifi_event_callback_add(&on_wifi_scan_finished, (void*)mctrl, WPA_SCAN_RESULTS_CALLBACK_ID);
	return mctrl;
}

int wpa_reset_config() {
	int ret = -1;
	FILE* fd = fopen(WPA_CONF, "w");
	if (fd) {
		if (DEBUG) {
			printf("[%s, %d] okay.\n", __FUNCTION__, __LINE__);
		}
		fprintf(fd, "ctrl_interface=%s\n", WPA_CTL_PATH);
		fprintf(fd, "update_config=1\n");
		fprintf(fd, "ap_scan=1\n");
		fclose(fd);
		ret = 0;
	} else {
		printf("[%s, %d] fopen(" WPA_CONF ", \"w\") failed.\n", __FUNCTION__, __LINE__);
	}
	return ret;
}

int wpa_save_config(void *ctrl) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "SAVE_CONFIG");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (str_match(reply, "FAIL")) {
			ret = -3;
			printf(
			    "The configuration could not be saved.\n"
			    "\n"
			    "The update_config=1 configuration option\n"
			    "must be used for configuration saving to\n"
			    "be permitted.\n");
		}

		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_reload_config(void *ctrl) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "RECONFIGURE");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_scan(void *ctrl) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	size_t reply_len = sizeof(reply) - 1;
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	//frank: clear the wifi ap list first.
	wpa_wifi_infos_clear();

	snprintf(cmd, sizeof(cmd), "SCAN");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_remove(void *ctrl, int network_id) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "REMOVE_NETWORK %d", network_id);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_add(void *ctrl, int* network_id) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "ADD_NETWORK");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (str_match(reply, "FAIL")) {
			ret = -3;
		} else {
			if (network_id) {
				sscanf(reply, "%d", network_id);
			}
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_select(void *ctrl, int network_id) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "SELECT_NETWORK %d", network_id);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_set_ssid(void *ctrl, int network_id, char* ssid) {
	int ret = 0;
	char cmd[256];
	char reply[32];
	size_t reply_len = sizeof(reply) - 1;
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	snprintf(cmd, sizeof(cmd), "SET_NETWORK %d ssid \"%s\"", network_id, ssid);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_set_key_mgmt(void *ctrl, int network_id, char* key_mgmt) {
	int ret = 0;
	char cmd[256];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;

	snprintf(cmd, sizeof(cmd), "SET_NETWORK %d key_mgmt %s", network_id, key_mgmt);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_set_psk(void *ctrl, int network_id, char* psk) {
	if (psk == NULL) {
		return wpa_wifi_set_key_mgmt(ctrl, network_id, "NONE");
	}
	int ret = 0;
	char cmd[256];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "SET_NETWORK %d psk \"%s\"", network_id, psk);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}


int wpa_wifi_set_wep_key(void *ctrl, int network_id, char* key) {
	int ret = 0;
	char cmd[256];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "SET_NETWORK %d wep_key0 \"%s\"", network_id, key);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		} else {
			ret = wpa_wifi_set_key_mgmt(ctrl, network_id, "NONE");
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}


int wpa_wifi_set_scan_ssid(void *ctrl, int network_id, int enable) {
	int ret = 0;
	char cmd[256];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "SET_NETWORK %d scan_ssid %d", network_id, enable);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_enable(void *ctrl, int network_id) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "ENABLE_NETWORK %d", network_id);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_disable(void *ctrl, int network_id) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "DISABLE_NETWORK %d", network_id);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

int wpa_wifi_autoreconnect(void *ctrl, int enable) {
	int ret = 0;
	char cmd[32];
	char reply[32];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "STA_AUTOCONNECT %d", enable);
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (!str_match(reply, "OK")) {
			ret = -3;
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

void wpa_wifi_status_dump(WiFiStatus* status) {
	if(status==NULL) return;
	printf("[WIFI STATUS]:\n");
	printf("ssid=%s\n",status->ap.ssid);
	printf("bssid=%s\n",status->ap.bssid);
	printf("key_mgmt=%s\n",status->key_mgmt);
	printf("wpa_state=%s\n",status->wpa_state);
	printf("freq(MHz)=%d\n",status->ap.frequency);
	printf("dBm=%d\n",status->ap.signal_level_dBm);
	printf("link_speed(Mbps)=%d\n",status->linkspeed);
	printf("flags:\n%s\n", status->ap.flag);
	printf("WEP:%s\n", wpa_wifi_flag_wep(&status->ap)?"y":"n");
	printf("WPA:%s\n", wpa_wifi_flag_wpa(&status->ap)?"y":"n");
	printf("WPA2:%s\n", wpa_wifi_flag_wpa2(&status->ap)?"y":"n");
	printf("WPA3:%s\n", wpa_wifi_flag_wpa3(&status->ap)?"y":"n");
}

static int _wpa_wifi_status(void *ctrl, WiFiStatus* status) {
	int ret = 0;
	char cmd[32];
	char reply[512];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "STATUS");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (str_match(reply, "FAIL")) {
			ret = -3;
		} else {
			// reply:
			// bssid=22:8c:0a:e5:2c:ef
			// freq=2412
			// ssid=note20
			// id=0
			// mode=station
			// wifi_generation=4
			// pairwise_cipher=CCMP
			// group_cipher=CCMP
			// key_mgmt=WPA2-PSK
			// wpa_state=COMPLETED
			// ip_address=192.168.17.64
			// address=2c:d2:6b:f7:59:87
			if(status){
				char buf[32];
				get_last_appear_value_from_reply_lines(reply, "ssid=",  "\n", (char*)&status->ap.ssid, sizeof(status->ap.ssid));
				get_last_appear_value_from_reply_lines(reply, "bssid=", "\n", (char*)&status->ap.bssid, sizeof(status->ap.bssid));
				get_last_appear_value_from_reply_lines(reply, "freq=",  "\n",(char*)&buf, sizeof(buf));
				sscanf(buf, "%d", &status->ap.frequency);
				get_last_appear_value_from_reply_lines(reply, "wpa_state=", "\n",(char*)&status->wpa_state, sizeof(status->wpa_state));
				get_last_appear_value_from_reply_lines(reply, "key_mgmt=",  "\n",(char*)&status->key_mgmt, sizeof(status->key_mgmt));			
				get_last_appear_value_from_reply_lines(reply, "pairwise_cipher=",  "\n",(char*)&buf[0], sizeof(buf));
				for(char *tmpstr=buf;*tmpstr!='\0';tmpstr++){ if(*tmpstr==' ') *tmpstr='+'; }
				snprintf(status->ap.flag, sizeof(status->ap.flag), "[%s-%s]", status->key_mgmt, buf);			

			}
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}

static int _wpa_wifi_signal(void *ctrl, WiFiStatus* status) {
	int ret = 0;
	char cmd[32];
	char reply[512];
	struct wpa_ctrl *mctrl = (struct wpa_ctrl *)ctrl;

	size_t reply_len = sizeof(reply) - 1;
	snprintf(cmd, sizeof(cmd), "SIGNAL_POLL");
	ret = wpa_ctrl_request(mctrl, cmd, sizeof(cmd), reply, &reply_len, NULL);
	reply[reply_len] = '\0';
	if (ret == -1) {
		// save or recv failed
		printf("[%s, %d] failed.\n", __FUNCTION__, __LINE__);
	} else if (ret == -2) {
		// timeout
		printf("[%s, %d] timeout.\n", __FUNCTION__, __LINE__);
	} else if (ret == 0) {
		// send command success
		if (str_match(reply, "FAIL")) {
			ret = -3;
		} else {
			// reply:
			// RSSI=-39
			// LINKSPEED=72
			// NOISE=9999
			// FREQUENCY=2412
			if(status){
				char buf[32]={0};
				get_last_appear_value_from_reply_lines(reply, "RSSI=", NULL, (char*)buf, sizeof(buf));
				sscanf(buf, "%d", &status->ap.signal_level_dBm);
				memset(buf, 0, sizeof(buf));
				get_last_appear_value_from_reply_lines(reply, "LINKSPEED=", NULL, (char*)buf, sizeof(buf));
				sscanf(buf, "%d", &status->linkspeed);								
			}
		}
		if (DEBUG) printf("[%s, %d] %s\n", __FUNCTION__, __LINE__, reply);
	}
	return ret;
}


int wpa_wifi_get_status(void *ctrl, WiFiStatus* status){
	if(status){
		memset(status, 0, sizeof(WiFiStatus));
		_wpa_wifi_status(ctrl, status);
		_wpa_wifi_signal(ctrl, status);
	}
	return 0;
}