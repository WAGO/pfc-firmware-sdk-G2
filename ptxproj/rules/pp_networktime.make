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
PACKAGES-$(PTXCONF_PP_NETWORKTIME) += pp_networktime

#
# Paths and names
#
PP_NETWORKTIME_VERSION        := 1.0.0
PP_NETWORKTIME_MD5            :=
PP_NETWORKTIME                := pp_networktime
PP_NETWORKTIME_BUILDCONFIG    := Release
PP_NETWORKTIME_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_NETWORKTIME)
PP_NETWORKTIME_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_NETWORKTIME)
PP_NETWORKTIME_DIR            := $(PP_NETWORKTIME_BUILDROOT_DIR)/src
PP_NETWORKTIME_BUILD_DIR      := $(PP_NETWORKTIME_BUILDROOT_DIR)/bin/$(PP_NETWORKTIME_BUILDCONFIG)
PP_NETWORKTIME_LICENSE        := WAGO
PP_NETWORKTIME_CONF_TOOL      := NO
PP_NETWORKTIME_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_NETWORKTIME_BUILDCONFIG) \
BIN_DIR=$(PP_NETWORKTIME_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_NETWORKTIME_PACKAGE_NAME             := $(PP_NETWORKTIME)_$(PP_NETWORKTIME_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_NETWORKTIME_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_networktime.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_NETWORKTIME_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_NETWORKTIME_DIR) ]; then \
	  ln -s $(PP_NETWORKTIME_SRC_DIR) $(PP_NETWORKTIME_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networktime.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_NETWORKTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networktime.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_NETWORKTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networktime.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_NETWORKTIME_PKGDIR) && \
	  tar xvzf $(PP_NETWORKTIME_PLATFORMCONFIGPACKAGEDIR)/$(PP_NETWORKTIME_PACKAGE_NAME).tgz -C $(PP_NETWORKTIME_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_NETWORKTIME)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_NETWORKTIME_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_NETWORKTIME_PKGDIR) && tar cvzf $(PP_NETWORKTIME_PLATFORMCONFIGPACKAGEDIR)/$(PP_NETWORKTIME_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networktime.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_networktime)
	@$(call install_fixup, pp_networktime,PRIORITY,optional)
	@$(call install_fixup, pp_networktime,SECTION,base)
	@$(call install_fixup, pp_networktime,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_networktime,DESCRIPTION,missing)

	@$(call install_finish, pp_networktime)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networktime.clean:
	@$(call targetinfo)
	@if [ -d $(PP_NETWORKTIME_DIR) ]; then \
		$(PP_NETWORKTIME_MAKE_ENV) $(PP_NETWORKTIME_PATH) $(MAKE) $(MFLAGS) -C $(PP_NETWORKTIME_DIR) clean; \
	fi
	@$(call clean_pkg, PP_NETWORKTIME)
	@rm -rf $(PP_NETWORKTIME_BUILDROOT_DIR)

# vim: syntax=make
