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
PACKAGES-$(PTXCONF_PP_SECURESHELL) += pp_secureshell

#
# Paths and names
#
PP_SECURESHELL_VERSION        := 1.0.0
PP_SECURESHELL_MD5            :=
PP_SECURESHELL                := pp_secureshell
PP_SECURESHELL_BUILDCONFIG    := Release
PP_SECURESHELL_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SECURESHELL)
PP_SECURESHELL_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SECURESHELL)
PP_SECURESHELL_DIR            := $(PP_SECURESHELL_BUILDROOT_DIR)/src
PP_SECURESHELL_BUILD_DIR      := $(PP_SECURESHELL_BUILDROOT_DIR)/bin/$(PP_SECURESHELL_BUILDCONFIG)
PP_SECURESHELL_LICENSE        := WAGO
PP_SECURESHELL_CONF_TOOL      := NO
PP_SECURESHELL_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SECURESHELL_BUILDCONFIG) \
BIN_DIR=$(PP_SECURESHELL_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SECURESHELL_PACKAGE_NAME             := $(PP_SECURESHELL)_$(PP_SECURESHELL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SECURESHELL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_secureshell.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_SECURESHELL_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SECURESHELL_DIR) ]; then \
	  ln -s $(PP_SECURESHELL_SRC_DIR) $(PP_SECURESHELL_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_secureshell.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SECURESHELL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_secureshell.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SECURESHELL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_secureshell.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SECURESHELL_PKGDIR) && \
	  tar xvzf $(PP_SECURESHELL_PLATFORMCONFIGPACKAGEDIR)/$(PP_SECURESHELL_PACKAGE_NAME).tgz -C $(PP_SECURESHELL_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SECURESHELL)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SECURESHELL_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SECURESHELL_PKGDIR) && tar cvzf $(PP_SECURESHELL_PLATFORMCONFIGPACKAGEDIR)/$(PP_SECURESHELL_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_secureshell.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_secureshell)
	@$(call install_fixup, pp_secureshell,PRIORITY,optional)
	@$(call install_fixup, pp_secureshell,SECTION,base)
	@$(call install_fixup, pp_secureshell,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_secureshell,DESCRIPTION,missing)

	@$(call install_finish, pp_secureshell)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_secureshell.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SECURESHELL_DIR) ]; then \
		$(PP_SECURESHELL_MAKE_ENV) $(PP_SECURESHELL_PATH) $(MAKE) $(MFLAGS) -C $(PP_SECURESHELL_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SECURESHELL)
	@rm -rf $(PP_SECURESHELL_BUILDROOT_DIR)

# vim: syntax=make
