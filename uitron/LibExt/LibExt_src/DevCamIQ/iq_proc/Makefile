#----------------------------------------------------------------------
# set the ROOT directory here
#----------------------------------------------------------------------
ROOT_DIR = ../../../..

#----------------------------------------------------------------------
# set the library name here
#----------------------------------------------------------------------
PRJ_NAME = IQ_PROC_NVT

#----------------------------------------------------------------------
# add/delete source files here
#----------------------------------------------------------------------
SRC = \
    iq_proc.c \
    iq_func.c \
    iq_effect_table.c \
    iq_tuning.c \

ASM =

#----------------------------------------------------------------------
# set the include directory here
#----------------------------------------------------------------------
INC_DIR = . \
    $(ROOT_DIR)/Include \
	../../DevCamIQ \

#----------------------------------------------------------------------
# set the image output directory here
#----------------------------------------------------------------------
IMG_BASE_DIR =  $(ROOT_DIR)/ARC/Lib

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
MAKE_COMMON_DIR =  $(ROOT_DIR)/MakeCommon
include $(MAKE_COMMON_DIR)/OutputLib.txt
