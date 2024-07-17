#######################################################################################################################
# Makefile for PTXdist package eeprom-xsection
# This makefile uses the infrastructure provided by ct-build.
#/#
#/# Lines starting with '#/#' are explanations. If you want to use this example as a template for your own project,
#/# remove these lines, for example with sed:
#/# sed -i '/^#\/#.*/d' Makefile
#/# or using eclipse's Find/Replace
#/# Find:^#\/#.*\n  Replace With:[nix]  check Regular expressions
#/#
#/# gnu make's manual can be found here: https://www.gnu.org/software/make/manual/html_node/
#/#
#/# This package compiles into two archive files (.a; first.a, second.a), which are linked into a
#/# shared object file (.so; libeeprom-xsection2.so) together with an API description.
#/# There are also two test executables. alltests.elf runs tests against the API of the shared object, whereas
#/# whitebox.elf calls the no-exported functions in the archives.

.SUFFIXES:  # Delete make's default suffix rules

#/# When make is called without parameters, it reads a file with name 'Makefile' and evaluates the first rule it finds.
#/# We call the target of this first rule 'default' and have it evaluate 'all', which is provided by ct-build.
#/# 'all' builds all MAIN_BUILDTARGETS; if the environment variable 'ENABLE_TESTS' ist set to 'T', 'all' also
#/# builds TEST_BUILDTARGETS.
.PHONY: default
default: all

#######################################################################################################################
# Overridable project configuration

#/# Describes where relative to this make file the project structure is located.
#/# Required by ct-build.
PROJECT_ROOT ?= .

#/# The PTXdist name of this package in upper case.
#/# Required by ct-build.
PTXDIST_PACKAGE ?= EEPROM_XSECTION
#/# The path of the PTXdist make file.
#/# PTXDIST_PACKAGE_MK_FILE is not required by ct-build, it is included and some of the resolved content is printed
#/# by build target 'info-txt'. Note that PTXDIST_WORKSPACE is not layer-aware. Using this variable will only work
#/# when project resides in top-level layer. See https://www.ptxdist.org/doc/dev_layers_in_ptxdist.html for details.
PTXDIST_PACKAGE_MK_FILE ?= $(PTXDIST_WORKSPACE)/rules/wago-tools-012-eeprom-xsection.make

#/# SRC_DIR is not used by ct-build, however the resolved content is printed by build target 'info-txt'.
SRC_DIR = $(PROJECT_ROOT)/src

#######################################################################################################################
# Optional package/ platform feature configuration

#/# Include common defines and macros for use in this make file.
-include $(SCRIPT_DIR)/platform_1.mk

#######################################################################################################################
# Custom toolchain configuration

#/# LINT_RULE_FILES is used to configure which rule set lint uses for checking the source code.
#/# Required by ct-build.
LINT_RULE_FILES = \
	$(LINT_CONFIG_DIR)/lint-rules/pfc.lnt

#######################################################################################################################
# Build target configuration

#/# MAIN_BUILDTARGETS defines which artefacts are subject to be built upon a request for target 'all'.
#/# Required by ct-build.
#/# Shared libraries' names always have to start with 'lib' as required by the gcc linker.
#/# This requirement also applies to static libraries (.a).
MAIN_BUILDTARGETS += \
	eeprom-xsection.elf \

TEST_BUILDTARGETS +=

#/# Required by ct-build.
BUILDTARGETS += \
	$(MAIN_BUILDTARGETS) \
	$(TEST_BUILDTARGETS)

#/# Define a list of those artifacts that will be installed when the build target 'install' is requested (which will
#/# also be done by PTXdist).
#/# Although it is possible to define the rule to install these targets in the PTXdist makefile, it is recommended
#/# to define them in this Makefile, as according to GNU standard a makefile should have an install target.
#/# Required by ct-build.
#/# DESTDIR: https://www.gnu.org/prep/standards/html_node/DESTDIR.html
#/# PTXdist will call 'install' with DESTDIR set to 'ptxproj.<XXX>/platform-<XXX>/packages'
INSTALL_TARGETS += \
	$(DESTDIR)/usr/sbin/eeprom-xsection

#######################################################################################################################
# Settings shared between build targets

#/# The variables in this section are used only within this make file.
SHARED_CCXXFLAGS += -fPIC
#/# Hide all symbols of the referenced object files from the shared object's API, except for those marked for export.
#/# https://gcc.gnu.org/wiki/Visibility and https://www.akkadia.org/drepper/dsohowto.pdf
SHARED_CCXXFLAGS += -fvisibility=hidden
SHARED_CFLAGS    += $(call option_std,gnu99)
SHARED_CXXFLAGS  += $(call option_std,gnu++14)
SHARED_CXXFLAGS  += -fvisibility-inlines-hidden


#/# Find the names of those artefacts in a dependency list which are generated by this makefile.
# $(call local_prerequisites,name)
local_prerequisites = $(call lib_buildtarget_raw,$($(1)_LIBS) $($(1)_PKG_CONFIG_LIBS),$($(1)_STATICALLYLINKED))


#######################################################################################################################

eeprom-xsection.elf_LIBS             +=
eeprom-xsection.elf_STATICALLYLINKED +=
eeprom-xsection.elf_PKG_CONFIGS      +=
eeprom-xsection.elf_DISABLEDWARNINGS +=
eeprom-xsection.elf_PREREQUISITES    += $(call local_prerequisites,eeprom-xsection.elf)
eeprom-xsection.elf_SOURCES          += $(call glob_r,$(addprefix $(PROJECT_ROOT)/src/**/*.,$(SOURCE_FILE_EXTENSIONS)))
#/# Add name of the folder containing the API headers to the include search path.
eeprom-xsection.elf_CPPFLAGS         += -I$(SRC_DIR)
eeprom-xsection.elf_CPPFLAGS         += $(call pkg_config_cppflags,$(eeprom-xsection.elf_PKG_CONFIGS))
eeprom-xsection.elf_CCXXFLAGS        += $(SHARED_CCXXFLAGS)
eeprom-xsection.elf_CCXXFLAGS        += $(call option_disable_warning,$(eeprom-xsection.elf_DISABLEDWARNINGS))
eeprom-xsection.elf_CFLAGS           += $(SHARED_CFLAGS)
eeprom-xsection.elf_CFLAGS           += $(eeprom-xsection.elf_CCXXFLAGS)
eeprom-xsection.elf_CFLAGS           += $(call pkg_config_cflags,$(eeprom-xsection.elf_PKG_CONFIGS))
eeprom-xsection.elf_CXXFLAGS         += $(SHARED_CXXFLAGS)
eeprom-xsection.elf_CXXFLAGS         += $(eeprom-xsection.elf_CCXXFLAGS)
eeprom-xsection.elf_CXXFLAGS         += $(call pkg_config_cxxflags,$(eeprom-xsection.elf_PKG_CONFIGS))
eeprom-xsection.elf_LDFLAGS          += $(call option_lib,$(eeprom-xsection.elf_LIBS),eeprom-xsection.elf)
eeprom-xsection.elf_LDFLAGS          += $(call pkg_config_ldflags,$(eeprom-xsection.elf_PKG_CONFIGS))


#######################################################################################################################
# Build infrastructure

#/# Include variables and rules on how to build a project for the C language.
include $(SCRIPT_DIR)/buildclang_1.mk
#/# Include support for checking all executables with valgrind, on-target.
#/# Note that PTXDIST_WORKSPACE is not layer-aware. Using this variable will only work when project resides in
#/# top-level layer. See https://www.ptxdist.org/doc/dev_layers_in_ptxdist.html for details.
-include $(PTXDIST_WORKSPACE)/../wago_intern/debugging/scripts/valgrind-target.mk


#######################################################################################################################
# Custom rules

#/# This section can be used to define specific make rules, as opposed to the rest of this file,
#/# where only configuration takes place.
#/# One example for specific rules is how to install header or packageconfig files.
#/#

#/# Install binaries.
$(DESTDIR)/usr/sbin/%: $(BIN_DIR)/%.elf | $(DESTDIR)/usr/sbin
	cp $< $@

#######################################################################################################################
# Bootstrapping

#/# This rule complains if the folder pointed to by SCRIPT_DIR does not contain any .mk files.
$(SCRIPT_DIR)/%.mk:
	$(error build scripts unavailable ($@) - set script dir (SCRIPT_DIR=$(SCRIPT_DIR)))

