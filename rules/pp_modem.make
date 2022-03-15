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
PACKAGES-$(PTXCONF_PP_MODEM) += pp_modem

#
# Paths and names
#
PP_MODEM_VERSION        := 1.0.0
PP_MODEM_MD5            :=
PP_MODEM                := pp_modem
PP_MODEM_BUILDCONFIG    := Release
PP_MODEM_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_MODEM)
PP_MODEM_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_MODEM)
PP_MODEM_DIR            := $(PP_MODEM_BUILDROOT_DIR)/src
PP_MODEM_BUILD_DIR      := $(PP_MODEM_BUILDROOT_DIR)/bin/$(PP_MODEM_BUILDCONFIG)
PP_MODEM_LICENSE        := WAGO
PP_MODEM_CONF_TOOL      := NO
PP_MODEM_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_MODEM_BUILDCONFIG) \
BIN_DIR=$(PP_MODEM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_MODEM_PACKAGE_NAME             := $(PP_MODEM)_$(PP_MODEM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_MODEM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_modem.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_MODEM_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_MODEM_DIR) ]; then \
	  ln -s $(PP_MODEM_SRC_DIR) $(PP_MODEM_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modem.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_MODEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modem.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_MODEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modem.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_MODEM_PKGDIR) && \
	  tar xvzf $(PP_MODEM_PLATFORMCONFIGPACKAGEDIR)/$(PP_MODEM_PACKAGE_NAME).tgz -C $(PP_MODEM_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_MODEM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_MODEM_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_MODEM_PKGDIR) && tar cvzf $(PP_MODEM_PLATFORMCONFIGPACKAGEDIR)/$(PP_MODEM_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modem.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_modem)
	@$(call install_fixup, pp_modem,PRIORITY,optional)
	@$(call install_fixup, pp_modem,SECTION,base)
	@$(call install_fixup, pp_modem,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_modem,DESCRIPTION,missing)

	@$(call install_copy, pp_modem, 0, 0, 0755, -, /usr/sbin/pp_modem)
	@$(call install_copy, pp_modem, 0, 0, 0750, -, /etc/init.d/pp_modem)
	
	@$(call install_link, pp_modem, /etc/init.d/pp_modem, /etc/rc.d/S99_pp_modem)

	@$(call install_finish, pp_modem)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modem.clean:
	@$(call targetinfo)
	@if [ -d $(PP_MODEM_DIR) ]; then \
		$(PP_MODEM_MAKE_ENV) $(PP_MODEM_PATH) $(MAKE) $(MFLAGS) -C $(PP_MODEM_DIR) clean; \
	fi
	@$(call clean_pkg, PP_MODEM)
	@rm -rf $(PP_MODEM_BUILDROOT_DIR)

# vim: syntax=make
