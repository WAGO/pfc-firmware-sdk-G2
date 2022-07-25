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
PACKAGES-$(PTXCONF_PP_MODBUS) += pp_modbus

#
# Paths and names
#
PP_MODBUS_VERSION        := 0.1.0
PP_MODBUS_MD5            :=
PP_MODBUS                := pp_modbus
PP_MODBUS_BUILDCONFIG    := Release
PP_MODBUS_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_MODBUS)
PP_MODBUS_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_MODBUS)
PP_MODBUS_DIR            := $(PP_MODBUS_BUILDROOT_DIR)/src
PP_MODBUS_BUILD_DIR      := $(PP_MODBUS_BUILDROOT_DIR)/bin/$(PP_MODBUS_BUILDCONFIG)
PP_MODBUS_LICENSE        := WAGO
PP_MODBUS_CONF_TOOL      := NO
PP_MODBUS_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_MODBUS_BUILDCONFIG) \
BIN_DIR=$(PP_MODBUS_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_MODBUS_PACKAGE_NAME             := $(PP_MODBUS)_$(PP_MODBUS_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_MODBUS_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_modbus.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_MODBUS_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_MODBUS_DIR) ]; then \
	  ln -s $(PP_MODBUS_SRC_DIR) $(PP_MODBUS_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modbus.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_MODBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modbus.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_MODBUS)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modbus.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_MODBUS_PKGDIR) && \
	  tar xvzf $(PP_MODBUS_PLATFORMCONFIGPACKAGEDIR)/$(PP_MODBUS_PACKAGE_NAME).tgz -C $(PP_MODBUS_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_MODBUS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_MODBUS_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_MODBUS_PKGDIR) && tar cvzf $(PP_MODBUS_PLATFORMCONFIGPACKAGEDIR)/$(PP_MODBUS_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modbus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_modbus)
	@$(call install_fixup, pp_modbus,PRIORITY,optional)
	@$(call install_fixup, pp_modbus,SECTION,base)
	@$(call install_fixup, pp_modbus,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_modbus,DESCRIPTION,missing)

	@$(call install_copy, pp_modbus, 0, 0, 0755, -, /usr/bin/pp_modbus)
	@$(call install_copy, pp_modbus, 0, 0, 0700, -, /etc/init.d/pp_modbus)
	
	@$(call install_link, pp_modbus, /etc/init.d/pp_modbus, /etc/rc.d/S98_pp_modbus)

	@$(call install_finish, pp_modbus)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_modbus.clean:
	@$(call targetinfo)
	@if [ -d $(PP_MODBUS_DIR) ]; then \
		$(PP_MODBUS_MAKE_ENV) $(PP_MODBUS_PATH) $(MAKE) $(MFLAGS) -C $(PP_MODBUS_DIR) clean; \
	fi
	@$(call clean_pkg, PP_MODBUS)
	@rm -rf $(PP_MODBUS_BUILDROOT_DIR)

# vim: syntax=make
