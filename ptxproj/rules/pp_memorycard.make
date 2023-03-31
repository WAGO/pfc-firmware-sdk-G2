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
PACKAGES-$(PTXCONF_PP_MEMORYCARD) += pp_memorycard

#
# Paths and names
#
PP_MEMORYCARD_VERSION        := 1.0.0
PP_MEMORYCARD_MD5            :=
PP_MEMORYCARD                := pp_memorycard
PP_MEMORYCARD_BUILDCONFIG    := Release
PP_MEMORYCARD_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_MEMORYCARD)
PP_MEMORYCARD_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_MEMORYCARD)
PP_MEMORYCARD_DIR            := $(PP_MEMORYCARD_BUILDROOT_DIR)/src
PP_MEMORYCARD_BUILD_DIR      := $(PP_MEMORYCARD_BUILDROOT_DIR)/bin/$(PP_MEMORYCARD_BUILDCONFIG)
PP_MEMORYCARD_LICENSE        := WAGO
PP_MEMORYCARD_CONF_TOOL      := NO
PP_MEMORYCARD_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_MEMORYCARD_BUILDCONFIG) \
BIN_DIR=$(PP_MEMORYCARD_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_MEMORYCARD_PACKAGE_NAME             := $(PP_MEMORYCARD)_$(PP_MEMORYCARD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_MEMORYCARD_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_memorycard.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_MEMORYCARD_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_MEMORYCARD_DIR) ]; then \
	  ln -s $(PP_MEMORYCARD_SRC_DIR) $(PP_MEMORYCARD_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_memorycard.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_MEMORYCARD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_memorycard.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_MEMORYCARD)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_memorycard.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_MEMORYCARD_PKGDIR) && \
	  tar xvzf $(PP_MEMORYCARD_PLATFORMCONFIGPACKAGEDIR)/$(PP_MEMORYCARD_PACKAGE_NAME).tgz -C $(PP_MEMORYCARD_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_MEMORYCARD)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_MEMORYCARD_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_MEMORYCARD_PKGDIR) && tar cvzf $(PP_MEMORYCARD_PLATFORMCONFIGPACKAGEDIR)/$(PP_MEMORYCARD_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_memorycard.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_memorycard)
	@$(call install_fixup, pp_memorycard,PRIORITY,optional)
	@$(call install_fixup, pp_memorycard,SECTION,base)
	@$(call install_fixup, pp_memorycard,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_memorycard,DESCRIPTION,missing)

#	@$(call install_lib, pp_memorycard, 0, 0, 0644, libpp_memorycard)
#	@$(call install_copy, pp_memorycard, 0, 0, 0755, -, /usr/bin/pp_memorycard)

	@$(call install_finish, pp_memorycard)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_memorycard.clean:
	@$(call targetinfo)
	@if [ -d $(PP_MEMORYCARD_DIR) ]; then \
		$(PP_MEMORYCARD_MAKE_ENV) $(PP_MEMORYCARD_PATH) $(MAKE) $(MFLAGS) -C $(PP_MEMORYCARD_DIR) clean; \
	fi
	@$(call clean_pkg, PP_MEMORYCARD)
	@rm -rf $(PP_MEMORYCARD_BUILDROOT_DIR)

# vim: syntax=make
