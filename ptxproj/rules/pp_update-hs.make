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
PACKAGES-$(PTXCONF_PP_UPDATE_HS) += pp_update-hs

#
# Paths and names
#
PP_UPDATE_HS_VERSION        := 1.0.0
PP_UPDATE_HS_MD5            :=
PP_UPDATE_HS                := pp_update-hs
PP_UPDATE_HS_BUILDCONFIG    := Release
PP_UPDATE_HS_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_UPDATE_HS)
PP_UPDATE_HS_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_UPDATE_HS)
PP_UPDATE_HS_DIR            := $(PP_UPDATE_HS_BUILDROOT_DIR)/src
PP_UPDATE_HS_BUILD_DIR      := $(PP_UPDATE_HS_BUILDROOT_DIR)/bin/$(PP_UPDATE_HS_BUILDCONFIG)
PP_UPDATE_HS_LICENSE        := WAGO
PP_UPDATE_HS_CONF_TOOL      := NO
PP_UPDATE_HS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_UPDATE_HS_BUILDCONFIG) \
BIN_DIR=$(PP_UPDATE_HS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_UPDATE_HS_PACKAGE_NAME             := $(PP_UPDATE_HS)_$(PP_UPDATE_HS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_UPDATE_HS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_update-hs.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_UPDATE_HS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_UPDATE_HS_DIR) ]; then \
	  ln -s $(PP_UPDATE_HS_SRC_DIR) $(PP_UPDATE_HS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_update-hs.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_UPDATE_HS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_update-hs.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_UPDATE_HS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_update-hs.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_UPDATE_HS_PKGDIR) && \
	  tar xvzf $(PP_UPDATE_HS_PLATFORMCONFIGPACKAGEDIR)/$(PP_UPDATE_HS_PACKAGE_NAME).tgz -C $(PP_UPDATE_HS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_UPDATE_HS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_UPDATE_HS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_UPDATE_HS_PKGDIR) && tar cvzf $(PP_UPDATE_HS_PLATFORMCONFIGPACKAGEDIR)/$(PP_UPDATE_HS_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_update-hs.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_update-hs)
	@$(call install_fixup, pp_update-hs,PRIORITY,optional)
	@$(call install_fixup, pp_update-hs,SECTION,base)
	@$(call install_fixup, pp_update-hs,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_update-hs,DESCRIPTION,missing)


	@$(call install_finish, pp_update-hs)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_update-hs.clean:
	@$(call targetinfo)
	@if [ -d $(PP_UPDATE_HS_DIR) ]; then \
		$(PP_UPDATE_HS_MAKE_ENV) $(PP_UPDATE_HS_PATH) $(MAKE) $(MFLAGS) -C $(PP_UPDATE_HS_DIR) clean; \
	fi
	@$(call clean_pkg, PP_UPDATE_HS)
	@rm -rf $(PP_UPDATE_HS_BUILDROOT_DIR)

# vim: syntax=make
