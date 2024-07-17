# -*-makefile-*-
#
# Copyright (C) 2024 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WDJ_CA) += wdj-ca

#
# Paths and names
#
WDJ_CA_VERSION        := 0.1.0
WDJ_CA_MD5            :=
WDJ_CA                := wdj-ca
WDJ_CA_BUILDCONFIG    := Release
WDJ_CA_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WDJ_CA))
WDJ_CA_BUILDROOT_DIR  := $(BUILDDIR)/$(WDJ_CA)
WDJ_CA_DIR            := $(WDJ_CA_BUILDROOT_DIR)/src
WDJ_CA_BUILD_DIR      := $(WDJ_CA_BUILDROOT_DIR)/bin/$(WDJ_CA_BUILDCONFIG)
WDJ_CA_LICENSE        := WAGO
WDJ_CA_CONF_TOOL      := NO
WDJ_CA_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WDJ_CA_BUILDCONFIG) \
BIN_DIR=$(WDJ_CA_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wdj-ca.make)

WDJ_CA_PACKAGE_NAME             := $(WDJ_CA)_$(WDJ_CA_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WDJ_CA_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wdj-ca.extract: 
	@$(call targetinfo)
	@mkdir -p $(WDJ_CA_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WDJ_CA_DIR) ]; then \
	  ln -s $(WDJ_CA_SRC_DIR) $(WDJ_CA_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wdj-ca.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WDJ_CA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wdj-ca.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WDJ_CA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wdj-ca.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(WDJ_CA_PKGDIR) && \
	  tar xvzf $(WDJ_CA_PLATFORMCONFIGPACKAGEDIR)/$(WDJ_CA_PACKAGE_NAME).tgz -C $(WDJ_CA_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, WDJ_CA)
	@mkdir -p $(WDJ_CA_PKGDIR)/etc/wdj
	@cp $(WDJ_CA_DIR)/etc/wdj/* $(WDJ_CA_PKGDIR)/etc/wdj/

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WDJ_CA_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WDJ_CA_PKGDIR) && tar cvzf $(WDJ_CA_PLATFORMCONFIGPACKAGEDIR)/$(WDJ_CA_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wdj-ca.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wdj-ca)
	@$(call install_fixup, wdj-ca,PRIORITY,optional)
	@$(call install_fixup, wdj-ca,SECTION,base)
	@$(call install_fixup, wdj-ca,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, wdj-ca,DESCRIPTION,missing)

	@$(call install_copy, wdj-ca, 0, 0, 0700, -, /usr/bin/wdj-ca)
	@$(call install_copy, wdj-ca, 0, 0, 0700, /etc/wdj)
	@$(call install_copy, wdj-ca, 0, 0, 0600, $(WDJ_CA_PKGDIR)/etc/wdj/wdj-ca.state, /etc/wdj/wdj-ca.state)

	@$(call install_copy, wdj-ca, 0, 0, 0750, $(PTXDIST_SYSROOT_TARGET)/etc/config-tools/wdj-ca-config, /etc/config-tools/wdj-ca)
	
	@$(call install_alternative, wdj-ca, 0, 0, 0755, /etc/init.d/wdj-ca)
	@$(call install_link, wdj-ca, /etc/init.d/wdj-ca, /etc/rc.d/S99_wdj-ca)

ifdef PTXCONF_CT_SETTINGS_BACKUP
	@$(call install_alternative, wdj-ca, 0, 0, 0750, /etc/config-tools/backup-restore/backup_wdj)
endif

	@$(call install_finish, wdj-ca)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wdj-ca.clean:
	@$(call targetinfo)
	@if [ -d $(WDJ_CA_DIR) ]; then \
		$(WDJ_CA_MAKE_ENV) $(WDJ_CA_PATH) $(MAKE) $(MFLAGS) -C $(WDJ_CA_DIR) clean; \
	fi
	@$(call clean_pkg, WDJ_CA)
	@rm -rf $(WDJ_CA_BUILDROOT_DIR)

# vim: syntax=make
