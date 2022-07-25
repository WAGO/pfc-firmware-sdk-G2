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
PACKAGES-$(PTXCONF_FP_REGULAR_FILE) += fp_regular-file

#
# Paths and names
#
FP_REGULAR_FILE_VERSION        := 0.1.0
FP_REGULAR_FILE_MD5            :=
FP_REGULAR_FILE                := fp_regular-file
FP_REGULAR_FILE_BUILDCONFIG    := Release
FP_REGULAR_FILE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(FP_REGULAR_FILE)
FP_REGULAR_FILE_BUILDROOT_DIR  := $(BUILDDIR)/$(FP_REGULAR_FILE)
FP_REGULAR_FILE_DIR            := $(FP_REGULAR_FILE_BUILDROOT_DIR)/src
FP_REGULAR_FILE_BUILD_DIR      := $(FP_REGULAR_FILE_BUILDROOT_DIR)/bin/$(FP_REGULAR_FILE_BUILDCONFIG)
FP_REGULAR_FILE_LICENSE        := WAGO
FP_REGULAR_FILE_CONF_TOOL      := NO
FP_REGULAR_FILE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(FP_REGULAR_FILE_BUILDCONFIG) \
BIN_DIR=$(FP_REGULAR_FILE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

FP_REGULAR_FILE_PACKAGE_NAME             := $(FP_REGULAR_FILE)_$(FP_REGULAR_FILE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
FP_REGULAR_FILE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/fp_regular-file.extract: 
	@$(call targetinfo)
	@mkdir -p $(FP_REGULAR_FILE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(FP_REGULAR_FILE_DIR) ]; then \
	  ln -s $(FP_REGULAR_FILE_SRC_DIR) $(FP_REGULAR_FILE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_regular-file.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, FP_REGULAR_FILE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_regular-file.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, FP_REGULAR_FILE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_regular-file.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(FP_REGULAR_FILE_PKGDIR) && \
	  tar xvzf $(FP_REGULAR_FILE_PLATFORMCONFIGPACKAGEDIR)/$(FP_REGULAR_FILE_PACKAGE_NAME).tgz -C $(FP_REGULAR_FILE_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, FP_REGULAR_FILE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(FP_REGULAR_FILE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(FP_REGULAR_FILE_PKGDIR) && tar cvzf $(FP_REGULAR_FILE_PLATFORMCONFIGPACKAGEDIR)/$(FP_REGULAR_FILE_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_regular-file.targetinstall:
	@$(call targetinfo)
	# Nothing to install on the target
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_regular-file.clean:
	@$(call targetinfo)
	@if [ -d $(FP_REGULAR_FILE_DIR) ]; then \
		$(FP_REGULAR_FILE_MAKE_ENV) $(FP_REGULAR_FILE_PATH) $(MAKE) $(MFLAGS) -C $(FP_REGULAR_FILE_DIR) clean; \
	fi
	@$(call clean_pkg, FP_REGULAR_FILE)
	@rm -rf $(FP_REGULAR_FILE_BUILDROOT_DIR)

# vim: syntax=make
