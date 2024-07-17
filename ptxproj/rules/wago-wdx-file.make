#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-wdx-file).
#
# Copyright (c) 2021-2022 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-wdx-file.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_WDX_FILE) += wago-wdx-file

#
# Paths and names
#
WAGO_WDX_FILE_VERSION        := 2.1.5
WAGO_WDX_FILE_MD5            :=
WAGO_WDX_FILE_BASE           := wdx-file
WAGO_WDX_FILE                := wago-$(WAGO_WDX_FILE_BASE)-$(WAGO_WDX_FILE_VERSION)
WAGO_WDX_FILE_BUILDCONFIG    := Release
WAGO_WDX_FILE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(WAGO_WDX_FILE_BASE)
WAGO_WDX_FILE_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_WDX_FILE)
WAGO_WDX_FILE_DIR            := $(WAGO_WDX_FILE_BUILDROOT_DIR)/src
WAGO_WDX_FILE_BUILD_DIR      := $(WAGO_WDX_FILE_BUILDROOT_DIR)/bin/$(WAGO_WDX_FILE_BUILDCONFIG)
WAGO_WDX_FILE_LICENSE        := WAGO
WAGO_WDX_FILE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_WDX_FILE_BUILDCONFIG) \
BIN_DIR=$(WAGO_WDX_FILE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

WAGO_WDX_FILE_PACKAGE_NAME             := $(WAGO_WDX_FILE_BASE)_$(WAGO_WDX_FILE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_WDX_FILE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-wdx-file.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_WDX_FILE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_WDX_FILE_DIR) ]; then \
	  ln -s $(WAGO_WDX_FILE_SRC_DIR) $(WAGO_WDX_FILE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WAGO_WDX_FILE_CONF_TOOL := NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-wdx-file.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-wdx-file.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-wdx-file.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_WDX_FILE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_WDX_FILE_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_WDX_FILE_PKGDIR) && tar cvzf $(WAGO_WDX_FILE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_WDX_FILE_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_WDX_FILE_PKGDIR) && \
	  tar xvzf $(WAGO_WDX_FILE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_WDX_FILE_PACKAGE_NAME).tgz -C $(WAGO_WDX_FILE_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-wdx-file.targetinstall:
	@$(call targetinfo)
	# Nothing to install on the target
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-wdx-file.clean:
	@$(call targetinfo)

	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/wdxfile/
	@if [ -d $(WAGO_WDX_FILE_DIR) ]; then \
	  $(WAGO_WDX_FILE_MAKE_ENV) $(WAGO_WDX_FILE_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_WDX_FILE_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_WDX_FILE)
	@rm -rf $(WAGO_WDX_FILE_BUILDROOT_DIR)

# vim: syntax=make
