#
# Makefile
#
CC ?= gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${shell pwd}

SYS_DIR = -I$(LVGL_DIR) \
		-I$(STAGING_DIR)/usr/include \
		-I$(STAGING_DIR)/usr/include/hcuapi \
		-I$(STAGING_DIR)/usr/include/libgoahead

#SYS_DIR += -I$(STAGING_DIR)/usr/include/Neptune \
#		-I$(STAGING_DIR)/usr/include/Platinum \


CFLAGS ?= -O3 -g0 $(SYS_DIR)/ -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare
CPPFLAGS ?= -O3 -g0 $(SYS_DIR)/ -Wall -Wshadow -Wundef -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare

LDFLAGS ?= -lm
BIN = gui_app

# CAST_LIBS = -lneptune -lplatinum  -ldlna-porting
# 
GUI_LIBS = -lffplayer -lpthread -Wl,--start-group -lstdc++ $(CAST_LIBS) -lgo -Wl,--end-group
GUI_LIBS += -lwpa_client

#LIBS = -lffplayer -lpthread

#Collect the files to compile
MAINSRC = ./main.cpp

include $(LVGL_DIR)/lvgl/lvgl.mk
include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
include $(LVGL_DIR)/application/gui_app.mk
include $(LVGL_DIR)/lv_i18n/lv_i18n.mk
#include $(LVGL_DIR)/lv_demos/lv_demo.mk

OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))
GUI_OBJS = $(GUI_APP_SRC:.c=$(OBJEXT))
GUI_OBJS += $(GUI_APP_CPP_SRC:.cpp=$(OBJEXT))

MAINOBJ = $(MAINSRC:.cpp=$(OBJEXT))

## MAINOBJ -> OBJFILES

#all: default gui_app
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
