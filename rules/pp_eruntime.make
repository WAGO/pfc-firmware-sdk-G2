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
PACKAGES-$(PTXCONF_PP_ERUNTIME) += pp_eruntime

#
# Paths and names
#
PP_ERUNTIME_VERSION        := 0.1.0
PP_ERUNTIME_MD5            :=
PP_ERUNTIME                := pp_eruntime
PP_ERUNTIME_BUILDCONFIG    := Release
PP_ERUNTIME_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_ERUNTIME)
PP_ERUNTIME_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_ERUNTIME)
PP_ERUNTIME_DIR            := $(PP_ERUNTIME_BUILDROOT_DIR)/src
PP_ERUNTIME_BUILD_DIR      := $(PP_ERUNTIME_BUILDROOT_DIR)/bin/$(PP_ERUNTIME_BUILDCONFIG)
PP_ERUNTIME_LICENSE        := WAGO
PP_ERUNTIME_CONF_TOOL      := NO
PP_ERUNTIME_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_ERUNTIME_BUILDCONFIG) \
BIN_DIR=$(PP_ERUNTIME_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_ERUNTIME_PACKAGE_NAME             := $(PP_ERUNTIME)_$(PP_ERUNTIME_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_ERUNTIME_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_eruntime.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_ERUNTIME_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_ERUNTIME_DIR) ]; then \
	  ln -s $(PP_ERUNTIME_SRC_DIR) $(PP_ERUNTIME_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_eruntime.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_ERUNTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_eruntime.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_ERUNTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_eruntime.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_ERUNTIME_PKGDIR) && \
	  tar xvzf $(PP_ERUNTIME_PLATFORMCONFIGPACKAGEDIR)/$(PP_ERUNTIME_PACKAGE_NAME).tgz -C $(PP_ERUNTIME_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_ERUNTIME)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_ERUNTIME_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_ERUNTIME_PKGDIR) && tar cvzf $(PP_ERUNTIME_PLATFORMCONFIGPACKAGEDIR)/$(PP_ERUNTIME_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_eruntime.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_eruntime)
	@$(call install_fixup, pp_eruntime,PRIORITY,optional)
	@$(call install_fixup, pp_eruntime,SECTION,base)
	@$(call install_fixup, pp_eruntime,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_eruntime,DESCRIPTION,missing)

	@$(call install_copy, pp_eruntime, 0, 0, 0755, -, /usr/sbin/pp_eruntime)
	@$(call install_copy, pp_eruntime, 0, 0, 0750, -, /etc/init.d/pp_eruntime)
	
	@$(call install_link, pp_eruntime, /etc/init.d/pp_eruntime, /etc/rc.d/S98_pp_eruntime)

	@$(call install_finish, pp_eruntime)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_eruntime.clean:
	@$(call targetinfo)
	@if [ -d $(PP_ERUNTIME_DIR) ]; then \
		$(PP_ERUNTIME_MAKE_ENV) $(PP_ERUNTIME_PATH) $(MAKE) $(MFLAGS) -C $(PP_ERUNTIME_DIR) clean; \
	fi
	@$(call clean_pkg, PP_ERUNTIME)
	@rm -rf $(PP_ERUNTIME_BUILDROOT_DIR)

# vim: syntax=make
