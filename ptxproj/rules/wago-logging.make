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
PACKAGES-$(PTXCONF_WAGO_LOGGING) += wago-logging

#
# Paths and names
#
WAGO_LOGGING_VERSION        := 1.0.1
WAGO_LOGGING_MD5            :=
WAGO_LOGGING                := wago-logging
WAGO_LOGGING_BUILDCONFIG    := Release
WAGO_LOGGING_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(WAGO_LOGGING))
WAGO_LOGGING_BUILDROOT_DIR  := $(BUILDDIR)/$(WAGO_LOGGING)
WAGO_LOGGING_DIR            := $(WAGO_LOGGING_BUILDROOT_DIR)/src
WAGO_LOGGING_BUILD_DIR      := $(WAGO_LOGGING_BUILDROOT_DIR)/bin/$(WAGO_LOGGING_BUILDCONFIG)
WAGO_LOGGING_LICENSE        := WAGO
WAGO_LOGGING_CONF_TOOL      := NO
WAGO_LOGGING_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(WAGO_LOGGING_BUILDCONFIG) \
BIN_DIR=$(WAGO_LOGGING_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/wago-logging.make)

WAGO_LOGGING_PACKAGE_NAME             := $(WAGO_LOGGING)_$(WAGO_LOGGING_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WAGO_LOGGING_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/wago-logging.extract: 
	@$(call targetinfo)
	@mkdir -p $(WAGO_LOGGING_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(WAGO_LOGGING_DIR) ]; then \
	  ln -s $(WAGO_LOGGING_SRC_DIR) $(WAGO_LOGGING_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-logging.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, WAGO_LOGGING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-logging.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, WAGO_LOGGING)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-logging.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(WAGO_LOGGING_PKGDIR) && \
	  tar xvzf $(WAGO_LOGGING_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_LOGGING_PACKAGE_NAME).tgz -C $(WAGO_LOGGING_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, WAGO_LOGGING)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(WAGO_LOGGING_PLATFORMCONFIGPACKAGEDIR)
	@cd $(WAGO_LOGGING_PKGDIR) && tar cvzf $(WAGO_LOGGING_PLATFORMCONFIGPACKAGEDIR)/$(WAGO_LOGGING_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-logging.targetinstall:
	@$(call targetinfo)
	# Nothing to install on the target
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wago-logging.clean:
	@$(call targetinfo)
	@if [ -d $(WAGO_LOGGING_DIR) ]; then \
		$(WAGO_LOGGING_MAKE_ENV) $(WAGO_LOGGING_PATH) $(MAKE) $(MFLAGS) -C $(WAGO_LOGGING_DIR) clean; \
	fi
	@$(call clean_pkg, WAGO_LOGGING)
	@rm -rf $(WAGO_LOGGING_BUILDROOT_DIR)

# vim: syntax=make
