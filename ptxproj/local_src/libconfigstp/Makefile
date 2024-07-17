#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2023 WAGO GmbH & Co. KG
#
##############################################################################################################################################################################################################################################
# Makefile for PTXdist package libconfigstp
# This makefile uses the infrastructure provided by ct-build.
.SUFFIXES:  # Delete make's default suffix rules

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= LIBCONFIGSTP
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/libconfigstp.make

SRC_DIR = $(PROJECT_ROOT)

#######################################################################################################################
# Optional package/ platform feature configuration

#/# Include common defines and macros for use in this make file.
-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
	$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

LIBCONFIGSTP_HEADER_INSTALL_DIR = $(DESTDIR)/usr/include/libconfigstp

LIBCONFIGSTP_INSTALL_HEADERS = $(call fglob_r,$(PROJECT_ROOT)/extern, h hpp)
LIBCONFIGSTP_HEADER_INSTALL_TARGETS = $(addprefix $(LIBCONFIGSTP_HEADER_INSTALL_DIR)/, $(notdir $(LIBCONFIGSTP_INSTALL_HEADERS)))

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
	libconfigstp.a \
	libconfigstp.so

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
    $(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(LIBCONFIGSTP_HEADER_INSTALL_TARGETS) \
	$(DESTDIR)/usr/lib/pkgconfig/libconfigstp.pc \
	$(DESTDIR)/usr/lib/libconfigstp.so.$(LIBCONFIGSTP_VERSION)

#######################################################################################################################
# Code analysis

GCOVR_EXCLUDE += $(libnetconfd_PROJECT_ROOT)/test-src # globally exclude files from coverage report

#######################################################################################################################
# Settings shared between build targets

SHARED_CCXXFLAGS += -fPIC
# SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++17)
# SHARED_CXXFLAGS  += -fvisibility-inlines-hidden

SHARED_INCLUDES  += -I$(SRC_DIR)/extern
SHARED_INCLUDES  += -I$(SRC_DIR)/src
SHARED_INCLUDES  += -I$(SRC_DIR)/src/util
SHARED_INCLUDES  += -I$(SRC_DIR)/src/stp
SHARED_INCLUDES  += -isystem ${SYSROOT}/usr/include/glib-2.0


local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))


#######################################################################################################################
# Settings for build target libconfigstp.so

libconfigstp.so_INCLUDES		 += $(SHARED_INCLUDES)
libconfigstp.so_VERSION          = $(LIBCONFIGSTP_VERSION)
libconfigstp.so_LIBS             +=
libconfigstp.so_STATICALLYLINKED +=
libconfigstp.so_PKG_CONFIGS      += glib-2.0
libconfigstp.so_DISABLEDWARNINGS += psabi
libconfigstp.so_PREREQUISITES    += $(call local_prerequisites,libconfigstp.so)
libconfigstp.so_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libconfigstp.so_CPPFLAGS 		 += $(call uniq, $(libconfigstp.so_INCLUDES))
libconfigstp.so_CPPFLAGS         += $(call pkg_config_cppflags,$(libconfigstp.so_PKG_CONFIGS))
libconfigstp.so_CPPFLAGS         += -D'LIBCONFIGSTP_API=WC_API_EXPORT'
libconfigstp.so_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libconfigstp.so_CCXXFLAGS        += $(call option_disable_warning,$(libconfigstp.so_DISABLEDWARNINGS))
libconfigstp.so_CCXXFLAGS 		 += $(OPTION_PIC)
libconfigstp.so_CFLAGS           += $(SHARED_CFLAGS)
libconfigstp.so_CFLAGS           += $(libconfigstp.so_CCXXFLAGS)
libconfigstp.so_CFLAGS           += $(call pkg_config_cflags,$(libconfigstp.so_PKG_CONFIGS))
libconfigstp.so_CXXFLAGS         += $(SHARED_CXXFLAGS)
libconfigstp.so_CXXFLAGS         += $(libconfigstp.so_CCXXFLAGS)
libconfigstp.so_CXXFLAGS         += $(call pkg_config_cxxflags,$(libconfigstp.so_PKG_CONFIGS))
libconfigstp.so_LDFLAGS          += $(call option_lib,$(libconfigstp.so_LIBS) $(libconfigstp.so_PKG_CONFIG_LIBS),libconfigstp.so)
libconfigstp.so_LDFLAGS          += $(call pkg_config_ldflags,$(libconfigstp.so_PKG_CONFIGS))
libconfigstp.so_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libconfigstp.so_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
libconfigstp.so_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libconfigstp.so_CLANG_TIDY_CHECKS += -google-runtime-references

#######################################################################################################################
# Settings for build target libconfigstp.a

libconfigstp.a_INCLUDES			+= $(SHARED_INCLUDES)
libconfigstp.a_LIBS             +=
libconfigstp.a_STATICALLYLINKED +=
libconfigstp.a_PKG_CONFIGS      += glib-2.0
libconfigstp.a_DISABLEDWARNINGS += useless-cast psabi
libconfigstp.a_PREREQUISITES    += $(call local_prerequisites,libconfigstp.a)
libconfigstp.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libconfigstp.a_CPPFLAGS 		+= $(call uniq, $(libconfigstp.a_INCLUDES))
libconfigstp.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libconfigstp.a_PKG_CONFIGS))
libconfigstp.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libconfigstp.a_CCXXFLAGS        += $(call option_disable_warning,$(libconfigstp.a_DISABLEDWARNINGS))
libconfigstp.a_CCXXFLAGS 		+= $(OPTION_PIC)
libconfigstp.a_CFLAGS           += $(SHARED_CFLAGS)
libconfigstp.a_CFLAGS           += $(libconfigstp.a_CCXXFLAGS)
libconfigstp.a_CFLAGS           += $(call pkg_config_cflags,$(libconfigstp.a_PKG_CONFIGS))
libconfigstp.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libconfigstp.a_CXXFLAGS         += $(libconfigstp.a_CCXXFLAGS)
libconfigstp.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libconfigstp.a_PKG_CONFIGS))
libconfigstp.a_LDFLAGS          += $(call option_lib,$(libconfigstp.a_LIBS) $(libconfigstp.a_PKG_CONFIG_LIBS),libconfigstp.a)
libconfigstp.a_LDFLAGS          += $(call pkg_config_ldflags,$(libconfigstp.a_PKG_CONFIGS))
libconfigstp.a_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
libconfigstp.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
libconfigstp.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
libconfigstp.a_CLANG_TIDY_CHECKS += -google-runtime-references


#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_LIBS             += gmock_main gmock gtest configstp $(libconfigstp.a_LIBS)
alltests.elf_STATICALLYLINKED += gmock_main gmock gtest configstp
alltests.elf_PKG_CONFIGS      += $(libconfigstp.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS += suggest-override
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += $(libconfigstp.a_INCLUDES)
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
alltests.elf_CCXXFLAGS        += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS           += $(SHARED_CFLAGS)
alltests.elf_CFLAGS           += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS           += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS         += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS          += $(call option_lib,$(alltests.elf_LIBS) $(alltests.elf_PKG_CONFIG_LIBS),alltests.elf)
alltests.elf_LDFLAGS          += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CLANG_TIDY_RULESET = $(CLANG_TIDY_CHECKS)
alltests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
alltests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
alltests.elf_CLANG_TIDY_CHECKS += -google-runtime-references
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)

alltests.elf_GCOVR_FILTER += $(libconfigstp_PROJECT_ROOT) # filter only source files in this sub-module
alltests.elf_GCOVR_SEARCH_PATH += libconfigstp.a # modules to include into this test's coverage report


#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

$(LIBCONFIGSTP_HEADER_INSTALL_DIR)/% : $(PROJECT_ROOT)/extern/%
	mkdir -p $(dir $@) && cp $(realpath $<) $@

$(DESTDIR)/usr/lib/pkgconfig/%.pc: $(PROJECT_ROOT)/%.pc | $(DESTDIR)/usr/lib/pkgconfig
	cp $< $@

$(DESTDIR)/usr/bin/%: $(BIN_DIR)/%.elf | $(DESTDIR)/usr/bin
	cp $< $@

#######################################################################################################################
# Bootstrapping

#/# This rule complains if the folder pointed to by SCRIPT_DIR does not contain any .mk files.
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
