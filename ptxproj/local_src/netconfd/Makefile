#######################################################################################################################
# Makefile for PTXdist package netconfd
# This makefile uses the infrastructure provided by ct-build.


.SUFFIXES:

.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

PROJECT_ROOT ?= .

PTXDIST_PACKAGE ?= NETCONFD
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/netconfd.make

INCLUDES += \
-I$(PROJECT_ROOT)/src/

# JSON_HAS_CPP_14: nlohmann: Prevent ambiguous call error [clang-diagnostic-error]
CFLAGS += $(call option_std,gnu99) -DJSON_HAS_CPP_14
CXXFLAGS += $(call option_std,gnu++17) -DJSON_HAS_CPP_14

SHARED_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-compat
SHARED_CLANG_TIDY_CHECKS += -clang-diagnostic-c++98-c++11-c++14-compat
SHARED_CLANG_TIDY_CHECKS += -google-runtime-references # removed from the //Google C++ Style Guide//

VALGRIND_FLAGS += --suppressions=$(PTXDIST_PLATFORMDIR)/sysroot-target/usr/share/glib-2.0/valgrind/glib.supp

#######################################################################################################################
# Optional package/ platform feature configuration

-include $(SCRIPT_DIR)/platform_1.mk

######################################################################################################################
# Custom toolchain configuration

LINT_RULE_FILES = \
$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt \
$(wildcard $(PROJECT_ROOT)/suppress.lnt)

#######################################################################################################################
# Build target configuration

MODULES += \
modules/netconfd/module.mk \
modules/dbus/module.mk \
modules/utility/module.mk \
modules/common/module.mk \
modules/clientapi/module.mk

MK_FILES += $(MODULES)

BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)

OPTION_HIDE_SYMBOLS ?= -fvisibility=hidden

NETCONFD_SHARED_INCLUDES :=     		\
-isystem$(SYSROOT)/usr/include

#######################################################################################################################
# Settings for module build targets

include $(MODULES)

-include $(PROJECT_ROOT)/license_install.mk
MK_FILES += $(PROJECT_ROOT)/license_install.mk

#######################################################################################################################
# Build infrastructure

-include $(SCRIPT_DIR)/buildclang_1.mk
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk

#######################################################################################################################
# Custom rules

$(DESTDIR)/usr/include/% : $(PROJECT_ROOT)/extern/%
	mkdir -p $(dir $@) && cp $(realpath $<) $@

# install files into 'etc' folder
$(DESTDIR)/etc/% : $(PROJECT_ROOT)/etc/%
	mkdir -p $(dir $@) && cp $(realpath $<) $@

#######################################################################################################################
# Bootstrapping

$(SCRIPT_DIR)/%.mk :
	$(error build scripts unavailable - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)) or checkout ct-build)
