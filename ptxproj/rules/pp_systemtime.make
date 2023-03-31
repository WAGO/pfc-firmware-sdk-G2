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
PACKAGES-$(PTXCONF_PP_SYSTEMTIME) += pp_systemtime

#
# Paths and names
#
PP_SYSTEMTIME_VERSION        := 1.0.0
PP_SYSTEMTIME_MD5            :=
PP_SYSTEMTIME                := pp_systemtime
PP_SYSTEMTIME_BUILDCONFIG    := Release
PP_SYSTEMTIME_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SYSTEMTIME)
PP_SYSTEMTIME_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SYSTEMTIME)
PP_SYSTEMTIME_DIR            := $(PP_SYSTEMTIME_BUILDROOT_DIR)/src
PP_SYSTEMTIME_BUILD_DIR      := $(PP_SYSTEMTIME_BUILDROOT_DIR)/bin/$(PP_SYSTEMTIME_BUILDCONFIG)
PP_SYSTEMTIME_LICENSE        := WAGO
PP_SYSTEMTIME_CONF_TOOL      := NO
PP_SYSTEMTIME_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SYSTEMTIME_BUILDCONFIG) \
BIN_DIR=$(PP_SYSTEMTIME_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SYSTEMTIME_PACKAGE_NAME             := $(PP_SYSTEMTIME)_$(PP_SYSTEMTIME_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SYSTEMTIME_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_systemtime.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_SYSTEMTIME_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SYSTEMTIME_DIR) ]; then \
	  ln -s $(PP_SYSTEMTIME_SRC_DIR) $(PP_SYSTEMTIME_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_systemtime.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SYSTEMTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_systemtime.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SYSTEMTIME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_systemtime.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SYSTEMTIME_PKGDIR) && \
	  tar xvzf $(PP_SYSTEMTIME_PLATFORMCONFIGPACKAGEDIR)/$(PP_SYSTEMTIME_PACKAGE_NAME).tgz -C $(PP_SYSTEMTIME_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SYSTEMTIME)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SYSTEMTIME_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SYSTEMTIME_PKGDIR) && tar cvzf $(PP_SYSTEMTIME_PLATFORMCONFIGPACKAGEDIR)/$(PP_SYSTEMTIME_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_systemtime.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_systemtime)
	@$(call install_fixup, pp_systemtime,PRIORITY,optional)
	@$(call install_fixup, pp_systemtime,SECTION,base)
	@$(call install_fixup, pp_systemtime,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_systemtime,DESCRIPTION,missing)

	@$(call install_copy, pp_systemtime, 0, 0, 0750, -, /etc/config-tools/backup-restore/backup_custom_tzstring)
	@$(call install_finish, pp_systemtime)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_systemtime.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SYSTEMTIME_DIR) ]; then \
		$(PP_SYSTEMTIME_MAKE_ENV) $(PP_SYSTEMTIME_PATH) $(MAKE) $(MFLAGS) -C $(PP_SYSTEMTIME_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SYSTEMTIME)
	@rm -rf $(PP_SYSTEMTIME_BUILDROOT_DIR)

# vim: syntax=make
