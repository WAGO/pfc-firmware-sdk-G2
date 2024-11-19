#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project parameter-service (PTXdist package wago-parameter-service).
#
# Copyright (c) 2019-2022 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-parameter-service.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_PARAMETER_SERVICE) += wago-parameter-service

#
# Paths and names
#
WAGO_PARAMETER_SERVICE_VERSION        := 1.5.0
WAGO_PARAMETER_SERVICE_MD5            :=
WAGO_PARAMETER_SERVICE_BASE           := parameter-service
WAGO_PARAMETER_SERVICE                := wago-$(WAGO_PARAMETER_SERVICE_BASE)-$(WAGO_PARAMETER_SERVICE_VERSION)
WAGO_PARAMETER_SERVICE_BUILDCONFIG    := Release
WAGO_PARAMETER_SERVICE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(WAGO_PARAMETER_SERVICE_BASE)
WAGO_PARAMETER_SERVICE_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_PARAMETER_SERVICE)
WAGO_PARAMETER_SERVICE_DIR            := $(WAGO_PARAMETER_SERVICE_BUILDROOT_DIR)/src
WAGO_PARAMETER_SERVICE_BUILD_DIR      := $(WAGO_PARAMETER_SERVICE_BUILDROOT_DIR)/bin/$(WAGO_PARAMETER_SERVICE_BUILDCONFIG)
WAGO_PARAMETER_SERVICE_LICENSE        := WAGO
WAGO_PARAMETER_SERVICE_PATH           := PATH=$(CROSS_PATH)
WAGO_PARAMETER_SERVICE_BIN            := 
WAGO_PARAMETER_SERVICE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_PARAMETER_SERVICE_BUILDCONFIG) \
BIN_DIR=$(WAGO_PARAMETER_SERVICE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

WAGO_PARAMETER_SERVICE_PACKAGE_NAME := $(WAGO_PARAMETER_SERVICE_BASE)_$(WAGO_PARAMETER_SERVICE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PARAMETER_SERVICE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service.extract:
	@$(call targetinfo)
	@mkdir -p $(WAGO_PARAMETER_SERVICE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES	
	@if [ ! -L $(WAGO_PARAMETER_SERVICE_DIR) ]; then \
	  ln -s $(WAGO_PARAMETER_SERVICE_SRC_DIR) $(WAGO_PARAMETER_SERVICE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WAGO_PARAMETER_SERVICE_CONF_TOOL	:= NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

$(STATEDIR)/wago-parameter-service.prepare:
	@$(call targetinfo)
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/wago-parameter-service.compile: export WAGO_PARAMETER_SERVICE_WDM_DIR=$(PTXCONF_WAGO_PARAMETER_SERVICE_MODEL_FILES_WDM_DIR)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-parameter-service.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/wago-parameter-service.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_PARAMETER_SERVICE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_PARAMETER_SERVICE_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_PARAMETER_SERVICE_PKGDIR) && tar cvzf $(WAGO_PARAMETER_SERVICE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_PARAMETER_SERVICE_PKGDIR) && \
	  tar xvzf $(WAGO_PARAMETER_SERVICE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PARAMETER_SERVICE_PACKAGE_NAME).tgz -C $(WAGO_PARAMETER_SERVICE_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-parameter-service)
	@$(call install_fixup, wago-parameter-service,PRIORITY,optional)
	@$(call install_fixup, wago-parameter-service,SECTION,base)
	@$(call install_fixup, wago-parameter-service,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-parameter-service,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_PARAMETER_SERVICE_LIB
#	@$(call install_lib, wago-parameter-service, 0, 0, 0755, libparamserv)
endif

ifdef PTXCONF_WAGO_PARAMETER_SERVICE_LIGHTTPD_INTEGRATION
	@$(call install_alternative, wago-parameter-service, 0, 0, 0600, /etc/lighttpd/apps.confd/param_service.conf)
endif

ifdef PTXCONF_WAGO_PARAMETER_SERVICE_DAEMON
	@$(call install_copy, wago-parameter-service, 0,   0, 0750, -, /usr/sbin/paramd)
	@$(call install_copy, wago-parameter-service, 0, 121, 0770,    /etc/paramd)
	@$(call install_copy, wago-parameter-service, 0, 121, 0660, -, /etc/paramd/paramd.conf)
	@$(call install_copy, wago-parameter-service, 0, 124, 0770,    /etc/authd)
	@$(call install_copy, wago-parameter-service, 0, 124, 0770,    /etc/authd/clients)
	@$(call install_copy, wago-parameter-service, 0, 124, 0770,    /etc/authd/resource_servers)
	@$(call install_copy, wago-parameter-service, 0, 124, 0660, -, /etc/authd/clients/paramd.conf)
	@$(call install_copy, wago-parameter-service, 0, 124, 0660, -, /etc/authd/resource_servers/wda.conf)
	@$(call install_copy, wago-parameter-service, 0,   0, 0750, -, /etc/config-tools/get_wda)
	@$(call install_copy, wago-parameter-service, 0,   0, 0750, -, /etc/config-tools/config_wda)
	@$(call install_copy, wago-parameter-service, 0,   0, 0750, -, /etc/config-tools/backup-restore/parameter_service)

# Install sudoers.d file
	@$(call install_copy, wago-parameter-service, 0,   0, 0444, -, /etc/sudoers.d/config_wda)

# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_WAGO_PARAMETER_SERVICE_DAEMON_STARTSCRIPT
	@$(call install_copy, wago-parameter-service, 0, 0, 0755, -, /etc/init.d/paramd)

ifneq ($(call remove_quotes, $(PTXCONF_WAGO_PARAMETER_SERVICE_DAEMON_BBINIT_LINK)),)
	@$(call install_link, wago-parameter-service, \
	  /etc/init.d/paramd, \
	  /etc/rc.d/$(PTXCONF_WAGO_PARAMETER_SERVICE_DAEMON_BBINIT_LINK))
endif
endif # PTXCONF_WAGO_PARAMETER_SERVICE_DAEMON_STARTSCRIPT
endif # PTXCONF_INITMETHOD_BBINIT
endif # PTXCONF_WAGO_PARAMETER_SERVICE_DAEMON

	@$(call install_finish, wago-parameter-service)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-parameter-service.clean:
	@$(call targetinfo)

	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/wago/paramserv/
	@if [ -d $(WAGO_PARAMETER_SERVICE_DIR) ]; then \
	  $(WAGO_PARAMETER_SERVICE_MAKE_ENV) $(WAGO_PARAMETER_SERVICE_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_PARAMETER_SERVICE_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_PARAMETER_SERVICE)
	@rm -rf $(WAGO_PARAMETER_SERVICE_BUILDROOT_DIR)

# vim: syntax=make
