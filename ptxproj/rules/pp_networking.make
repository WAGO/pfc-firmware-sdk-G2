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
PACKAGES-$(PTXCONF_PP_NETWORKING) += pp_networking

#
# Paths and names
#
PP_NETWORKING_VERSION        := 1.1.0
PP_NETWORKING_MD5            :=
PP_NETWORKING                := pp_networking
PP_NETWORKING_BUILDCONFIG    := Release
PP_NETWORKING_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_NETWORKING)
PP_NETWORKING_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_NETWORKING)
PP_NETWORKING_DIR            := $(PP_NETWORKING_BUILDROOT_DIR)/src
PP_NETWORKING_BUILD_DIR      := $(PP_NETWORKING_BUILDROOT_DIR)/bin/$(PP_NETWORKING_BUILDCONFIG)
PP_NETWORKING_LICENSE        := WAGO
PP_NETWORKING_CONF_TOOL      := NO
PP_NETWORKING_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_NETWORKING_BUILDCONFIG) \
BIN_DIR=$(PP_NETWORKING_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_NETWORKING_PACKAGE_NAME             := $(PP_NETWORKING)_$(PP_NETWORKING_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_NETWORKING_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_networking.extract:
	@$(call targetinfo)
	@mkdir -p $(PP_NETWORKING_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_NETWORKING_DIR) ]; then \
	  ln -s $(PP_NETWORKING_SRC_DIR) $(PP_NETWORKING_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networking.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_NETWORKING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networking.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_NETWORKING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networking.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_NETWORKING_PKGDIR) && \
	  tar xvzf $(PP_NETWORKING_PLATFORMCONFIGPACKAGEDIR)/$(PP_NETWORKING_PACKAGE_NAME).tgz -C $(PP_NETWORKING_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_NETWORKING)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_NETWORKING_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_NETWORKING_PKGDIR) && tar cvzf $(PP_NETWORKING_PLATFORMCONFIGPACKAGEDIR)/$(PP_NETWORKING_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networking.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_networking)
	@$(call install_fixup, pp_networking,PRIORITY,optional)
	@$(call install_fixup, pp_networking,SECTION,base)
	@$(call install_fixup, pp_networking,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_networking,DESCRIPTION,missing)

	@$(call install_copy, pp_networking, 0, 0, 0755, -, /usr/bin/pp_networking)
	@$(call install_copy, pp_networking, 0, 0, 0755, -, /etc/init.d/pp_network)
	@$(call install_link, pp_networking, ../init.d/pp_network, /etc/rc.d/S90_pp_network)
	@$(call install_copy, pp_networking, 0, 0, 0755, -, /etc/init.d/pp_routing)
	@$(call install_link, pp_networking, ../init.d/pp_routing, /etc/rc.d/S90_pp_routing)
	@$(call install_copy, pp_networking, 0, 0, 0755, -, /etc/init.d/pp_hostdomainname)
	@$(call install_link, pp_networking, ../init.d/pp_hostdomainname, /etc/rc.d/S90_pp_hostdomainname)

	@$(call install_finish, pp_networking)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_networking.clean:
	@$(call targetinfo)
	@if [ -d $(PP_NETWORKING_DIR) ]; then \
		$(PP_NETWORKING_MAKE_ENV) $(PP_NETWORKING_PATH) $(MAKE) $(MFLAGS) -C $(PP_NETWORKING_DIR) clean; \
	fi
	@$(call clean_pkg, PP_NETWORKING)
	@rm -rf $(PP_NETWORKING_BUILDROOT_DIR)

# vim: syntax=make
