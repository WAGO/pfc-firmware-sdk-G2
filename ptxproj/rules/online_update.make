# -*-makefile-*-
#
# Copyright (C) 2023 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ONLINE_UPDATE) += online_update

#
# Paths and names
#
ONLINE_UPDATE_VERSION        := 0.1.0
ONLINE_UPDATE_MD5            :=
ONLINE_UPDATE                := online_update
ONLINE_UPDATE_BUILDCONFIG    := Release
ONLINE_UPDATE_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(ONLINE_UPDATE))
ONLINE_UPDATE_BUILDROOT_DIR  := $(BUILDDIR)/$(ONLINE_UPDATE)
ONLINE_UPDATE_DIR            := $(ONLINE_UPDATE_BUILDROOT_DIR)/src
ONLINE_UPDATE_BUILD_DIR      := $(ONLINE_UPDATE_BUILDROOT_DIR)/bin/$(ONLINE_UPDATE_BUILDCONFIG)
ONLINE_UPDATE_LICENSE        := WAGO
ONLINE_UPDATE_CONF_TOOL      := NO
ONLINE_UPDATE_MAKE_ENV       := $(CROSS_ENV) \
	BUILDCONFIG=$(ONLINE_UPDATE_BUILDCONFIG) \
	BIN_DIR=$(ONLINE_UPDATE_BUILD_DIR) \
	SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
	PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/online_update.make)

ONLINE_UPDATE_PACKAGE_NAME             := $(ONLINE_UPDATE)_$(ONLINE_UPDATE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
ONLINE_UPDATE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/online_update.extract: 
	@$(call targetinfo)
	@mkdir -p $(ONLINE_UPDATE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(ONLINE_UPDATE_DIR) ]; then \
	  ln -s $(ONLINE_UPDATE_SRC_DIR) $(ONLINE_UPDATE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/online_update.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, ONLINE_UPDATE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/online_update.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, ONLINE_UPDATE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/online_update.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(ONLINE_UPDATE_PKGDIR) && \
	  tar xvzf $(ONLINE_UPDATE_PLATFORMCONFIGPACKAGEDIR)/$(ONLINE_UPDATE_PACKAGE_NAME).tgz -C $(ONLINE_UPDATE_PKGDIR)
else
# normal mode, call "make install"

	@mkdir -p $(ONLINE_UPDATE_PKGDIR)/etc/config-tools/
		@cp $(ONLINE_UPDATE_BUILD_DIR)/$(ONLINE_UPDATE).elf $(ONLINE_UPDATE_PKGDIR)/etc/config-tools/$(ONLINE_UPDATE)
		@cp $(ONLINE_UPDATE_DIR)/etc/$(ONLINE_UPDATE).conf $(ONLINE_UPDATE_PKGDIR)/etc/$(ONLINE_UPDATE).conf
		
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(ONLINE_UPDATE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(ONLINE_UPDATE_PKGDIR) && tar cvzf $(ONLINE_UPDATE_PLATFORMCONFIGPACKAGEDIR)/$(ONLINE_UPDATE_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/online_update.targetinstall:
	@$(call targetinfo)

	@$(call install_init, online_update)
	@$(call install_fixup, online_update,PRIORITY,optional)
	@$(call install_fixup, online_update,SECTION,base)
	@$(call install_fixup, online_update,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, online_update,DESCRIPTION,missing)

	@$(call install_copy, online_update, 0, 0, 0700, -, /etc/config-tools/online_update)
	@$(call install_copy, online_update, 0, 0, 0600, -, /etc/online_update.conf)

ifdef PTXCONF_CT_SETTINGS_BACKUP
	@$(call install_alternative, online_update, 0, 0, 0750, /etc/config-tools/backup-restore/backup_online_update)
endif

	@$(call install_finish, online_update)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/online_update.clean:
	@$(call targetinfo)
	@if [ -d $(ONLINE_UPDATE_DIR) ]; then \
		$(ONLINE_UPDATE_MAKE_ENV) $(ONLINE_UPDATE_PATH) $(MAKE) $(MFLAGS) -C $(ONLINE_UPDATE_DIR) clean; \
	fi
	@$(call clean_pkg, ONLINE_UPDATE)
	@rm -rf $(ONLINE_UPDATE_BUILDROOT_DIR)

# vim: syntax=make
