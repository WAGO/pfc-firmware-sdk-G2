# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO Kontakttechnik GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_HS_UTILS) += hs-utils

#
# Paths and names
#
HS_UTILS_VERSION        := 0.1.0
HS_UTILS_MD5            :=
HS_UTILS                := hs-utils
HS_UTILS_BUILDCONFIG    := Release
HS_UTILS_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(HS_UTILS)
HS_UTILS_BUILDROOT_DIR  := $(BUILDDIR)/$(HS_UTILS)
HS_UTILS_DIR            := $(HS_UTILS_BUILDROOT_DIR)/src
HS_UTILS_BUILD_DIR      := $(HS_UTILS_BUILDROOT_DIR)/bin/$(HS_UTILS_BUILDCONFIG)
HS_UTILS_LICENSE        := WAGO
HS_UTILS_CONF_TOOL      := NO
HS_UTILS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(HS_UTILS_BUILDCONFIG) \
BIN_DIR=$(HS_UTILS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

HS_UTILS_PACKAGE_NAME             := $(HS_UTILS)_$(HS_UTILS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
HS_UTILS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/hs-utils.extract: 
	@$(call targetinfo)
	@mkdir -p $(HS_UTILS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(HS_UTILS_DIR) ]; then \
	  ln -s $(HS_UTILS_SRC_DIR) $(HS_UTILS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/hs-utils.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, HS_UTILS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/hs-utils.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, HS_UTILS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/hs-utils.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(HS_UTILS_PKGDIR) && \
	  tar xvzf $(HS_UTILS_PLATFORMCONFIGPACKAGEDIR)/$(HS_UTILS_PACKAGE_NAME).tgz -C $(HS_UTILS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, HS_UTILS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(HS_UTILS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(HS_UTILS_PKGDIR) && tar cvzf $(HS_UTILS_PLATFORMCONFIGPACKAGEDIR)/$(HS_UTILS_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/hs-utils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, hs-utils)
	@$(call install_fixup, hs-utils,PRIORITY,optional)
	@$(call install_fixup, hs-utils,SECTION,base)
	@$(call install_fixup, hs-utils,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, hs-utils,DESCRIPTION,missing)
	@$(call install_finish, hs-utils)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/hs-utils.clean:
	@$(call targetinfo)
	@if [ -d $(HS_UTILS_DIR) ]; then \
		$(HS_UTILS_MAKE_ENV) $(HS_UTILS_PATH) $(MAKE) $(MFLAGS) -C $(HS_UTILS_DIR) clean; \
	fi
	@$(call clean_pkg, HS_UTILS)
	@rm -rf $(HS_UTILS_BUILDROOT_DIR)

# vim: syntax=make
