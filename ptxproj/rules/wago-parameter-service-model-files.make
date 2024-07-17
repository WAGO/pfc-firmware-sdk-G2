#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service-model-files (PTXdist package wago-parameter-service-model-files).
#
# Copyright (c) 2021 WAGO GmbH & Co. KG
#
# Contributors:
#   MaHe: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-parameter-service-model-files.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES) += wago-parameter-service-model-files

#
# Paths and names
#
WAGO_PARAMETER_SERVICE_MODEL_FILES_VERSION        := 1.7.0
WAGO_PARAMETER_SERVICE_MODEL_FILES_MD5            :=
WAGO_PARAMETER_SERVICE_MODEL_FILES_BASE           := parameter-service-model-files
WAGO_PARAMETER_SERVICE_MODEL_FILES                := wago-$(WAGO_PARAMETER_SERVICE_MODEL_FILES_BASE)-$(WAGO_PARAMETER_SERVICE_MODEL_FILES_VERSION)
WAGO_PARAMETER_SERVICE_MODEL_FILES_SUFFIX         := tar.gz
WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDCONFIG    := Release
WAGO_PARAMETER_SERVICE_MODEL_FILES_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_BASE))
WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES)
WAGO_PARAMETER_SERVICE_MODEL_FILES_DIR            := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/src
WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILD_DIR      := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/bin
WAGO_PARAMETER_SERVICE_MODEL_FILES_INCLUDE_DIR    := /usr/include
WAGO_PARAMETER_SERVICE_MODEL_FILES_LICENSE        := WAGO
WAGO_PARAMETER_SERVICE_MODEL_FILES_PATH           := PATH=$(CROSS_PATH)
WAGO_PARAMETER_SERVICE_MODEL_FILES_BIN            :=
WAGO_PARAMETER_SERVICE_MODEL_FILES_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDCONFIG) \
BIN_DIR=$(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILD_DIR) \

WAGO_PARAMETER_SERVICE_MODEL_FILES_SCRIPT_DIR       := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_SRC_DIR)/script
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_TEMPLATE_DIR := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_SRC_DIR)/wdd/template/$(PTXCONF_PLATFORM)
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_FRAGMENT_DIR := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_SRC_DIR)/wdd/fragment
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_OUTPUT_DIR   := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/wdd

WAGO_PARAMETER_SERVICE_MODEL_FILES_PACKAGE_NAME := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BASE)_$(WAGO_PARAMETER_SERVICE_MODEL_FILES_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PARAMETER_SERVICE_MODEL_FILES_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# WDx artifacts
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_VERSION        := 1.3.10
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX                := wago-parameter-service-model-files-wdx-$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_VERSION)
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_DIR            := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX)
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_URL            := $(call jfrog_template_to_url, WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX)
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SUFFIX         := $(suffix $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_URL))
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_MD5             = $(shell [ -f $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_MD5_FILE) ] && cat $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_MD5_FILE))
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SOURCE_BASEDIR := wago_intern/artifactory_sources
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_MD5_FILE       := $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SOURCE_BASEDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX)$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SUFFIX).md5
WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_ARTIFACT        = $(call jfrog_get_filename,$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_URL))

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-parameter-service-model-files.get:
	# download WDx files from artifactory
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
	  '$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_URL)' $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SOURCE_BASEDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX)$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SUFFIX) '$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_MD5_FILE)'
	@$(call touch)

else
$(STATEDIR)/wago-parameter-service-model-files.get:
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-model-files.extract:
	@$(call targetinfo)
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# link local wdx source directory
	@if [ ! -L $(WAGO_PARAMETER_SERVICE_MODEL_FILES_DIR) ]; then \
	  ln -s $(WAGO_PARAMETER_SERVICE_MODEL_FILES_SRC_DIR) $(WAGO_PARAMETER_SERVICE_MODEL_FILES_DIR); \
	fi
	# extract downloaded artifacts
	@unzip -o $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SOURCE_BASEDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX)$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_SUFFIX) \
	  -d $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_DIR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WAGO_PARAMETER_SERVICE_MODEL_FILES_CONF_TOOL	:= NO

$(STATEDIR)/wago-parameter-service-model-files.prepare:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-model-files.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR) && \
	  cp -r $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_DIR)/include $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/include
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/wdm && \
	  cp $(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_DIR)/WAGO.bundle.wdm.json $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/wdm/
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/wdd && \
	  $(HOSTPYTHON3) $(WAGO_PARAMETER_SERVICE_MODEL_FILES_SCRIPT_DIR)/generate_wdd.py -v -p \
	    -t "$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_TEMPLATE_DIR)" \
	    -f "$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_FRAGMENT_DIR)" \
	    -o "$(WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_OUTPUT_DIR)"
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-model-files.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: install model files into target directories
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)/$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR) && \
	  cp $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/wdm/*.wdm.json $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR)/ && \
	  chmod 644 $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR)/*.wdm.json

	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)/$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_DIR) && \
	  cp $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/wdd/*.wdd.json $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_DIR)/ && \
	  chmod 644 $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_DIR)/*.wdd.json

ifdef PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDX_CPP_HEADERS
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_INCLUDE_DIR) && \
	  cp -r $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)/include/* $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)$(WAGO_PARAMETER_SERVICE_MODEL_FILES_INCLUDE_DIR)/
endif

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
# avoid to install header files in BSP mode: for internal use only
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR) && tar cvzf $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_PACKAGE_NAME).tgz --exclude=usr/include/* *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR) && \
	  tar xvzf $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_MODEL_FILES_PACKAGE_NAME).tgz -C $(WAGO_PARAMETER_SERVICE_MODEL_FILES_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-model-files.targetinstall:
	@$(call targetinfo)
	@$(call install_init, wago-parameter-service-model-files)
	@$(call install_fixup, wago-parameter-service-model-files,PRIORITY,optional)
	@$(call install_fixup, wago-parameter-service-model-files,SECTION,base)
	@$(call install_fixup, wago-parameter-service-model-files,AUTHOR,"MaHe - WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-parameter-service-model-files,DESCRIPTION,missing)

	# copy WDM and WDDs to target directories
	@$(call install_copy, wago-parameter-service-model-files, 0, 0, 0755, $(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR))
	@$(call install_copy, wago-parameter-service-model-files, 0, 0, 0755, $(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_DIR))
	@$(call install_glob, wago-parameter-service-model-files, 0, 0, -,    $(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR), *.wdm.json)
	@$(call install_glob, wago-parameter-service-model-files, 0, 0, -,    $(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_DIR), *.wdd.json)

	@$(call install_finish, wago-parameter-service-model-files)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-model-files.clean:
	@$(call targetinfo)
	rm -rf $(PTXCONF_SYSROOT_TARGET)$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR)
	rm -rf $(PTXCONF_SYSROOT_TARGET)$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDD_DIR)
	@$(call clean_pkg, WAGO_PARAMETER_SERVICE_MODEL_FILES)
	@rm -rf $(WAGO_PARAMETER_SERVICE_MODEL_FILES_BUILDROOT_DIR)

# vim: syntax=make
