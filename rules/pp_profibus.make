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
PACKAGES-$(PTXCONF_PP_PROFIBUS) += pp_profibus

#
# Paths and names
#
PP_PROFIBUS_VERSION        := 1.0.0
PP_PROFIBUS_MD5            :=
PP_PROFIBUS                := pp_profibus
PP_PROFIBUS_BUILDCONFIG    := Release
PP_PROFIBUS_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_PROFIBUS)
PP_PROFIBUS_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_PROFIBUS)
PP_PROFIBUS_DIR            := $(PP_PROFIBUS_BUILDROOT_DIR)/src
PP_PROFIBUS_BUILD_DIR      := $(PP_PROFIBUS_BUILDROOT_DIR)/bin/$(PP_PROFIBUS_BUILDCONFIG)
PP_PROFIBUS_LICENSE        := WAGO
PP_PROFIBUS_CONF_TOOL      := NO
PP_PROFIBUS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_PROFIBUS_BUILDCONFIG) \
BIN_DIR=$(PP_PROFIBUS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_PROFIBUS_PACKAGE_NAME             := $(PP_PROFIBUS)_$(PP_PROFIBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_PROFIBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_profibus.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_PROFIBUS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_PROFIBUS_DIR) ]; then \
	  ln -s $(PP_PROFIBUS_SRC_DIR) $(PP_PROFIBUS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_profibus.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_PROFIBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_profibus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_PROFIBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_profibus.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_PROFIBUS_PKGDIR) && \
	  tar xvzf $(PP_PROFIBUS_PLATFORMCONFIGPACKAGEDIR)/$(PP_PROFIBUS_PACKAGE_NAME).tgz -C $(PP_PROFIBUS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_PROFIBUS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_PROFIBUS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_PROFIBUS_PKGDIR) && tar cvzf $(PP_PROFIBUS_PLATFORMCONFIGPACKAGEDIR)/$(PP_PROFIBUS_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_profibus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_profibus)
	@$(call install_fixup, pp_profibus,PRIORITY,optional)
	@$(call install_fixup, pp_profibus,SECTION,base)
	@$(call install_fixup, pp_profibus,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_profibus,DESCRIPTION,missing)

	@$(call install_copy, pp_profibus, 0, 0, 0755, -, /usr/sbin/pp_profibus)
	@$(call install_copy, pp_profibus, 0, 0, 0750, -, /etc/init.d/pp_profibus)

	@$(call install_link, pp_profibus, /etc/init.d/pp_profibus, /etc/rc.d/S99_pp_profibus)

	@$(call install_finish, pp_profibus)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_profibus.clean:
	@$(call targetinfo)
	@if [ -d $(PP_PROFIBUS_DIR) ]; then \
		$(PP_PROFIBUS_MAKE_ENV) $(PP_PROFIBUS_PATH) $(MAKE) $(MFLAGS) -C $(PP_PROFIBUS_DIR) clean; \
	fi
	@$(call clean_pkg, PP_PROFIBUS)
	@rm -rf $(PP_PROFIBUS_BUILDROOT_DIR)

# vim: syntax=make
