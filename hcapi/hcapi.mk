GUI_APP_SRC += com_api.c
GUI_APP_SRC += key.c
GUI_APP_SRC += media_player.c
GUI_APP_SRC += os_api.c
GUI_APP_SRC += wifi_api.c
GUI_APP_CPP_SRC += cast_api.cpp

DEPPATH += --dep-path $(ROOT_DIR)/hcapi
VPATH += :$(ROOT_DIR)/hcapi

CFLAGS += "-I$(ROOT_DIR)/hcapi"

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(ROOT_DIR)/hcapi/wpa_funs/wpa_funs.mk
include $(ROOT_DIR)/hcapi/hc_src/hc_src.mk
