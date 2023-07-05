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
PACKAGES-$(PTXCONF_LIBWAGOAPPBR) += libwagoappbr

#
# Paths and names
#
LIBWAGOAPPBR_VERSION        := 0.1.0
LIBWAGOAPPBR_MD5            :=
LIBWAGOAPPBR                := libwagoappbr
LIBWAGOAPPBR_BUILDCONFIG    := Release
LIBWAGOAPPBR_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(LIBWAGOAPPBR)
LIBWAGOAPPBR_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBWAGOAPPBR)
LIBWAGOAPPBR_DIR            := $(LIBWAGOAPPBR_BUILDROOT_DIR)/src
LIBWAGOAPPBR_BUILD_DIR      := $(LIBWAGOAPPBR_BUILDROOT_DIR)/bin/$(LIBWAGOAPPBR_BUILDCONFIG)
LIBWAGOAPPBR_LICENSE        := WAGO
LIBWAGOAPPBR_CONF_TOOL      := NO
LIBWAGOAPPBR_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBWAGOAPPBR_BUILDCONFIG) \
BIN_DIR=$(LIBWAGOAPPBR_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBWAGOAPPBR_PACKAGE_NAME             := $(LIBWAGOAPPBR)_$(LIBWAGOAPPBR_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWAGOAPPBR_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/libwagoappbr.extract: 
	@$(call targetinfo)
	@mkdir -p $(LIBWAGOAPPBR_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBWAGOAPPBR_DIR) ]; then \
	  ln -s $(LIBWAGOAPPBR_SRC_DIR) $(LIBWAGOAPPBR_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagoappbr.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LIBWAGOAPPBR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagoappbr.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBWAGOAPPBR)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagoappbr.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(LIBWAGOAPPBR_PKGDIR) && \
	  tar xvzf $(LIBWAGOAPPBR_PLATFORMCONFIGPACKAGEDIR)/$(LIBWAGOAPPBR_PACKAGE_NAME).tgz -C $(LIBWAGOAPPBR_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, LIBWAGOAPPBR)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(LIBWAGOAPPBR_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBWAGOAPPBR_PKGDIR) && tar cvzf $(LIBWAGOAPPBR_PLATFORMCONFIGPACKAGEDIR)/$(LIBWAGOAPPBR_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagoappbr.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libwagoappbr)
	@$(call install_fixup, libwagoappbr,PRIORITY,optional)
	@$(call install_fixup, libwagoappbr,SECTION,base)
	@$(call install_fixup, libwagoappbr,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, libwagoappbr,DESCRIPTION,missing)

#	@$(call install_lib, libwagoappbr, 0, 0, 0644, liblibwagoappbr)
	@$(call install_lib, libwagoappbr, 0, 0, 0644, libwagoappbr)
	@$(call install_finish, libwagoappbr)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libwagoappbr.clean:
	@$(call targetinfo)
	@if [ -d $(LIBWAGOAPPBR_DIR) ]; then \
		$(LIBWAGOAPPBR_MAKE_ENV) $(LIBWAGOAPPBR_PATH) $(MAKE) $(MFLAGS) -C $(LIBWAGOAPPBR_DIR) clean; \
	fi
	@$(call clean_pkg, LIBWAGOAPPBR)
	@rm -rf $(LIBWAGOAPPBR_BUILDROOT_DIR)

# vim: syntax=make
