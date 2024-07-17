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
PACKAGES-$(PTXCONF_WAGO_OPTPARSING) += wago-optparsing

#
# Paths and names
#
WAGO_OPTPARSING_VERSION        := 1.0.1
WAGO_OPTPARSING_MD5            :=
WAGO_OPTPARSING                := wago-optparsing
WAGO_OPTPARSING_BUILDCONFIG    := Release
WAGO_OPTPARSING_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WAGO_OPTPARSING))
WAGO_OPTPARSING_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_OPTPARSING)
WAGO_OPTPARSING_DIR            := $(WAGO_OPTPARSING_BUILDROOT_DIR)/src
WAGO_OPTPARSING_BUILD_DIR      := $(WAGO_OPTPARSING_BUILDROOT_DIR)/bin/$(WAGO_OPTPARSING_BUILDCONFIG)
WAGO_OPTPARSING_LICENSE        := WAGO
WAGO_OPTPARSING_CONF_TOOL      := NO
WAGO_OPTPARSING_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_OPTPARSING_BUILDCONFIG) \
BIN_DIR=$(WAGO_OPTPARSING_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wago-optparsing.make)

WAGO_OPTPARSING_PACKAGE_NAME             := $(WAGO_OPTPARSING)_$(WAGO_OPTPARSING_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_OPTPARSING_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-optparsing.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_OPTPARSING_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_OPTPARSING_DIR) ]; then \
	  ln -s $(WAGO_OPTPARSING_SRC_DIR) $(WAGO_OPTPARSING_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-optparsing.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WAGO_OPTPARSING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-optparsing.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WAGO_OPTPARSING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-optparsing.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(WAGO_OPTPARSING_PKGDIR) && \
	  tar xvzf $(WAGO_OPTPARSING_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_OPTPARSING_PACKAGE_NAME).tgz -C $(WAGO_OPTPARSING_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, WAGO_OPTPARSING)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_OPTPARSING_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WAGO_OPTPARSING_PKGDIR) && tar cvzf $(WAGO_OPTPARSING_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_OPTPARSING_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-optparsing.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wago-optparsing)
	@$(call install_fixup, wago-optparsing,PRIORITY,optional)
	@$(call install_fixup, wago-optparsing,SECTION,base)
	@$(call install_fixup, wago-optparsing,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, wago-optparsing,DESCRIPTION,missing)

	@$(call install_lib, wago-optparsing, 0, 0, 0644, libwagooptparsing)

	@$(call install_finish, wago-optparsing)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-optparsing.clean:
	@$(call targetinfo)
	@if [ -d $(WAGO_OPTPARSING_DIR) ]; then \
		$(WAGO_OPTPARSING_MAKE_ENV) $(WAGO_OPTPARSING_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_OPTPARSING_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_OPTPARSING)
	@rm -rf $(WAGO_OPTPARSING_BUILDROOT_DIR)

# vim: syntax=make
