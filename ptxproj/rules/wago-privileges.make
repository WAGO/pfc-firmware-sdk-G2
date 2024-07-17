# -*-makefile-*-
#
# Copyright (C) 2023 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WAGO_PRIVILEGES) += wago-privileges

#
# Paths and names
#
WAGO_PRIVILEGES_VERSION        := 1.0.1
WAGO_PRIVILEGES_MD5            :=
WAGO_PRIVILEGES                := wago-privileges
WAGO_PRIVILEGES_BUILDCONFIG    := Release
WAGO_PRIVILEGES_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WAGO_PRIVILEGES))
WAGO_PRIVILEGES_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_PRIVILEGES)
WAGO_PRIVILEGES_DIR            := $(WAGO_PRIVILEGES_BUILDROOT_DIR)/src
WAGO_PRIVILEGES_BUILD_DIR      := $(WAGO_PRIVILEGES_BUILDROOT_DIR)/bin/$(WAGO_PRIVILEGES_BUILDCONFIG)
WAGO_PRIVILEGES_LICENSE        := WAGO
WAGO_PRIVILEGES_CONF_TOOL      := NO
WAGO_PRIVILEGES_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_PRIVILEGES_BUILDCONFIG) \
BIN_DIR=$(WAGO_PRIVILEGES_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wago-privileges.make)

WAGO_PRIVILEGES_PACKAGE_NAME             := $(WAGO_PRIVILEGES)_$(WAGO_PRIVILEGES_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_PRIVILEGES_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-privileges.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_PRIVILEGES_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_PRIVILEGES_DIR) ]; then \
	  ln -s $(WAGO_PRIVILEGES_SRC_DIR) $(WAGO_PRIVILEGES_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-privileges.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WAGO_PRIVILEGES)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-privileges.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WAGO_PRIVILEGES)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-privileges.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(WAGO_PRIVILEGES_PKGDIR) && \
	  tar xvzf $(WAGO_PRIVILEGES_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PRIVILEGES_PACKAGE_NAME).tgz -C $(WAGO_PRIVILEGES_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, WAGO_PRIVILEGES)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_PRIVILEGES_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WAGO_PRIVILEGES_PKGDIR) && tar cvzf $(WAGO_PRIVILEGES_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_PRIVILEGES_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-privileges.targetinstall:
	@$(call targetinfo)
	# Nothing to install on the target
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-privileges.clean:
	@$(call targetinfo)
	@if [ -d $(WAGO_PRIVILEGES_DIR) ]; then \
		$(WAGO_PRIVILEGES_MAKE_ENV) $(WAGO_PRIVILEGES_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_PRIVILEGES_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_PRIVILEGES)
	@rm -rf $(WAGO_PRIVILEGES_BUILDROOT_DIR)

# vim: syntax=make
