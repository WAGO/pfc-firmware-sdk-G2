#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-parameter-service).
#
# Copyright (c) 2021 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-trace.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_TRACE) += wago-trace

#
# Paths and names
#
WAGO_TRACE_VERSION        := 0.1.0
WAGO_TRACE_MD5            :=
WAGO_TRACE_BASE           := wago-trace
WAGO_TRACE                := $(WAGO_TRACE_BASE)-$(WAGO_TRACE_VERSION)
WAGO_TRACE_URL            := file://wago_intern/$(WAGO_TRACE_BASE)
WAGO_TRACE_BUILDCONFIG    := Release
WAGO_TRACE_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WAGO_TRACE_BASE))
WAGO_TRACE_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_TRACE)
WAGO_TRACE_DIR            := $(WAGO_TRACE_BUILDROOT_DIR)/src
WAGO_TRACE_BUILD_DIR      := $(WAGO_TRACE_BUILDROOT_DIR)/bin/$(WAGO_TRACE_BUILDCONFIG)
WAGO_TRACE_LICENSE        := WAGO
WAGO_TRACE_CONF_TOOL      := NO
WAGO_TRACE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_TRACE_BUILDCONFIG) \
BIN_DIR=$(WAGO_TRACE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wago-trace.make)

WAGO_TRACE_PACKAGE_NAME             := $(WAGO_TRACE_BASE)_$(WAGO_TRACE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_TRACE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-trace.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_TRACE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_TRACE_DIR) ]; then \
	  ln -s $(WAGO_TRACE_SRC_DIR) $(WAGO_TRACE_DIR); \
	fi
endif
	@$(call touch)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-trace.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-trace.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-trace.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-trace.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_TRACE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_TRACE_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_TRACE_PKGDIR) && tar cvzf $(WAGO_TRACE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_TRACE_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_TRACE_PKGDIR) && \
	  tar xvzf $(WAGO_TRACE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_TRACE_PACKAGE_NAME).tgz -C $(WAGO_TRACE_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-trace.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-trace)
	@$(call install_fixup, wago-trace,PRIORITY,optional)
	@$(call install_fixup, wago-trace,SECTION,base)
	@$(call install_fixup, wago-trace,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-trace,DESCRIPTION,missing)

	@$(call install_lib, wago-trace, 0, 0, 0644, libwtrace)

	@$(call install_finish, wago-trace)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-trace.clean:
	@$(call targetinfo)

	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/wtrace/
	@if [ -d $(WAGO_TRACE_DIR) ]; then \
		$(WAGO_TRACE_MAKE_ENV) $(WAGO_TRACE_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_TRACE_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_TRACE)
	@rm -rf $(WAGO_TRACE_BUILDROOT_DIR)

# vim: syntax=make
