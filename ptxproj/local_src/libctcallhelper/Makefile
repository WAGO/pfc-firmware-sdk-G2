#Set this to @ to keep the makefile quiet.
SILENCE = @
#Set this to --silent to keep libtool quiet.
LIBTOOL_SILENCE=--silent 

#-------------------------------------------------------------------
# Outputs
#-------------------------------------------------------------------
COMPONENT_NAME = ctcallhelper


#-------------------------------------------------------------------
# Inputs
#-------------------------------------------------------------------
PROJECT_HOME_DIR = .


#-------------------------------------------------------------------
# Compiler to use
#-------------------------------------------------------------------
CC := $(CC)
LD := $(LD)
CXX := $(CXX)

LIBTOOL=$(CROSS_LIBTOOL)libtool

ifeq ($(USE_CLANG), y)
CC=clang
LD=llvm-ld
CXX=clang++
endif


#-------------------------------------------------------------------
# Parameters passed to this Makefile
#-------------------------------------------------------------------

# distribution directory (is controlled by ptx-dist)
# for local builds set this to your ptxdist platform path

ifndef SYSROOT
$(error Environment variable SYSROOT not set propery!)
endif

#-------------------------------------------------------------------
# Compiler Warnings
#-------------------------------------------------------------------

MK_WARNINGFLAGS += -Wall 
MK_WARNINGFLAGS += -Werror
MK_WARNINGFLAGS += -Wstrict-prototypes 
MK_WARNINGFLAGS += -Wextra  
MK_WARNINGFLAGS += -Wswitch-default 
MK_WARNINGFLAGS += -Wswitch-enum

#-------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------

MK_CFLAGS:= $(CFLAGS) 
MK_CFLAGS += -g3
MK_CFLAGS += -O2
MK_CFLAGS += -std=c99 
MK_CFLAGS += -D_GNU_SOURCE
MK_CFLAGS += -D__USE_GNU

MK_CFLAGS += -fdiagnostics-show-option
MK_CFLAGS += -fstrict-aliasing


#-------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------
MK_LDFLAGS:= $(LDFLAGS)
MK_LDFLAGS += -rdynamic

#-------------------------------------------------------------------
# LD Flags
#-------------------------------------------------------------------
MK_LDFLAGS  += -L$(SYSROOT)/lib -L$(SYSROOT)/usr/lib\
				-Wl,-rpath-link -Wl,$(SYSROOT)/lib\
				-Wl,-rpath-link -Wl,$(SYSROOT)/usr/lib\
				-rpath /usr/lib



#-------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------

SRC_DIRS = \
	src\
	
	

#-------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------
	
INCLUDE_DIRS =\
	  inc\
	  
#-------------------------------------------------------------------
# System Includes
#-------------------------------------------------------------------

MK_CFLAGS += -isystem $(SYSROOT)/usr/include
MK_CFLAGS += -isystem $(SYSROOT)/include
MK_CFLAGS += -isystem $(SYSROOT)/usr/include/glib-2.0
MK_CFLAGS += -isystem $(SYSROOT)/usr/lib/glib-2.0/include
MK_CFLAGS += -isystem $(SYSROOT)/usr/include/dbus-1.0
MK_CFLAGS += -isystem $(SYSROOT)/usr/lib/dbus-1.0/include


# -------------------------------------------------------------------
# Miscellaneous Settings
# -------------------------------------------------------------------
# disable the makefile worker's debug switch (we use the debug switch, 
# that gets passed in LIBSERCOMM_DEBUG_LEVEL.
MK_ENABLE_DEBUG=n

include build/LibraryMakefileWorker.mk
