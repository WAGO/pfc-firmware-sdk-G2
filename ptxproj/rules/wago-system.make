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
PACKAGES-$(PTXCONF_WAGO_SYSTEM) += wago-system

#
# Paths and names
#
WAGO_SYSTEM_VERSION        := 0.1.0
WAGO_SYSTEM_MD5            :=
WAGO_SYSTEM                := wago-system
WAGO_SYSTEM_BUILDCONFIG    := Release
WAGO_SYSTEM_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(WAGO_SYSTEM)
WAGO_SYSTEM_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_SYSTEM)
WAGO_SYSTEM_DIR            := $(WAGO_SYSTEM_BUILDROOT_DIR)/src
WAGO_SYSTEM_BUILD_DIR      := $(WAGO_SYSTEM_BUILDROOT_DIR)/bin/$(WAGO_SYSTEM_BUILDCONFIG)
WAGO_SYSTEM_LICENSE        := WAGO
WAGO_SYSTEM_CONF_TOOL      := NO
WAGO_SYSTEM_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_SYSTEM_BUILDCONFIG) \
BIN_DIR=$(WAGO_SYSTEM_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

WAGO_SYSTEM_PACKAGE_NAME             := $(WAGO_SYSTEM)_$(WAGO_SYSTEM_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_SYSTEM_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-system.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_SYSTEM_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_SYSTEM_DIR) ]; then \
	  ln -s $(WAGO_SYSTEM_SRC_DIR) $(WAGO_SYSTEM_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-system.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WAGO_SYSTEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-system.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WAGO_SYSTEM)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-system.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(WAGO_SYSTEM_PKGDIR) && \
	  tar xvzf $(WAGO_SYSTEM_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_SYSTEM_PACKAGE_NAME).tgz -C $(WAGO_SYSTEM_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, WAGO_SYSTEM)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_SYSTEM_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WAGO_SYSTEM_PKGDIR) && tar cvzf $(WAGO_SYSTEM_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_SYSTEM_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-system.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-system)
	@$(call install_fixup, wago-system,PRIORITY,optional)
	@$(call install_fixup, wago-system,SECTION,base)
	@$(call install_fixup, wago-system,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-system,DESCRIPTION,missing)

	#@$(call install_lib, wago-system, 0, 0, 0644, libwago-system)
	#@$(call install_copy, wago-system, 0, 0, 0755, -, /usr/bin/wago-system)

	@$(call install_finish, wago-system)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-system.clean:
	@$(call targetinfo)
	@if [ -d $(WAGO_SYSTEM_DIR) ]; then \
		$(WAGO_SYSTEM_MAKE_ENV) $(WAGO_SYSTEM_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_SYSTEM_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_SYSTEM)
	@rm -rf $(WAGO_SYSTEM_BUILDROOT_DIR)

# vim: syntax=make
