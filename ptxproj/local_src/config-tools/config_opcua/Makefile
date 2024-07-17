#######################################################################################################################
# Makefile for PTXdist package config_opcua
# This makefile uses the infrastructure provided by ct-build.

.SUFFIXES:  # Delete make's default suffix rules

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= CONFIG_OPCUA
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/config_opcua.make

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
	config_opcua.elf \
	libopcua-config.a

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/etc/config-tools/config_opcua

#######################################################################################################################
# Settings shared between build targets

SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu11)
SHARED_CXXFLAGS  += $(call option_std,gnu++14)
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden
SHARED_INCLUDES  += -isystem $(SYSROOT)/usr/include/glib-2.0

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libopcua-config.a

libopcua-config.a_LIBS             +=
libopcua-config.a_STATICALLYLINKED +=
libopcua-config.a_PKG_CONFIGS      += glib-2.0 typelabel nlohmann_json
libopcua-config.a_DISABLEDWARNINGS +=
libopcua-config.a_PREREQUISITES    += $(call local_prerequisites,libopcua-config.a)
libopcua-config.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/opcua-config/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libopcua-config.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libopcua-config.a_PKG_CONFIGS))
libopcua-config.a_CPPFLAGS         += $(SHARED_INCLUDES)
libopcua-config.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libopcua-config.a_CFLAGS           += $(SHARED_CFLAGS)
libopcua-config.a_CFLAGS           += $(libopcua-config.a_CCXXFLAGS)
libopcua-config.a_CFLAGS           += $(call pkg_config_cflags,$(libopcua-config.a_PKG_CONFIGS))
libopcua-config.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libopcua-config.a_CXXFLAGS         += $(libopcua-config.a_CCXXFLAGS)
libopcua-config.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libopcua-config.a_PKG_CONFIGS))
libopcua-config.a_LDFLAGS          += $(call option_lib,$(libopcua-config.a_LIBS),libopcua-config)
libopcua-config.a_LDFLAGS          += $(call pkg_config_ldflags,$(libopcua-config.a_PKG_CONFIGS))

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_LIBS             += opcua-config gmock_main gmock gtest
alltests.elf_STATICALLYLINKED += opcua-config gmock_main gmock gtest
alltests.elf_PKG_CONFIGS      += $(libopcua-config.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS +=
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS         += $(SHARED_INCLUDES)
alltests.elf_CPPFLAGS         += -DOPCUACONFIGTOOL_VERSION=$(CONFIG_OPCUA_VERSION)
alltests.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
alltests.elf_CCXXFLAGS        += $(call option_disable_warning,$(alltests.elf_DISABLEDWARNINGS))
alltests.elf_CFLAGS           += $(SHARED_CFLAGS)
alltests.elf_CFLAGS           += $(alltests.elf_CCXXFLAGS)
alltests.elf_CFLAGS           += $(call pkg_config_cflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
alltests.elf_CXXFLAGS         += $(alltests.elf_CCXXFLAGS)
alltests.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_LDFLAGS          += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS          += $(call pkg_config_ldflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CLANG_TIDY_CHECKS += $(GTEST_CLANG_TIDY_CHECKS)

#######################################################################################################################
# Settings for build target config_opcua.elf

config_opcua.elf_LIBS             += opcua-config
config_opcua.elf_STATICALLYLINKED += opcua-config
config_opcua.elf_PKG_CONFIGS      += $(libopcua-config.a_PKG_CONFIGS)
config_opcua.elf_DISABLEDWARNINGS +=
config_opcua.elf_PREREQUISITES    += $(call local_prerequisites,config_opcua.elf)
config_opcua.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/*.,$(SOURCE_FILE_EXTENSIONS)))
config_opcua.elf_CPPFLAGS         += -I$(SRC_DIR)
config_opcua.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(config_opcua.elf_PKG_CONFIGS))
config_opcua.elf_CPPFLAGS         += $(SHARED_INCLUDES)
config_opcua.elf_CPPFLAGS         += -DOPCUACONFIGTOOL_VERSION=$(CONFIG_OPCUA_VERSION)
config_opcua.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
config_opcua.elf_CCXXFLAGS        += $(call option_disable_warning,$(config_opcua.elf_DISABLEDWARNINGS))
config_opcua.elf_CFLAGS           += $(SHARED_CFLAGS)
config_opcua.elf_CFLAGS           += $(config_opcua.elf_CCXXFLAGS)
config_opcua.elf_CFLAGS           += $(call pkg_config_cflags,$(config_opcua.elf_PKG_CONFIGS))
config_opcua.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
config_opcua.elf_CXXFLAGS         += $(config_opcua.elf_CCXXFLAGS)
config_opcua.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(config_opcua.elf_PKG_CONFIGS))
config_opcua.elf_LDFLAGS          += $(call option_lib,$(config_opcua.elf_LIBS),config_opcua.elf)
config_opcua.elf_LDFLAGS          += $(call pkg_config_ldflags,$(config_opcua.elf_PKG_CONFIGS))


#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

HDR_DIR=$(DESTDIR)/usr/include/config_opcua

$(DESTDIR)/etc/config-tools/%: $(BIN_DIR)/%.elf | $(DESTDIR)/etc/config-tools
	cp $< $@

$(DESTDIR)/etc/config-tools: $(DESTDIR)
	mkdir -p $(DESTDIR)/etc/config-tools

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
