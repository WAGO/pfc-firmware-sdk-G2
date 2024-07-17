.SUFFIXES:
 
.PHONY: default
default: all
 
PROJECT_ROOT ?= .
SRC_DIR = $(PROJECT_ROOT)/src
 
include $(SCRIPT_DIR)/platform_1.mk
 
LINT_RULE_FILES = $(LINT_CONFIG_DIR)/lint-rules/pfc.lnt
 
MAIN_BUILDTARGETS +=alltests.elf
TEST_BUILDTARGETS +=
BUILDTARGETS += $(MAIN_BUILDTARGETS) $(TEST_BUILDTARGETS)


alltests.elf_PKGCONFIGS_LIBS += glib-2.0

alltests.elf_STATICALLYLINKED += gmock_main gmock gtest
alltests.elf_LIBS += $(alltests.elf_STATICALLYLINKED)
alltests.elf_PREREQUISITES += $(call lib_buildtarget,$(alltests.elf_LIBS) $(alltests.elf_PKGCONFIGS_LIBS),alltests.elf)
alltests.elf_SOURCES += $(SRC_DIR)/set_serial_mode_test.cpp
alltests.elf_CXXFLAGS += $(call option_std,gnu++14)
srvunittests.elf_CCXXFLAGS += $(call pkg_config_cflags,$(alltests.elf_PKGCONFIGS_LIBS))
alltests.elf_CPPFLAGS += $(call pkg_config_cppflags,$(alltests.elf_PKGCONFIGS_LIBS))

alltests.elf_LDFLAGS += $(call option_lib,$(alltests.elf_LIBS),alltests.elf)
alltests.elf_LDFLAGS += $(call pkg_config_ldflags,$(alltests.elf_PKGCONFIGS_LIBS))


include $(SCRIPT_DIR)/buildclang_1.mk