#######################################################################################################################
# Makefile for PTXdist package get_snmp_data
# This makefile uses the infrastructure provided by ct-build.

.SUFFIXES:  # Delete make's default suffix rules

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= GET_SNMP_DATA
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/get_snmp_data.make

SRC_DIR = $(PROJECT_ROOT)/src

#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk
#CLANG_TIDY = clang-tidy-12

#######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
	$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

#######################################################################################################################
# Build target configuration

MAIN_BUILDTARGETS += \
	get_snmp_data.elf \
	libget_snmp_data.a

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/etc/config-tools/get_snmp_data

#######################################################################################################################
# Settings shared between build targets

SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu11)
SHARED_CXXFLAGS  += $(call option_std,gnu++17) -fchar8_t
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden
SHARED_INCLUDES  += -I$(PROJECT_ROOT)/src/get_snmp_data/

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libget_snmp_data.a
libget_snmp_data.a_LIBS             +=
libget_snmp_data.a_STATICALLYLINKED +=
libget_snmp_data.a_PKG_CONFIGS      += snmp-config-lib 
libget_snmp_data.a_DISABLEDWARNINGS +=
libget_snmp_data.a_PREREQUISITES    += $(call local_prerequisites,libget_snmp_data.a)
libget_snmp_data.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/get_snmp_data/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libget_snmp_data.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libget_snmp_data.a_PKG_CONFIGS))
libget_snmp_data.a_CPPFLAGS         += $(SHARED_INCLUDES)
libget_snmp_data.a_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0 #system header that clang tidy should ignore
libget_snmp_data.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libget_snmp_data.a_CFLAGS           += $(SHARED_CFLAGS)
libget_snmp_data.a_CFLAGS           += $(libget_snmp_data.a_CCXXFLAGS)
libget_snmp_data.a_CFLAGS           += $(call pkg_config_cflags,$(libget_snmp_data.a_PKG_CONFIGS))
libget_snmp_data.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libget_snmp_data.a_CXXFLAGS         += $(libget_snmp_data.a_CCXXFLAGS)
libget_snmp_data.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libget_snmp_data.a_PKG_CONFIGS))
libget_snmp_data.a_CLANG_TIDY_CHECKS += $(CLANG_TIDY_CHECKS)
libget_snmp_data.a_CLANG_TIDY_CHECKS += -google-runtime-references
libget_snmp_data.a_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat

#######################################################################################################################
# Settings for build target alltests.elf

alltests.elf_LIBS             += get_snmp_data gmock_main gmock gtest
alltests.elf_STATICALLYLINKED += get_snmp_data gmock_main gmock gtest
alltests.elf_PKG_CONFIGS      += $(libget_snmp_data.a_PKG_CONFIGS)
alltests.elf_DISABLEDWARNINGS +=
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += -I$(SRC_DIR)
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
alltests.elf_CPPFLAGS         += $(SHARED_INCLUDES)
alltests.elf_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0 #system header that clang tidy should ignore
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
# Settings for build target get_snmp_data.elf

get_snmp_data.elf_LIBS             += get_snmp_data
get_snmp_data.elf_STATICALLYLINKED += get_snmp_data
get_snmp_data.elf_PKG_CONFIGS      += $(libget_snmp_data.a_PKG_CONFIGS) glib-2.0
get_snmp_data.elf_DISABLEDWARNINGS +=
get_snmp_data.elf_PREREQUISITES    += $(call local_prerequisites,get_snmp_data.elf)
get_snmp_data.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/*.,$(SOURCE_FILE_EXTENSIONS)))
get_snmp_data.elf_CPPFLAGS         += -I$(SRC_DIR)
get_snmp_data.elf_CPPFLAGS         += $(SHARED_INCLUDES)
get_snmp_data.elf_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0 #system header that clang tidy should ignore
get_snmp_data.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(get_snmp_data.elf_PKG_CONFIGS))
get_snmp_data.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
get_snmp_data.elf_CCXXFLAGS        += $(call option_disable_warning,$(get_snmp_data.elf_DISABLEDWARNINGS))
get_snmp_data.elf_CFLAGS           += $(SHARED_CFLAGS)
get_snmp_data.elf_CFLAGS           += $(get_snmp_data.elf_CCXXFLAGS)
get_snmp_data.elf_CFLAGS           += $(call pkg_config_cflags,$(get_snmp_data.elf_PKG_CONFIGS))
get_snmp_data.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
get_snmp_data.elf_CXXFLAGS         += $(get_snmp_data.elf_CCXXFLAGS)
get_snmp_data.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(get_snmp_data.elf_PKG_CONFIGS))
get_snmp_data.elf_LDFLAGS          += $(call option_lib,$(get_snmp_data.elf_LIBS),get_snmp_data.elf)
get_snmp_data.elf_LDFLAGS          += $(call pkg_config_ldflags,$(get_snmp_data.elf_PKG_CONFIGS), --static)
get_snmp_data.elf_CLANG_TIDY_CHECKS += $(CLANG_TIDY_CHECKS)
get_snmp_data.elf_CLANG_TIDY_CHECKS += -google-runtime-references
get_snmp_data.elf_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat

#######################################################################################################################
# Build infrastructure

include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

HDR_DIR=$(DESTDIR)/usr/include/get_snmp_data

$(DESTDIR)/etc/config-tools/%: $(BIN_DIR)/%.elf | $(DESTDIR)/etc/config-tools
	cp $< $@

$(DESTDIR)/etc/config-tools: $(DESTDIR)
	mkdir -p $(DESTDIR)/etc/config-tools


#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))
