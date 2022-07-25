# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FP_X509_PKEY) += fp_x509-pkey

#
# Paths and names
#
FP_X509_PKEY_VERSION        := 0.2.0
FP_X509_PKEY_MD5            :=
FP_X509_PKEY                := fp_x509-pkey
FP_X509_PKEY_BUILDCONFIG    := Release
FP_X509_PKEY_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(FP_X509_PKEY)
FP_X509_PKEY_BUILDROOT_DIR  := $(BUILDDIR)/$(FP_X509_PKEY)
FP_X509_PKEY_DIR            := $(FP_X509_PKEY_BUILDROOT_DIR)/src
FP_X509_PKEY_BUILD_DIR      := $(FP_X509_PKEY_BUILDROOT_DIR)/bin/$(FP_X509_PKEY_BUILDCONFIG)
FP_X509_PKEY_LICENSE        := WAGO
FP_X509_PKEY_CONF_TOOL      := NO
FP_X509_PKEY_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(FP_X509_PKEY_BUILDCONFIG) \
BIN_DIR=$(FP_X509_PKEY_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

FP_X509_PKEY_PACKAGE_NAME             := $(FP_X509_PKEY)_$(FP_X509_PKEY_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
FP_X509_PKEY_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/fp_x509-pkey.extract:
	@$(call targetinfo)
	@mkdir -p $(FP_X509_PKEY_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(FP_X509_PKEY_DIR) ]; then \
	  ln -s $(FP_X509_PKEY_SRC_DIR) $(FP_X509_PKEY_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_x509-pkey.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, FP_X509_PKEY)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_x509-pkey.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, FP_X509_PKEY)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_x509-pkey.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(FP_X509_PKEY_PKGDIR) && \
	  tar xvzf $(FP_X509_PKEY_PLATFORMCONFIGPACKAGEDIR)/$(FP_X509_PKEY_PACKAGE_NAME).tgz -C $(FP_X509_PKEY_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, FP_X509_PKEY)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(FP_X509_PKEY_PLATFORMCONFIGPACKAGEDIR)
	@cd $(FP_X509_PKEY_PKGDIR) && tar cvzf $(FP_X509_PKEY_PLATFORMCONFIGPACKAGEDIR)/$(FP_X509_PKEY_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_x509-pkey.targetinstall:
	@$(call targetinfo)
	# Nothing to install on the target
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/fp_x509-pkey.clean:
	@$(call targetinfo)
	@if [ -d $(FP_X509_PKEY_DIR) ]; then \
		$(FP_X509_PKEY_MAKE_ENV) $(FP_X509_PKEY_PATH) $(MAKE) $(MFLAGS) -C $(FP_X509_PKEY_DIR) clean; \
	fi
	@$(call clean_pkg, FP_X509_PKEY)
	@rm -rf $(FP_X509_PKEY_BUILDROOT_DIR)

# vim: syntax=make
