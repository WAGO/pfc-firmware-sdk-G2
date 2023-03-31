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
PACKAGES-$(PTXCONF_PP_CODESYS3) += pp_codesys3

#
# Paths and names
#
PP_CODESYS3_VERSION        := 1.0.0
PP_CODESYS3_MD5            :=
PP_CODESYS3                := pp_codesys3
PP_CODESYS3_BUILDCONFIG    := Release
PP_CODESYS3_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_CODESYS3)
PP_CODESYS3_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_CODESYS3)
PP_CODESYS3_DIR            := $(PP_CODESYS3_BUILDROOT_DIR)/src
PP_CODESYS3_BUILD_DIR      := $(PP_CODESYS3_BUILDROOT_DIR)/bin/$(PP_CODESYS3_BUILDCONFIG)
PP_CODESYS3_LICENSE        := WAGO
PP_CODESYS3_CONF_TOOL      := NO
PP_CODESYS3_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_CODESYS3_BUILDCONFIG) \
BIN_DIR=$(PP_CODESYS3_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_CODESYS3_PACKAGE_NAME             := $(PP_CODESYS3)_$(PP_CODESYS3_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_CODESYS3_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_codesys3.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_CODESYS3_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_CODESYS3_DIR) ]; then \
	  ln -s $(PP_CODESYS3_SRC_DIR) $(PP_CODESYS3_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys3.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_CODESYS3)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys3.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_CODESYS3)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys3.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_CODESYS3_PKGDIR) && \
	  tar xvzf $(PP_CODESYS3_PLATFORMCONFIGPACKAGEDIR)/$(PP_CODESYS3_PACKAGE_NAME).tgz -C $(PP_CODESYS3_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_CODESYS3)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_CODESYS3_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_CODESYS3_PKGDIR) && tar cvzf $(PP_CODESYS3_PLATFORMCONFIGPACKAGEDIR)/$(PP_CODESYS3_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys3.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_codesys3)
	@$(call install_fixup, pp_codesys3,PRIORITY,optional)
	@$(call install_fixup, pp_codesys3,SECTION,base)
	@$(call install_fixup, pp_codesys3,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_codesys3,DESCRIPTION,missing)

	@$(call install_copy, pp_codesys3, 0, 0, 0755, -, /usr/sbin/pp_codesys3)
	@$(call install_copy, pp_codesys3, 0, 0, 0750, -, /etc/init.d/pp_codesys3)
	
	@$(call install_link, pp_codesys3, /etc/init.d/pp_codesys3, /etc/rc.d/S98_pp_codesys3)

	@$(call install_finish, pp_codesys3)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys3.clean:
	@$(call targetinfo)
	@if [ -d $(PP_CODESYS3_DIR) ]; then \
		$(PP_CODESYS3_MAKE_ENV) $(PP_CODESYS3_PATH) $(MAKE) $(MFLAGS) -C $(PP_CODESYS3_DIR) clean; \
	fi
	@$(call clean_pkg, PP_CODESYS3)
	@rm -rf $(PP_CODESYS3_BUILDROOT_DIR)

# vim: syntax=make
