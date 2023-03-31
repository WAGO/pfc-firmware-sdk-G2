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
PACKAGES-$(PTXCONF_PP_PASSWORD_MANAGEMENT) += pp_password_management

#
# Paths and names
#
PP_PASSWORD_MANAGEMENT_VERSION        := 1.0.0
PP_PASSWORD_MANAGEMENT_MD5            :=
PP_PASSWORD_MANAGEMENT                := pp_password_management
PP_PASSWORD_MANAGEMENT_BUILDCONFIG    := Release
PP_PASSWORD_MANAGEMENT_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_PASSWORD_MANAGEMENT)
PP_PASSWORD_MANAGEMENT_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_PASSWORD_MANAGEMENT)
PP_PASSWORD_MANAGEMENT_DIR            := $(PP_PASSWORD_MANAGEMENT_BUILDROOT_DIR)/src
PP_PASSWORD_MANAGEMENT_BUILD_DIR      := $(PP_PASSWORD_MANAGEMENT_BUILDROOT_DIR)/bin/$(PP_PASSWORD_MANAGEMENT_BUILDCONFIG)
PP_PASSWORD_MANAGEMENT_LICENSE        := WAGO
PP_PASSWORD_MANAGEMENT_CONF_TOOL      := NO
PP_PASSWORD_MANAGEMENT_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_PASSWORD_MANAGEMENT_BUILDCONFIG) \
BIN_DIR=$(PP_PASSWORD_MANAGEMENT_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_PASSWORD_MANAGEMENT_PACKAGE_NAME             := $(PP_PASSWORD_MANAGEMENT)_$(PP_PASSWORD_MANAGEMENT_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_PASSWORD_MANAGEMENT_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_password_management.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_PASSWORD_MANAGEMENT_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_PASSWORD_MANAGEMENT_DIR) ]; then \
	  ln -s $(PP_PASSWORD_MANAGEMENT_SRC_DIR) $(PP_PASSWORD_MANAGEMENT_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_password_management.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_PASSWORD_MANAGEMENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_password_management.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_PASSWORD_MANAGEMENT)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_password_management.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_PASSWORD_MANAGEMENT_PKGDIR) && \
	  tar xvzf $(PP_PASSWORD_MANAGEMENT_PLATFORMCONFIGPACKAGEDIR)/$(PP_PASSWORD_MANAGEMENT_PACKAGE_NAME).tgz -C $(PP_PASSWORD_MANAGEMENT_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_PASSWORD_MANAGEMENT)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_PASSWORD_MANAGEMENT_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_PASSWORD_MANAGEMENT_PKGDIR) && tar cvzf $(PP_PASSWORD_MANAGEMENT_PLATFORMCONFIGPACKAGEDIR)/$(PP_PASSWORD_MANAGEMENT_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_password_management.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_password_management)
	@$(call install_fixup, pp_password_management,PRIORITY,optional)
	@$(call install_fixup, pp_password_management,SECTION,base)
	@$(call install_fixup, pp_password_management,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_password_management,DESCRIPTION,missing)

	@$(call install_finish, pp_password_management)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_password_management.clean:
	@$(call targetinfo)
	@if [ -d $(PP_PASSWORD_MANAGEMENT_DIR) ]; then \
		$(PP_PASSWORD_MANAGEMENT_MAKE_ENV) $(PP_PASSWORD_MANAGEMENT_PATH) $(MAKE) $(MFLAGS) -C $(PP_PASSWORD_MANAGEMENT_DIR) clean; \
	fi
	@$(call clean_pkg, PP_PASSWORD_MANAGEMENT)
	@rm -rf $(PP_PASSWORD_MANAGEMENT_BUILDROOT_DIR)

# vim: syntax=make
