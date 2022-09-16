#
# Makefile
#
CC ?= gcc
ROOT_DIR ?= ${shell pwd}

SYS_DIR = -I$(ROOT_DIR) \
		-I$(STAGING_DIR)/usr/include \
		-I$(STAGING_DIR)/usr/include/hcuapi

LVGL_HEADERS = -I$(STAGING_DIR)/usr/include/lvgl/ \
		-I$(STAGING_DIR)/usr/include/lvgl/lvgl

CFLAGS ?= -O3 -g $(SYS_DIR)/ -Wall -Wshadow -Wundef -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare
CPPFLAGS ?= -O3 -g $(SYS_DIR)/ -Wall -Wshadow -Wundef -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare

CODE_MICRO = 
CODE_MICRO += -DHCCHIP_GCC
CODE_MICRO += -DDLNA_SUPPORT
CODE_MICRO += -DMIRACAST_SUPPORT
CODE_MICRO += -DAIRCAST_SUPPORT

CFLAGS += $(CODE_MICRO)
CFLAGS += $(LVGL_HEADERS)
#CPPFLAGS += -std=c++17
CPPFLAGS += "-I$(ROOT_DIR)/boost_1_79_0" $(CODE_MICRO) $(LVGL_HEADERS)

LDFLAGS ?= -lm -llvgl -lmiracast -laircast -ldlna
BIN = gui_app

GUI_LIBS = -lffplayer -lpthread -lge -Wl,--start-group -lstdc++ $(CAST_LIBS) -Wl,--end-group
GUI_LIBS += -lcjson -lwpa_client -lhccast-com -lhccast-net -lhccast-wl -lmdns -lcrypto -lhcfota

MAINSRC = ./main.c

include $(ROOT_DIR)/hcapi/hcapi.mk
include $(ROOT_DIR)/application/gui_app.mk
include $(ROOT_DIR)/lv_i18n/lv_i18n.mk

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))
GUI_OBJS = $(GUI_APP_SRC:.c=$(OBJEXT))
GUI_OBJS += $(GUI_APP_CPP_SRC:.cpp=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

all: make_gui_app

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	echo "CC $<"

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@
	echo "CC $<"

default: $(AOBJS) $(COBJS) $(MAINOBJ)
	@$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS) -lffplayer
	
make_gui_app:	$(COBJS) $(GUI_OBJS) $(MAINOBJ)
	@$(CC) -o $(BIN) $(MAINOBJ) $(GUI_OBJS) $(COBJS) $(LDFLAGS) $(GUI_LIBS)
	
clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ) $(GUI_OBJS)

install:
	@echo "install ......"
