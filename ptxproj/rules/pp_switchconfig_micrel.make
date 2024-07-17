# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PP_SWITCHCONFIG_MICREL) += pp_switchconfig_micrel

#
# Paths and names
#
PP_SWITCHCONFIG_MICREL_VERSION        := 1.1.0
PP_SWITCHCONFIG_MICREL_MD5            :=
PP_SWITCHCONFIG_MICREL                := pp_switchconfig_micrel
PP_SWITCHCONFIG_MICREL_BUILDCONFIG    := Release
PP_SWITCHCONFIG_MICREL_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SWITCHCONFIG_MICREL)
PP_SWITCHCONFIG_MICREL_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SWITCHCONFIG_MICREL)
PP_SWITCHCONFIG_MICREL_DIR            := $(PP_SWITCHCONFIG_MICREL_BUILDROOT_DIR)/src
PP_SWITCHCONFIG_MICREL_BUILD_DIR      := $(PP_SWITCHCONFIG_MICREL_BUILDROOT_DIR)/bin/$(PP_SWITCHCONFIG_MICREL_BUILDCONFIG)
PP_SWITCHCONFIG_MICREL_LICENSE        := WAGO
PP_SWITCHCONFIG_MICREL_CONF_TOOL      := NO
PP_SWITCHCONFIG_MICREL_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SWITCHCONFIG_MICREL_BUILDCONFIG) \
BIN_DIR=$(PP_SWITCHCONFIG_MICREL_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SWITCHCONFIG_MICREL_PACKAGE_NAME             := $(PP_SWITCHCONFIG_MICREL)_$(PP_SWITCHCONFIG_MICREL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SWITCHCONFIG_MICREL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_switchconfig_micrel.extract:
	@$(call targetinfo)
	@mkdir -p $(PP_SWITCHCONFIG_MICREL_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SWITCHCONFIG_MICREL_DIR) ]; then \
	  ln -s $(PP_SWITCHCONFIG_MICREL_SRC_DIR) $(PP_SWITCHCONFIG_MICREL_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig_micrel_micrel.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SWITCHCONFIG_MICREL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig_micrel.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SWITCHCONFIG_MICREL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig_micrel.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SWITCHCONFIG_MICREL_PKGDIR) && \
	  tar xvzf $(PP_SWITCHCONFIG_MICREL_PLATFORMCONFIGPACKAGEDIR)/$(PP_SWITCHCONFIG_MICREL_PACKAGE_NAME).tgz -C $(PP_SWITCHCONFIG_MICREL_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SWITCHCONFIG_MICREL)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SWITCHCONFIG_MICREL_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SWITCHCONFIG_MICREL_PKGDIR) && tar cvzf $(PP_SWITCHCONFIG_MICREL_PLATFORMCONFIGPACKAGEDIR)/$(PP_SWITCHCONFIG_MICREL_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig_micrel.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_switchconfig_micrel)
	@$(call install_fixup, pp_switchconfig_micrel,PRIORITY,optional)
	@$(call install_fixup, pp_switchconfig_micrel,SECTION,base)
	@$(call install_fixup, pp_switchconfig_micrel,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_switchconfig_micrel,DESCRIPTION,missing)

	@$(call install_copy, pp_switchconfig_micrel, 0, 0, 0755, -, /usr/bin/pp_switchconfig)
	@$(call install_copy, pp_switchconfig_micrel, 0, 0, 0755, -, /etc/init.d/pp_switchconfig)
	@$(call install_link, pp_switchconfig_micrel, ../init.d/pp_switchconfig, /etc/rc.d/S90_pp_switchconfig)

	@$(call install_finish, pp_switchconfig_micrel)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig_micrel.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SWITCHCONFIG_MICREL_DIR) ]; then \
		$(PP_SWITCHCONFIG_MICREL_MAKE_ENV) $(PP_SWITCHCONFIG_MICREL_PATH) $(MAKE) $(MFLAGS) -C $(PP_SWITCHCONFIG_MICREL_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SWITCHCONFIG_MICREL)
	@rm -rf $(PP_SWITCHCONFIG_MICREL_BUILDROOT_DIR)

# vim: syntax=make
