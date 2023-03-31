# -*-makefile-*-
#
# Copyright (C) 2020 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PP_DEVICE_INFORMATION) += pp_device-information

#
# Paths and names
#
PP_DEVICE_INFORMATION_VERSION        := 1.0.0
PP_DEVICE_INFORMATION_MD5            :=
PP_DEVICE_INFORMATION                := pp_device-information
PP_DEVICE_INFORMATION_BUILDCONFIG    := Release
PP_DEVICE_INFORMATION_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_DEVICE_INFORMATION)
PP_DEVICE_INFORMATION_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_DEVICE_INFORMATION)
PP_DEVICE_INFORMATION_DIR            := $(PP_DEVICE_INFORMATION_BUILDROOT_DIR)/src
PP_DEVICE_INFORMATION_BUILD_DIR      := $(PP_DEVICE_INFORMATION_BUILDROOT_DIR)/bin/$(PP_DEVICE_INFORMATION_BUILDCONFIG)
PP_DEVICE_INFORMATION_LICENSE        := WAGO
PP_DEVICE_INFORMATION_CONF_TOOL      := NO
PP_DEVICE_INFORMATION_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_DEVICE_INFORMATION_BUILDCONFIG) \
BIN_DIR=$(PP_DEVICE_INFORMATION_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_DEVICE_INFORMATION_PACKAGE_NAME             := $(PP_DEVICE_INFORMATION)_$(PP_DEVICE_INFORMATION_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_DEVICE_INFORMATION_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_device-information.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_DEVICE_INFORMATION_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_DEVICE_INFORMATION_DIR) ]; then \
	  ln -s $(PP_DEVICE_INFORMATION_SRC_DIR) $(PP_DEVICE_INFORMATION_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_device-information.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_DEVICE_INFORMATION)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_device-information.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_DEVICE_INFORMATION)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_device-information.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_DEVICE_INFORMATION_PKGDIR) && \
	  tar xvzf $(PP_DEVICE_INFORMATION_PLATFORMCONFIGPACKAGEDIR)/$(PP_DEVICE_INFORMATION_PACKAGE_NAME).tgz -C $(PP_DEVICE_INFORMATION_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_DEVICE_INFORMATION)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_DEVICE_INFORMATION_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_DEVICE_INFORMATION_PKGDIR) && tar cvzf $(PP_DEVICE_INFORMATION_PLATFORMCONFIGPACKAGEDIR)/$(PP_DEVICE_INFORMATION_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_device-information.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_device-information)
	@$(call install_fixup, pp_device-information,PRIORITY,optional)
	@$(call install_fixup, pp_device-information,SECTION,base)
	@$(call install_fixup, pp_device-information,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_device-information,DESCRIPTION,missing)
	@$(call install_finish, pp_device-information)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_device-information.clean:
	@$(call targetinfo)
	@if [ -d $(PP_DEVICE_INFORMATION_DIR) ]; then \
		$(PP_DEVICE_INFORMATION_MAKE_ENV) $(PP_DEVICE_INFORMATION_PATH) $(MAKE) $(MFLAGS) -C $(PP_DEVICE_INFORMATION_DIR) clean; \
	fi
	@$(call clean_pkg, PP_DEVICE_INFORMATION)
	@rm -rf $(PP_DEVICE_INFORMATION_BUILDROOT_DIR)

# vim: syntax=make
