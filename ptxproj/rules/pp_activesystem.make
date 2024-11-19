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
PACKAGES-$(PTXCONF_PP_ACTIVESYSTEM) += pp_activesystem

#
# Paths and names
#
PP_ACTIVESYSTEM_VERSION        := 1.0.0
PP_ACTIVESYSTEM_MD5            :=
PP_ACTIVESYSTEM                := pp_activesystem
PP_ACTIVESYSTEM_BUILDCONFIG    := Release
PP_ACTIVESYSTEM_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_ACTIVESYSTEM)
PP_ACTIVESYSTEM_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_ACTIVESYSTEM)
PP_ACTIVESYSTEM_DIR            := $(PP_ACTIVESYSTEM_BUILDROOT_DIR)/src
PP_ACTIVESYSTEM_BUILD_DIR      := $(PP_ACTIVESYSTEM_BUILDROOT_DIR)/bin/$(PP_ACTIVESYSTEM_BUILDCONFIG)
PP_ACTIVESYSTEM_LICENSE        := WAGO
PP_ACTIVESYSTEM_CONF_TOOL      := NO
PP_ACTIVESYSTEM_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_ACTIVESYSTEM_BUILDCONFIG) \
BIN_DIR=$(PP_ACTIVESYSTEM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_ACTIVESYSTEM_PACKAGE_NAME             := $(PP_ACTIVESYSTEM)_$(PP_ACTIVESYSTEM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_ACTIVESYSTEM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

ifdef PTXCONF_U_BOOT_TOOLS
	PP_ACTIVESYSTEM_MAKE_ENV += USE_UBOOT=-DUSE_UBOOT
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_activesystem.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_ACTIVESYSTEM_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_ACTIVESYSTEM_DIR) ]; then \
	  ln -s $(PP_ACTIVESYSTEM_SRC_DIR) $(PP_ACTIVESYSTEM_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_activesystem.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_ACTIVESYSTEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_activesystem.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_ACTIVESYSTEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_activesystem.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_ACTIVESYSTEM_PKGDIR) && \
	  tar xvzf $(PP_ACTIVESYSTEM_PLATFORMCONFIGPACKAGEDIR)/$(PP_ACTIVESYSTEM_PACKAGE_NAME).tgz -C $(PP_ACTIVESYSTEM_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_ACTIVESYSTEM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_ACTIVESYSTEM_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_ACTIVESYSTEM_PKGDIR) && tar cvzf $(PP_ACTIVESYSTEM_PLATFORMCONFIGPACKAGEDIR)/$(PP_ACTIVESYSTEM_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_activesystem.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_activesystem)
	@$(call install_fixup, pp_activesystem,PRIORITY,optional)
	@$(call install_fixup, pp_activesystem,SECTION,base)
	@$(call install_fixup, pp_activesystem,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_activesystem,DESCRIPTION,missing)

	@$(call install_finish, pp_activesystem)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_activesystem.clean:
	@$(call targetinfo)
	@if [ -d $(PP_ACTIVESYSTEM_DIR) ]; then \
		$(PP_ACTIVESYSTEM_MAKE_ENV) $(PP_ACTIVESYSTEM_PATH) $(MAKE) $(MFLAGS) -C $(PP_ACTIVESYSTEM_DIR) clean; \
	fi
	@$(call clean_pkg, PP_ACTIVESYSTEM)
	@rm -rf $(PP_ACTIVESYSTEM_BUILDROOT_DIR)

# vim: syntax=make
