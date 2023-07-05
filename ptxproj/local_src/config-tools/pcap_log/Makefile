#######################################################################################################################
# Makefile for PTXdist package pcap_log
# This makefile uses the infrastructure provided by ct-build.
.SUFFIXES:  # Delete make's default suffix rules
.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration
PROJECT_ROOT ?= .
PTXDIST_PACKAGE ?= PCAP_LOG
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/pcap_log.make

SRC_DIR = $(PROJECT_ROOT)/src

INC_DIR = $(PROJECT_ROOT)/src
INC_DIR += $(PROJECT_ROOT)/src/wago_pcap
#INC_DIR += -isystem ${SYSROOT}/usr/include/netconf

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
	pcap_log.elf \
	libwago_pcap.a

TEST_BUILDTARGETS += \
	alltests.elf

BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

INSTALL_TARGETS += \
	$(DESTDIR)/etc/config-tools/pcap_log

#######################################################################################################################
# Settings shared between build targets
SHARED_CCXXFLAGS += -fPIC
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++17)
SHARED_CXXFLAGS  += $(call option_std,c++17)
#SHARED_CXXFLAGS  += -fvisibility-inlines-hidden

# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))

#######################################################################################################################
# Settings for build target libwago_pcap.a
libwago_pcap.a_LIBS             += curl
libwago_pcap.a_STATICALLYLINKED += 
libwago_pcap.a_PKG_CONFIGS      += nlohmann_json libnetconf
libwago_pcap.a_DISABLEDWARNINGS += 
libwago_pcap.a_PREREQUISITES    += $(call local_prerequisites,libwago_pcap.a)
libwago_pcap.a_SOURCES          += $(call glob_r,$(addprefix $(SRC_DIR)/wago_pcap/**/*.,$(SOURCE_FILE_EXTENSIONS)))
libwago_pcap.a_CPPFLAGS         += $(addprefix -I,$(INC_DIR))
libwago_pcap.a_CPPFLAGS         += $(call pkg_config_cppflags,$(libwago_pcap.a_PKG_CONFIGS))
libwago_pcap.a_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
libwago_pcap.a_CFLAGS           += $(SHARED_CFLAGS)
libwago_pcap.a_CFLAGS           += $(libwago_pcap.a_CCXXFLAGS)
libwago_pcap.a_CFLAGS           += $(call pkg_config_cflags,$(libwago_pcap.a_PKG_CONFIGS))
libwago_pcap.a_CXXFLAGS         += $(SHARED_CXXFLAGS)
libwago_pcap.a_CXXFLAGS         += $(libwago_pcap.a_CCXXFLAGS)
libwago_pcap.a_CXXFLAGS         += $(call pkg_config_cxxflags,$(libwago_pcap.a_PKG_CONFIGS))
libwago_pcap.a_LDFLAGS          += $(call option_lib,$(libwago_pcap.a_LIBS),libwago_pcap)
libwago_pcap.a_LDFLAGS          += $(call pkg_config_ldflags,$(libwago_pcap.a_PKG_CONFIGS))

#######################################################################################################################
# Settings for build target alltests.elf
alltests.elf_LIBS             += gmock_main gmock gtest wago_pcap curl
alltests.elf_STATICALLYLINKED += gmock_main gmock gtest
alltests.elf_PKG_CONFIGS      += nlohmann_json libnetconf
alltests.elf_DISABLEDWARNINGS += 
alltests.elf_PREREQUISITES    += $(call local_prerequisites,alltests.elf)
alltests.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/test-src/*.,$(SOURCE_FILE_EXTENSIONS)))
alltests.elf_CPPFLAGS         += $(addprefix -I,$(INC_DIR))
alltests.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(alltests.elf_PKG_CONFIGS))
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
alltests.elf_CLANG_TIDY_CHECKS += -google-runtime-references -google-build-using-namespace
alltests.elf_CLANG_TIDY_CHECKS += -modernize-raw-string-literal

#######################################################################################################################
# Settings for build target pcap_log.elf

pcap_log.elf_LIBS             += wago_pcap pcap curl ctlog
pcap_log.elf_STATICALLYLINKED += wago_pcap
pcap_log.elf_PKG_CONFIGS      += nlohmann_json libnetconf glib-2.0
pcap_log.elf_DISABLEDWARNINGS += 
pcap_log.elf_PREREQUISITES    += $(call local_prerequisites,pcap_log.elf)
pcap_log.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/pcap_log_main/**/*.,$(SOURCE_FILE_EXTENSIONS)))
pcap_log.elf_CPPFLAGS         += $(addprefix -I,$(INC_DIR))
pcap_log.elf_CPPFLAGS         += -isystem ${SYSROOT}/usr/include/glib-2.0
pcap_log.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(pcap_log.elf_PKG_CONFIGS))
pcap_log.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
pcap_log.elf_CCXXFLAGS        += $(call option_disable_warning,$(pcap_log.elf_DISABLEDWARNINGS))
pcap_log.elf_CFLAGS           += $(SHARED_CFLAGS)
pcap_log.elf_CFLAGS           += $(pcap_log.elf_CCXXFLAGS)
pcap_log.elf_CFLAGS           += $(call pkg_config_cflags,$(pcap_log.elf_PKG_CONFIGS))
pcap_log.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
pcap_log.elf_CXXFLAGS         += $(pcap_log.elf_CCXXFLAGS)
pcap_log.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(pcap_log.elf_PKG_CONFIGS))
pcap_log.elf_LDFLAGS          += $(call option_lib,$(pcap_log.elf_LIBS),pcap_log.elf)
pcap_log.elf_LDFLAGS          += $(call pkg_config_ldflags,$(pcap_log.elf_PKG_CONFIGS))

#######################################################################################################################
# Build infrastructure
include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk

#######################################################################################################################
# Custom rules
$(DESTDIR)/etc/config-tools/%: $(BIN_DIR)/%.elf | $(DESTDIR)/etc/config-tools
	cp $< $@

$(DESTDIR)/etc/config-tools: $(DESTDIR)
	mkdir -p $(DESTDIR)/etc/config-tools

#######################################################################################################################
# Bootstrapping
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

