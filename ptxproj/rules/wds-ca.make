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
PACKAGES-$(PTXCONF_WDS_CA) += wds-ca

#
# Paths and names
#
WDS_CA_VERSION        := 0.1.0
WDS_CA_MD5            :=
WDS_CA                := wds-ca
WDS_CA_BUILDCONFIG    := Release
WDS_CA_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WDS_CA))
WDS_CA_BUILDROOT_DIR  := $(BUILDDIR)/$(WDS_CA)
WDS_CA_DIR            := $(WDS_CA_BUILDROOT_DIR)/src
WDS_CA_BUILD_DIR      := $(WDS_CA_BUILDROOT_DIR)/bin/$(WDS_CA_BUILDCONFIG)
WDS_CA_LICENSE        := WAGO
WDS_CA_CONF_TOOL      := NO
WDS_CA_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WDS_CA_BUILDCONFIG) \
BIN_DIR=$(WDS_CA_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wds-ca.make)

WDS_CA_PACKAGE_NAME             := $(WDS_CA)_$(WDS_CA_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WDS_CA_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wds-ca.extract: 
	@$(call targetinfo)
	@mkdir -p $(WDS_CA_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WDS_CA_DIR) ]; then \
	  ln -s $(WDS_CA_SRC_DIR) $(WDS_CA_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wds-ca.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WDS_CA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wds-ca.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WDS_CA)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wds-ca.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(WDS_CA_PKGDIR) && \
	  tar xvzf $(WDS_CA_PLATFORMCONFIGPACKAGEDIR)/$(WDS_CA_PACKAGE_NAME).tgz -C $(WDS_CA_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, WDS_CA)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WDS_CA_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WDS_CA_PKGDIR) && tar cvzf $(WDS_CA_PLATFORMCONFIGPACKAGEDIR)/$(WDS_CA_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wds-ca.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wds-ca)
	@$(call install_fixup, wds-ca,PRIORITY,optional)
	@$(call install_fixup, wds-ca,SECTION,base)
	@$(call install_fixup, wds-ca,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, wds-ca,DESCRIPTION,missing)

	@$(call install_copy, wds-ca, 0, 0, 0700, - , /usr/bin/wds-ca)

	@$(call install_copy, wds-ca, 0, 0, 0750, $(PTXDIST_SYSROOT_TARGET)/etc/config-tools/wds-ca-config, /etc/config-tools/wds-ca)

	@$(call install_alternative, wds-ca, 0, 0, 0755, /etc/init.d/wds-ca)
	@$(call install_link, wds-ca, /etc/init.d/wds-ca, /etc/rc.d/S99_wds-ca)

ifdef PTXCONF_CT_SETTINGS_BACKUP
	@$(call install_alternative, wds-ca, 0, 0, 0750, /etc/config-tools/backup-restore/backup_wds)
endif

	@$(call install_finish, wds-ca)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wds-ca.clean:
	@$(call targetinfo)
	@if [ -d $(WDS_CA_DIR) ]; then \
		$(WDS_CA_MAKE_ENV) $(WDS_CA_PATH) $(MAKE) $(MFLAGS) -C $(WDS_CA_DIR) clean; \
	fi
	@$(call clean_pkg, WDS_CA)
	@rm -rf $(WDS_CA_BUILDROOT_DIR)

# vim: syntax=make
