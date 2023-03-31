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
PACKAGES-$(PTXCONF_PP_SWITCHCONFIG) += pp_switchconfig

#
# Paths and names
#
PP_SWITCHCONFIG_VERSION        := 1.1.0
PP_SWITCHCONFIG_MD5            :=
PP_SWITCHCONFIG                := pp_switchconfig
PP_SWITCHCONFIG_BUILDCONFIG    := Release
PP_SWITCHCONFIG_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SWITCHCONFIG)
PP_SWITCHCONFIG_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SWITCHCONFIG)
PP_SWITCHCONFIG_DIR            := $(PP_SWITCHCONFIG_BUILDROOT_DIR)/src
PP_SWITCHCONFIG_BUILD_DIR      := $(PP_SWITCHCONFIG_BUILDROOT_DIR)/bin/$(PP_SWITCHCONFIG_BUILDCONFIG)
PP_SWITCHCONFIG_LICENSE        := WAGO
PP_SWITCHCONFIG_CONF_TOOL      := NO
PP_SWITCHCONFIG_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SWITCHCONFIG_BUILDCONFIG) \
BIN_DIR=$(PP_SWITCHCONFIG_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SWITCHCONFIG_PACKAGE_NAME             := $(PP_SWITCHCONFIG)_$(PP_SWITCHCONFIG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SWITCHCONFIG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_switchconfig.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_SWITCHCONFIG_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SWITCHCONFIG_DIR) ]; then \
	  ln -s $(PP_SWITCHCONFIG_SRC_DIR) $(PP_SWITCHCONFIG_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SWITCHCONFIG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SWITCHCONFIG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SWITCHCONFIG_PKGDIR) && \
	  tar xvzf $(PP_SWITCHCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(PP_SWITCHCONFIG_PACKAGE_NAME).tgz -C $(PP_SWITCHCONFIG_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SWITCHCONFIG)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SWITCHCONFIG_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SWITCHCONFIG_PKGDIR) && tar cvzf $(PP_SWITCHCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(PP_SWITCHCONFIG_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

# $(STATEDIR)/pp_switchconfig.targetinstall:
# 	@$(call targetinfo)

# 	@$(call install_init, pp_switchconfig)
# 	@$(call install_fixup, pp_switchconfig,PRIORITY,optional)
# 	@$(call install_fixup, pp_switchconfig,SECTION,base)
# 	@$(call install_fixup, pp_switchconfig,AUTHOR,"WAGO GmbH \& Co. KG")
# 	@$(call install_fixup, pp_switchconfig,DESCRIPTION,missing)

# 	@$(call install_lib, pp_switchconfig, 0, 0, 0644, libpp_switchconfig)
# 	@$(call install_copy, pp_switchconfig, 0, 0, 0755, -, /usr/bin/pp_switchconfig)

# 	@$(call install_finish, pp_switchconfig)

# 	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_switchconfig.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SWITCHCONFIG_DIR) ]; then \
		$(PP_SWITCHCONFIG_MAKE_ENV) $(PP_SWITCHCONFIG_PATH) $(MAKE) $(MFLAGS) -C $(PP_SWITCHCONFIG_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SWITCHCONFIG)
	@rm -rf $(PP_SWITCHCONFIG_BUILDROOT_DIR)

# vim: syntax=make
