# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PP_REBOOT) += pp_reboot

#
# Paths and names
#
PP_REBOOT_VERSION        := 1.0.0
PP_REBOOT_MD5            :=
PP_REBOOT                := pp_reboot
PP_REBOOT_BUILDCONFIG    := Release
PP_REBOOT_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_REBOOT)
PP_REBOOT_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_REBOOT)
PP_REBOOT_DIR            := $(PP_REBOOT_BUILDROOT_DIR)
PP_REBOOT_BUILD_DIR      := $(PP_REBOOT_BUILDROOT_DIR)/bin/$(PP_REBOOT_BUILDCONFIG)
PP_REBOOT_LICENSE        := WAGO
PP_REBOOT_CONF_TOOL      := NO
PP_REBOOT_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_REBOOT_BUILDCONFIG) \
BIN_DIR=$(PP_REBOOT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_REBOOT_PACKAGE_NAME             := $(PP_REBOOT)_$(PP_REBOOT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_REBOOT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_reboot.extract: 
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_REBOOT_DIR) ]; then \
	  ln -s $(PP_REBOOT_SRC_DIR) $(PP_REBOOT_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_reboot.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_REBOOT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_reboot.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_REBOOT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_reboot.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_REBOOT_PKGDIR) && \
	  tar xvzf $(PP_REBOOT_PLATFORMCONFIGPACKAGEDIR)/$(PP_REBOOT_PACKAGE_NAME).tgz -C $(PP_REBOOT_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_REBOOT)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_REBOOT_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_REBOOT_PKGDIR) && tar cvzf $(PP_REBOOT_PLATFORMCONFIGPACKAGEDIR)/$(PP_REBOOT_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_reboot.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_reboot)
	@$(call install_fixup, pp_reboot,PRIORITY,optional)
	@$(call install_fixup, pp_reboot,SECTION,base)
	@$(call install_fixup, pp_reboot,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_reboot,DESCRIPTION,missing)

	@$(call install_copy, pp_reboot, 0, 0, 0750, -, /etc/init.d/set_bootstatus)
	@$(call install_link, pp_reboot, ../init.d/set_bootstatus, /etc/rc.d/S99_z_set_bootstatus)

	@$(call install_finish, pp_reboot)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_reboot.clean:
	@$(call targetinfo)
	@if [ -d $(PP_REBOOT_DIR) ]; then \
		$(PP_REBOOT_MAKE_ENV) $(PP_REBOOT_PATH) $(MAKE) $(MFLAGS) -C $(PP_REBOOT_DIR) clean; \
	fi
	@$(call clean_pkg, PP_REBOOT)
	@rm -rf $(PP_REBOOT_BUILDROOT_DIR)

# vim: syntax=make
