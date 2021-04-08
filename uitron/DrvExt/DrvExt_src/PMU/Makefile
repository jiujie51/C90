include ../../../Project/ProjectConfig.txt
include ../../../Project/$(ACTIVE_PROJECT)/ModelConfig.txt
include ../../../Project/$(ACTIVE_PROJECT)/ModelConfig_$(MODEL).txt
include ../../../Project/$(ACTIVE_PROJECT)/MakeConfig.txt

#----------------------------------------------------------------------
# set the library name here
#----------------------------------------------------------------------
PRJ_NAME = PMU

#----------------------------------------------------------------------
# add/delete source files here
#----------------------------------------------------------------------
SRC = \
	AXP_cfg.c \
	axp-sply.c \
	IP6303.c

ASM =

#----------------------------------------------------------------------
# set the include directory here
#----------------------------------------------------------------------
INC_DIR = .. \
	../../../Include \

#----------------------------------------------------------------------
# set the image output directory here
#----------------------------------------------------------------------
IMG_BASE_DIR = ../../../ARC/Drv

#----------------------------------------------------------------------
# add additional C and assembly flags here
#----------------------------------------------------------------------
CFLAGS_D = $(C_DEFINE_EXT)
ASMFLAGS_D =

CFLAGS_R = $(C_DEFINE_EXT)
ASMFLAGS_R =

GCCFLAGS = $(C_DEFINE_EXT)

#----------------------------------------------------------------------
# include common parts of the makefile
#----------------------------------------------------------------------
MAKE_COMMON_DIR = ../../../MakeCommon
include $(MAKE_COMMON_DIR)/OutputLib.txt
