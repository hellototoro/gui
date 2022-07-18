GUI_APP_SRC += kmp_search.c
GUI_APP_SRC += wpa_tools.c
GUI_APP_SRC += wpa_wifi_control.c
GUI_APP_SRC += wpa_wifi_events.c
GUI_APP_CPP_SRC += wpa_wifi_info.cpp

DEPPATH += --dep-path $(ROOT_DIR)/hcapi/wpa_funs
VPATH += :$(ROOT_DIR)/hcapi/wpa_funs

CFLAGS += "-I$(ROOT_DIR)/hcapi/wpa_funs"
