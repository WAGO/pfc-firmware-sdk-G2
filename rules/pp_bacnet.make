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
PACKAGES-$(PTXCONF_PP_BACNET) += pp_bacnet

#
# Paths and names
#
PP_BACNET_VERSION        := 0.1.0
PP_BACNET_MD5            :=
PP_BACNET                := pp_bacnet
PP_BACNET_BUILDCONFIG    := Release
PP_BACNET_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_BACNET)
PP_BACNET_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_BACNET)
PP_BACNET_DIR            := $(PP_BACNET_BUILDROOT_DIR)/src
PP_BACNET_BUILD_DIR      := $(PP_BACNET_BUILDROOT_DIR)/bin/$(PP_BACNET_BUILDCONFIG)
PP_BACNET_LICENSE        := WAGO
PP_BACNET_CONF_TOOL      := NO
PP_BACNET_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_BACNET_BUILDCONFIG) \
BIN_DIR=$(PP_BACNET_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_BACNET_PACKAGE_NAME             := $(PP_BACNET)_$(PP_BACNET_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_BACNET_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_bacnet.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_BACNET_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_BACNET_DIR) ]; then \
	  ln -s $(PP_BACNET_SRC_DIR) $(PP_BACNET_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_BACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_BACNET)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_BACNET_PKGDIR) && \
	  tar xvzf $(PP_BACNET_PLATFORMCONFIGPACKAGEDIR)/$(PP_BACNET_PACKAGE_NAME).tgz -C $(PP_BACNET_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_BACNET)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_BACNET_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_BACNET_PKGDIR) && tar cvzf $(PP_BACNET_PLATFORMCONFIGPACKAGEDIR)/$(PP_BACNET_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_bacnet)
	@$(call install_fixup, pp_bacnet,PRIORITY,optional)
	@$(call install_fixup, pp_bacnet,SECTION,base)
	@$(call install_fixup, pp_bacnet,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_bacnet,DESCRIPTION,missing)

	@$(call install_copy, pp_bacnet, 0, 0, 0750, -, /usr/sbin/pp_bacnet)
	@$(call install_copy, pp_bacnet, 0, 0, 0700, -, /etc/init.d/pp_bacnet)
	@$(call install_link, pp_bacnet, ../init.d/pp_bacnet, /etc/rc.d/S87_pp_bacnet)
	
	@$(call install_finish, pp_bacnet)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_bacnet.clean:
	@$(call targetinfo)
	@if [ -d $(PP_BACNET_DIR) ]; then \
		$(PP_BACNET_MAKE_ENV) $(PP_BACNET_PATH) $(MAKE) $(MFLAGS) -C $(PP_BACNET_DIR) clean; \
	fi
	@$(call clean_pkg, PP_BACNET)
	@rm -rf $(PP_BACNET_BUILDROOT_DIR)

# vim: syntax=make
