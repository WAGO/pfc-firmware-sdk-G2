#######################################################################################################################
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file is part of project auth-service (PTXdist package wago-auth-service).
#
# Copyright (c) 2023 WAGO GmbH & Co. KG
#
# Contributors:
#   PEn: WAGO GmbH & Co. KG
#######################################################################################################################
# Makefile for PTXdist package wago-auth-service.
#
# For further information about the PTXdist project and license conditions
# see the PTXdist README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_AUTH_SERVICE) += wago-auth-service

#
# Paths and names
#
WAGO_AUTH_SERVICE_VERSION        := 1.2.0
WAGO_AUTH_SERVICE_MD5            :=
WAGO_AUTH_SERVICE_BASE           := auth-service
WAGO_AUTH_SERVICE                := wago-$(WAGO_AUTH_SERVICE_BASE)-$(WAGO_AUTH_SERVICE_VERSION)
WAGO_AUTH_SERVICE_BUILDCONFIG    := Release
WAGO_AUTH_SERVICE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(WAGO_AUTH_SERVICE_BASE)
WAGO_AUTH_SERVICE_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_AUTH_SERVICE)
WAGO_AUTH_SERVICE_DIR            := $(WAGO_AUTH_SERVICE_BUILDROOT_DIR)/src
WAGO_AUTH_SERVICE_BUILD_DIR      := $(WAGO_AUTH_SERVICE_BUILDROOT_DIR)/bin/$(WAGO_AUTH_SERVICE_BUILDCONFIG)
WAGO_AUTH_SERVICE_LICENSE        := WAGO
WAGO_AUTH_SERVICE_PATH           := PATH=$(CROSS_PATH)
WAGO_AUTH_SERVICE_BIN            := 
WAGO_AUTH_SERVICE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_AUTH_SERVICE_BUILDCONFIG) \
BIN_DIR=$(WAGO_AUTH_SERVICE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

WAGO_AUTH_SERVICE_PACKAGE_NAME := $(WAGO_AUTH_SERVICE_BASE)_$(WAGO_AUTH_SERVICE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_AUTH_SERVICE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-service.extract:
	@$(call targetinfo)
	@mkdir -p $(WAGO_AUTH_SERVICE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES	
	@if [ ! -L $(WAGO_AUTH_SERVICE_DIR) ]; then \
	  ln -s $(WAGO_AUTH_SERVICE_SRC_DIR) $(WAGO_AUTH_SERVICE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

WAGO_AUTH_SERVICE_CONF_TOOL	:= NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

$(STATEDIR)/wago-auth-service.prepare:
	@$(call targetinfo)
	@$(call touch)

endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/wago-auth-service.compile: export WAGO_AUTH_SERVICE_WDM_DIR=$(PTXCONF_WAGO_AUTH_SERVICE_MODEL_FILES_WDM_DIR)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wago-auth-service.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/wago-auth-service.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, WAGO_AUTH_SERVICE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_AUTH_SERVICE_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(WAGO_AUTH_SERVICE_PKGDIR) && tar cvzf $(WAGO_AUTH_SERVICE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_AUTH_SERVICE_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(WAGO_AUTH_SERVICE_PKGDIR) && \
	  tar xvzf $(WAGO_AUTH_SERVICE_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_AUTH_SERVICE_PACKAGE_NAME).tgz -C $(WAGO_AUTH_SERVICE_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-service.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-auth-service)
	@$(call install_fixup, wago-auth-service,PRIORITY,optional)
	@$(call install_fixup, wago-auth-service,SECTION,base)
	@$(call install_fixup, wago-auth-service,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-auth-service,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_AUTH_SERVICE_LIB
#	@$(call install_lib, wago-auth-service, 0, 0, 0755, libauthserv)
	@$(call install_copy, wago-auth-service, 0, 0, 0644, -, /etc/pam.d/authd)
endif

ifdef PTXCONF_WAGO_AUTH_SERVICE_LIGHTTPD_INTEGRATION
	@$(call install_alternative, wago-auth-service, 0, 0, 0600, /etc/lighttpd/apps.confd/auth_service.conf)
endif

ifdef PTXCONF_WAGO_AUTH_SERVICE_DAEMON
	@$(call install_copy, wago-auth-service, 0,   0, 0750, -, /usr/sbin/authd)
	@$(call install_copy, wago-auth-service, 0, 124, 0770,    /etc/authd)
	@$(call install_copy, wago-auth-service, 0, 124, 0660, -, /etc/authd/authd.conf)

	# Install login web-page
	@$(call install_copy, wago-auth-service, 0, 124, 0775,    /var/www/auth)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/auth.js)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/login.css)
	@$(call install_copy, wago-auth-service, 0, 124, 0660, -, /var/www/auth/login.html.template)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/login.js)
	@$(call install_copy, wago-auth-service, 0, 124, 0775,    /var/www/auth/images)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/favicon.ico)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/spinner.gif)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/wago-logo.svg)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/warning.svg)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/chevron-left.svg)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/chevron-right.svg)
	@$(call install_copy, wago-auth-service, 0, 124, 0664, -, /var/www/auth/images/btn-close.svg)


# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_WAGO_AUTH_SERVICE_DAEMON_STARTSCRIPT
	@$(call install_copy, wago-auth-service, 0, 0, 0755, -, /etc/init.d/authd)

ifneq ($(call remove_quotes, $(PTXCONF_WAGO_AUTH_SERVICE_DAEMON_BBINIT_LINK)),)
	@$(call install_link, wago-auth-service, \
	  /etc/init.d/authd, \
	  /etc/rc.d/$(PTXCONF_WAGO_AUTH_SERVICE_DAEMON_BBINIT_LINK))
endif
endif # PTXCONF_WAGO_AUTH_SERVICE_DAEMON_STARTSCRIPT
endif # PTXCONF_INITMETHOD_BBINIT
endif # PTXCONF_WAGO_AUTH_SERVICE_DAEMON

	@$(call install_finish, wago-auth-service)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-auth-service.clean:
	@$(call targetinfo)

	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/wago/auth/
	@if [ -d $(WAGO_AUTH_SERVICE_DIR) ]; then \
	  $(WAGO_AUTH_SERVICE_MAKE_ENV) $(WAGO_AUTH_SERVICE_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_AUTH_SERVICE_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_AUTH_SERVICE)
	@rm -rf $(WAGO_AUTH_SERVICE_BUILDROOT_DIR)

# vim: syntax=make
