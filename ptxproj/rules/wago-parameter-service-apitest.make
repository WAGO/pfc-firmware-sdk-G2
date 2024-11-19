#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-parameter-service-apitest).
#
# Copyright (c) 2021-2023 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-parameter-service-apitest.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
LAZY_PACKAGES-$(PTXCONF_WAGO_PARAMETER_SERVICE_APITEST) += wago-parameter-service-apitest

#
# Paths and names
#
WAGO_PARAMETER_SERVICE_APITEST_VERSION        := 0.12.0
WAGO_PARAMETER_SERVICE_APITEST_MD5            :=
WAGO_PARAMETER_SERVICE_APITEST_BASE           := parameter-service-apitest
WAGO_PARAMETER_SERVICE_APITEST                := wago-$(WAGO_PARAMETER_SERVICE_APITEST_BASE)-$(WAGO_PARAMETER_SERVICE_APITEST_VERSION)
WAGO_PARAMETER_SERVICE_APITEST_BUILDCONFIG    := Release
WAGO_PARAMETER_SERVICE_APITEST_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(WAGO_PARAMETER_SERVICE_APITEST_BASE)
WAGO_PARAMETER_SERVICE_APITEST_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE_APITEST)
WAGO_PARAMETER_SERVICE_APITEST_DIR            := $(WAGO_PARAMETER_SERVICE_APITEST_BUILDROOT_DIR)/src
WAGO_PARAMETER_SERVICE_APITEST_BUILD_DIR      := $(WAGO_PARAMETER_SERVICE_APITEST_BUILDROOT_DIR)/bin/$(WAGO_PARAMETER_SERVICE_APITEST_BUILDCONFIG)
WAGO_PARAMETER_SERVICE_APITEST_LICENSE        := WAGO
WAGO_PARAMETER_SERVICE_APITEST_CONF_TOOL      := NO
WAGO_PARAMETER_SERVICE_APITEST_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_PARAMETER_SERVICE_APITEST_BUILDCONFIG) \
BIN_DIR=$(WAGO_PARAMETER_SERVICE_APITEST_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

WAGO_PARAMETER_SERVICE_APITEST_PACKAGE_NAME             := $(WAGO_PARAMETER_SERVICE_APITEST_BASE)_$(WAGO_PARAMETER_SERVICE_APITEST_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PARAMETER_SERVICE_APITEST_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-parameter-service-apitest.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_PARAMETER_SERVICE_APITEST_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_PARAMETER_SERVICE_APITEST_DIR) ]; then \
	  ln -s $(WAGO_PARAMETER_SERVICE_APITEST_SRC_DIR) $(WAGO_PARAMETER_SERVICE_APITEST_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-apitest.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WAGO_PARAMETER_SERVICE_APITEST)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-apitest.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WAGO_PARAMETER_SERVICE_APITEST)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-apitest.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_PARAMETER_SERVICE_APITEST)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_PARAMETER_SERVICE_APITEST_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_PARAMETER_SERVICE_APITEST_PKGDIR) && tar cvzf $(WAGO_PARAMETER_SERVICE_APITEST_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_APITEST_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_PARAMETER_SERVICE_APITEST_PKGDIR) && \
	  tar xvzf $(WAGO_PARAMETER_SERVICE_APITEST_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_APITEST_PACKAGE_NAME).tgz -C $(WAGO_PARAMETER_SERVICE_APITEST_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-apitest.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-parameter-service-apitest)
	@$(call install_fixup, wago-parameter-service-apitest,PRIORITY,optional)
	@$(call install_fixup, wago-parameter-service-apitest,SECTION,base)
	@$(call install_fixup, wago-parameter-service-apitest,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-parameter-service-apitest,DESCRIPTION,missing)

	@$(call install_copy, wago-parameter-service-apitest, 0, 0, 0755, -, /usr/sbin/apitest_client)
	@$(call install_copy, wago-parameter-service-apitest, 0, 0, 0700, -, /etc/init.d/apitest_client)

	@$(call install_finish, wago-parameter-service-apitest)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service-apitest.clean:
	@$(call targetinfo)
	@if [ -d $(WAGO_PARAMETER_SERVICE_APITEST_DIR) ]; then \
		$(WAGO_PARAMETER_SERVICE_APITEST_MAKE_ENV) $(WAGO_PARAMETER_SERVICE_APITEST_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_PARAMETER_SERVICE_APITEST_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_PARAMETER_SERVICE_APITEST)
	@rm -rf $(WAGO_PARAMETER_SERVICE_APITEST_BUILDROOT_DIR)

# vim: syntax=make
