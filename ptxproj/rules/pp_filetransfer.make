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
PACKAGES-$(PTXCONF_PP_FILETRANSFER) += pp_filetransfer

#
# Paths and names
#
PP_FILETRANSFER_VERSION        := 1.0.0
PP_FILETRANSFER_MD5            :=
PP_FILETRANSFER                := pp_filetransfer
PP_FILETRANSFER_BUILDCONFIG    := Release
PP_FILETRANSFER_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_FILETRANSFER)
PP_FILETRANSFER_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_FILETRANSFER)
PP_FILETRANSFER_DIR            := $(PP_FILETRANSFER_BUILDROOT_DIR)/src
PP_FILETRANSFER_BUILD_DIR      := $(PP_FILETRANSFER_BUILDROOT_DIR)/bin/$(PP_FILETRANSFER_BUILDCONFIG)
PP_FILETRANSFER_LICENSE        := WAGO
PP_FILETRANSFER_CONF_TOOL      := NO
PP_FILETRANSFER_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_FILETRANSFER_BUILDCONFIG) \
BIN_DIR=$(PP_FILETRANSFER_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_FILETRANSFER_PACKAGE_NAME             := $(PP_FILETRANSFER)_$(PP_FILETRANSFER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_FILETRANSFER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_filetransfer.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_FILETRANSFER_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_FILETRANSFER_DIR) ]; then \
	  ln -s $(PP_FILETRANSFER_SRC_DIR) $(PP_FILETRANSFER_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_filetransfer.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_FILETRANSFER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_filetransfer.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_FILETRANSFER)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_filetransfer.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_FILETRANSFER_PKGDIR) && \
	  tar xvzf $(PP_FILETRANSFER_PLATFORMCONFIGPACKAGEDIR)/$(PP_FILETRANSFER_PACKAGE_NAME).tgz -C $(PP_FILETRANSFER_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_FILETRANSFER)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_FILETRANSFER_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_FILETRANSFER_PKGDIR) && tar cvzf $(PP_FILETRANSFER_PLATFORMCONFIGPACKAGEDIR)/$(PP_FILETRANSFER_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_filetransfer.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_filetransfer)
	@$(call install_fixup, pp_filetransfer,PRIORITY,optional)
	@$(call install_fixup, pp_filetransfer,SECTION,base)
	@$(call install_fixup, pp_filetransfer,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_filetransfer,DESCRIPTION,missing)

	@$(call install_finish, pp_filetransfer)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_filetransfer.clean:
	@$(call targetinfo)
	@if [ -d $(PP_FILETRANSFER_DIR) ]; then \
		$(PP_FILETRANSFER_MAKE_ENV) $(PP_FILETRANSFER_PATH) $(MAKE) $(MFLAGS) -C $(PP_FILETRANSFER_DIR) clean; \
	fi
	@$(call clean_pkg, PP_FILETRANSFER)
	@rm -rf $(PP_FILETRANSFER_BUILDROOT_DIR)

# vim: syntax=make
