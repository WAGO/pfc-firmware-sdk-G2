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
PACKAGES-$(PTXCONF_PP_ROUTING) += pp_routing

#
# Paths and names
#
PP_ROUTING_VERSION        := 1.0.0
PP_ROUTING_MD5            :=
PP_ROUTING                := pp_routing
PP_ROUTING_BUILDCONFIG    := Release
PP_ROUTING_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_ROUTING)
PP_ROUTING_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_ROUTING)
PP_ROUTING_DIR            := $(PP_ROUTING_BUILDROOT_DIR)/src
PP_ROUTING_BUILD_DIR      := $(PP_ROUTING_BUILDROOT_DIR)/bin/$(PP_ROUTING_BUILDCONFIG)
PP_ROUTING_LICENSE        := WAGO
PP_ROUTING_CONF_TOOL      := NO
PP_ROUTING_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_ROUTING_BUILDCONFIG) \
BIN_DIR=$(PP_ROUTING_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_ROUTING_PACKAGE_NAME             := $(PP_ROUTING)_$(PP_ROUTING_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_ROUTING_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_routing.extract:
	@$(call targetinfo)
	@mkdir -p $(PP_ROUTING_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_ROUTING_DIR) ]; then \
	  ln -s $(PP_ROUTING_SRC_DIR) $(PP_ROUTING_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_routing.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_ROUTING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_routing.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_ROUTING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_routing.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_ROUTING_PKGDIR) && \
	  tar xvzf $(PP_ROUTING_PLATFORMCONFIGPACKAGEDIR)/$(PP_ROUTING_PACKAGE_NAME).tgz -C $(PP_ROUTING_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_ROUTING)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_ROUTING_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_ROUTING_PKGDIR) && tar cvzf $(PP_ROUTING_PLATFORMCONFIGPACKAGEDIR)/$(PP_ROUTING_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/pp_routing.targetinstall:
#	@$(call targetinfo)
#
#	@$(call install_init, pp_routing)
#	@$(call install_fixup, pp_routing,PRIORITY,optional)
#	@$(call install_fixup, pp_routing,SECTION,base)
#	@$(call install_fixup, pp_routing,AUTHOR,"WAGO GmbH \& Co. KG")
#	@$(call install_fixup, pp_routing,DESCRIPTION,missing)
#
#	@$(call install_finish, pp_routing)
#
#	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_routing.clean:
	@$(call targetinfo)
	@if [ -d $(PP_ROUTING_DIR) ]; then \
		$(PP_ROUTING_MAKE_ENV) $(PP_ROUTING_PATH) $(MAKE) $(MFLAGS) -C $(PP_ROUTING_DIR) clean; \
	fi
	@$(call clean_pkg, PP_ROUTING)
	@rm -rf $(PP_ROUTING_BUILDROOT_DIR)

# vim: syntax=make
