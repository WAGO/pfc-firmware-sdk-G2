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
PACKAGES-$(PTXCONF_PP_NETCONF) += pp_netconf

#
# Paths and names
#
PP_NETCONF_VERSION        := 1.0.0
PP_NETCONF_MD5            :=
PP_NETCONF                := pp_netconf
PP_NETCONF_BUILDCONFIG    := Release
PP_NETCONF_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_NETCONF)
PP_NETCONF_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_NETCONF)
PP_NETCONF_DIR            := $(PP_NETCONF_BUILDROOT_DIR)/src
PP_NETCONF_BUILD_DIR      := $(PP_NETCONF_BUILDROOT_DIR)/bin/$(PP_NETCONF_BUILDCONFIG)
PP_NETCONF_LICENSE        := WAGO
PP_NETCONF_CONF_TOOL      := NO
PP_NETCONF_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_NETCONF_BUILDCONFIG) \
BIN_DIR=$(PP_NETCONF_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_NETCONF_PACKAGE_NAME             := $(PP_NETCONF)_$(PP_NETCONF_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_NETCONF_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_netconf.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_NETCONF_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_NETCONF_DIR) ]; then \
	  ln -s $(PP_NETCONF_SRC_DIR) $(PP_NETCONF_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_netconf.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_NETCONF)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_netconf.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_NETCONF)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_netconf.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_NETCONF_PKGDIR) && \
	  tar xvzf $(PP_NETCONF_PLATFORMCONFIGPACKAGEDIR)/$(PP_NETCONF_PACKAGE_NAME).tgz -C $(PP_NETCONF_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_NETCONF)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_NETCONF_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_NETCONF_PKGDIR) && tar cvzf $(PP_NETCONF_PLATFORMCONFIGPACKAGEDIR)/$(PP_NETCONF_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_netconf.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_netconf)
	@$(call install_fixup, pp_netconf,PRIORITY,optional)
	@$(call install_fixup, pp_netconf,SECTION,base)
	@$(call install_fixup, pp_netconf,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_netconf,DESCRIPTION,missing)

	@$(call install_finish, pp_netconf)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_netconf.clean:
	@$(call targetinfo)
	@if [ -d $(PP_NETCONF_DIR) ]; then \
		$(PP_NETCONF_MAKE_ENV) $(PP_NETCONF_PATH) $(MAKE) $(MFLAGS) -C $(PP_NETCONF_DIR) clean; \
	fi
	@$(call clean_pkg, PP_NETCONF)
	@rm -rf $(PP_NETCONF_BUILDROOT_DIR)

# vim: syntax=make
