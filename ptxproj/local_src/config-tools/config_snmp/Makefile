#######################################################################################################################
# Makefile for PTXdist package snmp-config
# This makefile uses the infrastructure provided by ct-build.

.SUFFIXES:  # Delete make's default suffix rules

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= CONFIG_SNMP
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/config_snmp.make

SRC_DIR = $(PROJECT_ROOT)/src

#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
	$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
	config_snmp.elf \
	libsnmp-config.a

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/etc/config-tools/config_snmp

#######################################################################################################################
# Settings shared between build targets

SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu11)
SHARED_CXXFLAGS  += $(call option_std,c++17) -fchar8_t
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden
SHARED_INCLUDES  += -isystem $(SYSROOT)/usr/include
SHARED_INCLUDES  += -isystem $(SYSROOT)/usr/include/glib-2.0 #system header that clang tidy should ignore
SHARED_INCLUDES  += -I$(PROJECT_ROOT)/src/snmp-config/

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libsnmp-config.a
libsnmp-config.a_LIBS             +=
libsnmp-config.a_STATICALLYLINKED +=
libsnmp-config.a_PKG_CONFIGS      += snmp-config-lib
libsnmp-config.a_DISABLEDWARNINGS +=
libsnmp-config.a_PREREQUISITES    += $(call local_prerequisites,libsnmp-config.a)
libsnmp-config.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/snmp-config/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libsnmp-config.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libsnmp-config.a_PKG_CONFIGS))
libsnmp-config.a_CPPFLAGS         += $(SHARED_INCLUDES)
libsnmp-config.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libsnmp-config.a_CFLAGS           += $(SHARED_CFLAGS)
libsnmp-config.a_CFLAGS           += $(libsnmp-config.a_CCXXFLAGS)
libsnmp-config.a_CFLAGS           += $(call pkg_config_cflags,$(libsnmp-config.a_PKG_CONFIGS))
libsnmp-config.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libsnmp-config.a_CXXFLAGS         += $(libsnmp-config.a_CCXXFLAGS)
libsnmp-config.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libsnmp-config.a_PKG_CONFIGS))
libsnmp-config.a_CLANG_TIDY_CHECKS += $(CLANG_TIDY_CHECKS)
libsnmp-config.a_CLANG_TIDY_CHECKS += -google-runtime-references
libsnmp-config.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_LIBS             += snmp-config gmock_main gmock gtest
alltests.elf_STATICALLYLINKED += snmp-config gmock_main gmock gtest
alltests.elf_PKG_CONFIGS      += $(libsnmp-config.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS +=
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS         += $(SHARED_INCLUDES)
alltests.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
alltests.elf_CCXXFLAGS        += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS           += $(SHARED_CFLAGS)
alltests.elf_CFLAGS           += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS           += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS         += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS          += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS          += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS), --static)
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)
alltests.elf_CLANG_TIDY_CHECKS += -google-runtime-references
alltests.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat


#######################################################################################################################
# Settings for build target config_snmp.elf

config_snmp.elf_LIBS             += snmp-config
config_snmp.elf_STATICALLYLINKED += snmp-config
config_snmp.elf_PKG_CONFIGS      += $(libsnmp-config.a_PKG_CONFIGS) glib-2.0
config_snmp.elf_DISABLEDWARNINGS +=
config_snmp.elf_PREREQUISITES    += $(call local_prerequisites,config_snmp.elf)
config_snmp.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/*.,$(SOURCE_FILE_EXTENSIONS)))
config_snmp.elf_CPPFLAGS         += -I$(SRC_DIR)
config_snmp.elf_CPPFLAGS         += $(SHARED_INCLUDES)
config_snmp.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(config_snmp.elf_PKG_CONFIGS))
config_snmp.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
config_snmp.elf_CCXXFLAGS        += $(call option_disable_warning,$(config_snmp.elf_DISABLEDWARNINGS))
config_snmp.elf_CFLAGS           += $(SHARED_CFLAGS)
config_snmp.elf_CFLAGS           += $(config_snmp.elf_CCXXFLAGS)
config_snmp.elf_CFLAGS           += $(call pkg_config_cflags,$(config_snmp.elf_PKG_CONFIGS))
config_snmp.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
config_snmp.elf_CXXFLAGS         += $(config_snmp.elf_CCXXFLAGS)
config_snmp.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(config_snmp.elf_PKG_CONFIGS))
config_snmp.elf_LDFLAGS          += $(call option_lib,$(config_snmp.elf_LIBS),config_snmp.elf)
config_snmp.elf_LDFLAGS          += $(call pkg_config_ldflags,$(config_snmp.elf_PKG_CONFIGS), --static)
config_snmp.elf_CLANG_TIDY_CHECKS += $(CLANG_TIDY_CHECKS)
config_snmp.elf_CLANG_TIDY_CHECKS += -google-runtime-references
config_snmp.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat

#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

HDR_DIR=$(DESTDIR)/usr/include/config_snmp

$(DESTDIR)/etc/config-tools/%: $(BIN_DIR)/%.elf | $(DESTDIR)/etc/config-tools
	cp $< $@

$(DESTDIR)/etc/config-tools: $(DESTDIR)
	mkdir -p $(DESTDIR)/etc/config-tools


#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
