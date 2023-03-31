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
PACKAGES-$(PTXCONF_PP_SNMP) += pp_snmp

#
# Paths and names
#
PP_SNMP_VERSION        := 1.0.0
PP_SNMP_MD5            :=
PP_SNMP                := pp_snmp
PP_SNMP_BUILDCONFIG    := Release
PP_SNMP_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SNMP)
PP_SNMP_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SNMP)
PP_SNMP_DIR            := $(PP_SNMP_BUILDROOT_DIR)/src
PP_SNMP_BUILD_DIR      := $(PP_SNMP_BUILDROOT_DIR)/bin/$(PP_SNMP_BUILDCONFIG)
PP_SNMP_LICENSE        := WAGO
PP_SNMP_CONF_TOOL      := NO
PP_SNMP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SNMP_BUILDCONFIG) \
BIN_DIR=$(PP_SNMP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SNMP_PACKAGE_NAME             := $(PP_SNMP)_$(PP_SNMP_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SNMP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_snmp.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_SNMP_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SNMP_DIR) ]; then \
	  ln -s $(PP_SNMP_SRC_DIR) $(PP_SNMP_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_snmp.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SNMP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_snmp.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SNMP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_snmp.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SNMP_PKGDIR) && \
	  tar xvzf $(PP_SNMP_PLATFORMCONFIGPACKAGEDIR)/$(PP_SNMP_PACKAGE_NAME).tgz -C $(PP_SNMP_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SNMP)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SNMP_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SNMP_PKGDIR) && tar cvzf $(PP_SNMP_PLATFORMCONFIGPACKAGEDIR)/$(PP_SNMP_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_snmp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_snmp)
	@$(call install_fixup, pp_snmp,PRIORITY,optional)
	@$(call install_fixup, pp_snmp,SECTION,base)
	@$(call install_fixup, pp_snmp,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_snmp,DESCRIPTION,missing)

	@$(call install_copy, pp_snmp, 0, 0, 0755, -, /usr/bin/pp_snmp)
	@$(call install_copy, pp_snmp, 0, 0, 0755, -, /etc/init.d/pp_snmp)
	@$(call install_link, pp_snmp, ../init.d/pp_snmp, /etc/rc.d/S99_pp_snmp)	

	@$(call install_finish, pp_snmp)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_snmp.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SNMP_DIR) ]; then \
		$(PP_SNMP_MAKE_ENV) $(PP_SNMP_PATH) $(MAKE) $(MFLAGS) -C $(PP_SNMP_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SNMP)
	@rm -rf $(PP_SNMP_BUILDROOT_DIR)

# vim: syntax=make
