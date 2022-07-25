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
PACKAGES-$(PTXCONF_TSCPPERUNTIMEAPP) += tscpperuntimeapp

#
# Paths and names
#
TSCPPERUNTIMEAPP_VERSION        := 0.1.0
TSCPPERUNTIMEAPP_MD5            :=
TSCPPERUNTIMEAPP                := TscPPERuntimeApp
TSCPPERUNTIMEAPP_BUILDCONFIG    := Release
TSCPPERUNTIMEAPP_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(TSCPPERUNTIMEAPP)
TSCPPERUNTIMEAPP_BUILDROOT_DIR  := $(BUILDDIR)/$(TSCPPERUNTIMEAPP)
TSCPPERUNTIMEAPP_DIR            := $(TSCPPERUNTIMEAPP_BUILDROOT_DIR)/src
TSCPPERUNTIMEAPP_BUILD_DIR      := $(TSCPPERUNTIMEAPP_BUILDROOT_DIR)/bin/$(TSCPPERUNTIMEAPP_BUILDCONFIG)
TSCPPERUNTIMEAPP_LICENSE        := WAGO
TSCPPERUNTIMEAPP_CONF_TOOL      := NO
TSCPPERUNTIMEAPP_SO_NAME        := lib$(TSCPPERUNTIMEAPP).so
TSCPPERUNTIMEAPP_BIN            := $(TSCPPERUNTIMEAPP_SO_NAME).$(TSCPPERUNTIMEAPP_VERSION)
TSCPPERUNTIMEAPP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(TSCPPERUNTIMEAPP_BUILDCONFIG) \
BIN_DIR=$(TSCPPERUNTIMEAPP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

TSCPPERUNTIMEAPP_PACKAGE_NAME             := $(TSCPPERUNTIMEAPP)_$(TSCPPERUNTIMEAPP_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
TSCPPERUNTIMEAPP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/tscpperuntimeapp.extract: 
	@$(call targetinfo)
	@mkdir -p $(TSCPPERUNTIMEAPP_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(TSCPPERUNTIMEAPP_DIR) ]; then \
	  ln -s $(TSCPPERUNTIMEAPP_SRC_DIR) $(TSCPPERUNTIMEAPP_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/tscpperuntimeapp.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, TSCPPERUNTIMEAPP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/tscpperuntimeapp.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, TSCPPERUNTIMEAPP)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tscpperuntimeapp.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(TSCPPERUNTIMEAPP_PKGDIR) && \
	  tar xvzf $(TSCPPERUNTIMEAPP_PLATFORMCONFIGPACKAGEDIR)/$(TSCPPERUNTIMEAPP_PACKAGE_NAME).tgz -C $(TSCPPERUNTIMEAPP_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, TSCPPERUNTIMEAPP)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(TSCPPERUNTIMEAPP_PLATFORMCONFIGPACKAGEDIR)
	@cd $(TSCPPERUNTIMEAPP_PKGDIR) && tar cvzf $(TSCPPERUNTIMEAPP_PLATFORMCONFIGPACKAGEDIR)/$(TSCPPERUNTIMEAPP_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tscpperuntimeapp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tscpperuntimeapp)
	@$(call install_fixup, tscpperuntimeapp,PRIORITY,optional)
	@$(call install_fixup, tscpperuntimeapp,SECTION,base)
	@$(call install_fixup, tscpperuntimeapp,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, tscpperuntimeapp,DESCRIPTION,missing)

	@$(call install_lib, tscpperuntimeapp, 0, 0, 0644, libTscPPERuntimeApp)
	@$(call install_link, tscpperuntimeapp, ../$(TSCPPERUNTIMEAPP_BIN), /usr/lib/cds3-custom-components/$(TSCPPERUNTIMEAPP_SO_NAME))

	@$(call install_finish, tscpperuntimeapp)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/tscpperuntimeapp.clean:
	@$(call targetinfo)
	@if [ -d $(TSCPPERUNTIMEAPP_DIR) ]; then \
		$(TSCPPERUNTIMEAPP_MAKE_ENV) $(TSCPPERUNTIMEAPP_PATH) $(MAKE) $(MFLAGS) -C $(TSCPPERUNTIMEAPP_DIR) clean; \
	fi
	@$(call clean_pkg, TSCPPERUNTIMEAPP)
	@rm -rf $(TSCPPERUNTIMEAPP_BUILDROOT_DIR)

# vim: syntax=make
