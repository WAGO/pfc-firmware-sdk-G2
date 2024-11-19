#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-parameter-service-com).
#
# Copyright (c) 2021-2022 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-parameter-service-com.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_PARAMETER_SERVICE_COM) += wago-parameter-service-com

#
# Paths and names
#
WAGO_PARAMETER_SERVICE_COM_VERSION        := 1.1.6
WAGO_PARAMETER_SERVICE_COM_MD5            :=
WAGO_PARAMETER_SERVICE_COM_BASE           := parameter-service-com
WAGO_PARAMETER_SERVICE_COM                := wago-$(WAGO_PARAMETER_SERVICE_COM_BASE)-$(WAGO_PARAMETER_SERVICE_COM_VERSION)
WAGO_PARAMETER_SERVICE_COM_BUILDCONFIG    := Release
WAGO_PARAMETER_SERVICE_COM_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WAGO_PARAMETER_SERVICE_COM_BASE))
WAGO_PARAMETER_SERVICE_COM_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE_COM)
WAGO_PARAMETER_SERVICE_COM_DIR            := $(WAGO_PARAMETER_SERVICE_COM_BUILDROOT_DIR)/src
WAGO_PARAMETER_SERVICE_COM_BUILD_DIR      := $(WAGO_PARAMETER_SERVICE_COM_BUILDROOT_DIR)/bin/$(WAGO_PARAMETER_SERVICE_COM_BUILDCONFIG)
WAGO_PARAMETER_SERVICE_COM_LICENSE        := WAGO
WAGO_PARAMETER_SERVICE_COM_PATH           := PATH=$(CROSS_PATH)
WAGO_PARAMETER_SERVICE_COM_BIN            := 
WAGO_PARAMETER_SERVICE_COM_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_PARAMETER_SERVICE_COM_BUILDCONFIG) \
BIN_DIR=$(WAGO_PARAMETER_SERVICE_COM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wago-parameter-service-com.make)

WAGO_PARAMETER_SERVICE_COM_PACKAGE_NAME := $(WAGO_PARAMETER_SERVICE_COM_BASE)_$(WAGO_PARAMETER_SERVICE_COM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PARAMETER_SERVICE_COM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-com.extract:
	@$(call targetinfo)
	@mkdir -p $(WAGO_PARAMETER_SERVICE_COM_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_PARAMETER_SERVICE_COM_DIR) ]; then \
	  ln -s $(WAGO_PARAMETER_SERVICE_COM_SRC_DIR) $(WAGO_PARAMETER_SERVICE_COM_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WAGO_PARAMETER_SERVICE_COM_CONF_TOOL := NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-parameter-service-com.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-parameter-service-com.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-com.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_PARAMETER_SERVICE_COM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_PARAMETER_SERVICE_COM_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_PARAMETER_SERVICE_COM_PKGDIR) && tar cvzf $(WAGO_PARAMETER_SERVICE_COM_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_COM_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_PARAMETER_SERVICE_COM_PKGDIR) && \
	  tar xvzf $(WAGO_PARAMETER_SERVICE_COM_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_COM_PACKAGE_NAME).tgz -C $(WAGO_PARAMETER_SERVICE_COM_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-com.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-parameter-service-com)
	@$(call install_fixup, wago-parameter-service-com,PRIORITY,optional)
	@$(call install_fixup, wago-parameter-service-com,SECTION,base)
	@$(call install_fixup, wago-parameter-service-com,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-parameter-service-com,DESCRIPTION,missing)

# only install .so if checked
ifdef PTXCONF_WAGO_PARAMETER_SERVICE_COM_LIB
	@$(call install_lib, wago-parameter-service-com, 0, 0, 0644, libparamcom)
endif

	@$(call install_finish, wago-parameter-service-com)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-com.clean:
	@$(call targetinfo)

	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/parambase/
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/paramtest/
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/paramcom/
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/paramclient/
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/parampriv/
	@if [ -d $(WAGO_PARAMETER_SERVICE_COM_DIR) ]; then \
	  $(WAGO_PARAMETER_SERVICE_COM_MAKE_ENV) $(WAGO_PARAMETER_SERVICE_COM_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_PARAMETER_SERVICE_COM_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_PARAMETER_SERVICE_COM)
	@rm -rf $(WAGO_PARAMETER_SERVICE_COM_BUILDROOT_DIR)

# vim: syntax=make
