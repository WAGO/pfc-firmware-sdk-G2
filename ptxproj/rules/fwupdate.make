# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FWUPDATE) += fwupdate

#
# Paths and names
#
FWUPDATE_VERSION        := 0.1.0
FWUPDATE_MD5            :=
FWUPDATE                := fwupdate
FWUPDATE_BUILDCONFIG    := Release
FWUPDATE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(FWUPDATE)
FWUPDATE_BUILDROOT_DIR  := $(BUILDDIR)/$(FWUPDATE)
FWUPDATE_DIR            := $(FWUPDATE_BUILDROOT_DIR)/src
FWUPDATE_BUILD_DIR      := $(FWUPDATE_BUILDROOT_DIR)/bin/$(FWUPDATE_BUILDCONFIG)
FWUPDATE_LICENSE        := WAGO
FWUPDATE_CONF_TOOL      := NO
FWUPDATE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(FWUPDATE_BUILDCONFIG) \
BIN_DIR=$(FWUPDATE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


WAGO_FWUPDATE                := wago-$(FWUPDATE)
WAGO_FWUPDATE_SRC_DIR        := $(PTXDIST_WORKSPACE)/shared_public/$(FWUPDATE)
WAGO_FWUPDATE_URL            := file://shared_public/$(FWUPDATE)
WAGO_FWUPDATE_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_FWUPDATE)
WAGO_FWUPDATE_DIR            := $(WAGO_FWUPDATE_BUILDROOT_DIR)/src
WAGO_FWUPDATE_LICENSE        := MPL-2.0

FWUPDATE_PACKAGE_NAME             := $(FWUPDATE)_$(FWUPDATE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
FWUPDATE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/fwupdate.extract: 
	@$(call targetinfo)
	@mkdir -p $(FWUPDATE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(FWUPDATE_DIR) ]; then \
	  ln -s $(FWUPDATE_SRC_DIR) $(FWUPDATE_DIR); \
	fi
endif

	@mkdir -p $(WAGO_FWUPDATE_BUILDROOT_DIR)
	
	# Link source directory
	@if [ ! -L $(WAGO_FWUPDATE_DIR) ]; then \
	  ln -s $(WAGO_FWUPDATE_SRC_DIR) $(WAGO_FWUPDATE_DIR); \
	fi

	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/fwupdate.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/prepare, FWUPDATE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/fwupdate.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, FWUPDATE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fwupdate.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(FWUPDATE_PKGDIR) && \
	  tar xvzf $(FWUPDATE_PLATFORMCONFIGPACKAGEDIR)/$(FWUPDATE_PACKAGE_NAME).tgz -C $(FWUPDATE_PKGDIR)
else
# normal mode, call "make install"

	$(call world/install, FWUPDATE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(FWUPDATE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(FWUPDATE_PKGDIR) && tar cvzf $(FWUPDATE_PLATFORMCONFIGPACKAGEDIR)/$(FWUPDATE_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fwupdate.targetinstall:
	@$(call targetinfo)

	@$(call install_init, fwupdate)
	@$(call install_fixup, fwupdate,PRIORITY,optional)
	@$(call install_fixup, fwupdate,SECTION,base)
	@$(call install_fixup, fwupdate,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, fwupdate,DESCRIPTION,missing)

	$(call install_copy, fwupdate, 0, 0, 0700, -, /usr/sbin/fwupdate_log)

	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate, /etc/config-tools/fwupdate)
	@$(call install_copy, fwupdate, 0, 0, 0600, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_basic_defines, /usr/sbin/fwupdate_basic_defines)
	@$(call install_replace, fwupdate, /usr/sbin/fwupdate_basic_defines, @FW_UPDATE_GROUP@, $(PTXCONF_WAGO_FW_UPDATE_GROUP))
	@$(call install_copy, fwupdate, 0, 0, 0600, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_private_storage, /usr/sbin/fwupdate_private_storage)
	@$(call install_copy, fwupdate, 0, 0, 0600, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_common, /usr/sbin/fwupdate_common)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_background_service, /usr/sbin/fwupdate_background_service)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_status, /usr/sbin/fwupdate_status)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_control, /usr/sbin/fwupdate_control)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_mode, /usr/sbin/fwupdate_mode)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_report, /usr/sbin/fwupdate_report)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_storage, /usr/sbin/fwupdate_storage)
	@$(call install_copy, fwupdate, 0, 0, 0700, $(WAGO_FWUPDATE_DIR)/scripts/fwupdate_service, /etc/init.d/fwupdate_service)
	@$(call install_link, fwupdate, ../init.d/fwupdate_service, /etc/rc.d/disabled/S15_fwupdate_service)

ifeq ($(PTXCONF_PLATFORM), $(filter $(PTXCONF_PLATFORM),wago-pfc300 wago-wp400))
	@$(call install_copy, fwupdate, 0, 0, 0755, /home/wago/log)
	@$(call install_copy, fwupdate, 0, 0, 0755, /home/wago/settings)
endif

ifdef PTXCONF_BAREBOX
	@$(call install_alternative, fwupdate, 0, 0, 0755, \
		/usr/sbin/barebox-state_failsafe.sh)
endif

	@$(call install_finish, fwupdate)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/fwupdate.clean:
	@$(call targetinfo)
	@if [ -d $(FWUPDATE_DIR) ]; then \
		$(FWUPDATE_MAKE_ENV) $(FWUPDATE_PATH) $(MAKE) $(MFLAGS) -C $(FWUPDATE_DIR) clean; \
	fi
	@$(call clean_pkg, FWUPDATE)
	@rm -rf $(FWUPDATE_BUILDROOT_DIR)
	@rm -rf $(WAGO_FWUPDATE_BUILDROOT_DIR)

# vim: syntax=make
