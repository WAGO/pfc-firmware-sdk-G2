# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO Kontakttechnik GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PP_CODESYS2) += pp_codesys2

#
# Paths and names
#
PP_CODESYS2_VERSION        := 0.1.0
PP_CODESYS2_MD5            :=
PP_CODESYS2                := pp_codesys2
PP_CODESYS2_BUILDCONFIG    := Release
PP_CODESYS2_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_CODESYS2)
PP_CODESYS2_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_CODESYS2)
PP_CODESYS2_DIR            := $(PP_CODESYS2_BUILDROOT_DIR)/src
PP_CODESYS2_BUILD_DIR      := $(PP_CODESYS2_BUILDROOT_DIR)/bin/$(PP_CODESYS2_BUILDCONFIG)
PP_CODESYS2_LICENSE        := WAGO
PP_CODESYS2_CONF_TOOL      := NO
PP_CODESYS2_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_CODESYS2_BUILDCONFIG) \
BIN_DIR=$(PP_CODESYS2_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_CODESYS2_PACKAGE_NAME             := $(PP_CODESYS2)_$(PP_CODESYS2_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_CODESYS2_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_codesys2.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_CODESYS2_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_CODESYS2_DIR) ]; then \
	  ln -s $(PP_CODESYS2_SRC_DIR) $(PP_CODESYS2_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys2.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_CODESYS2)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys2.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_CODESYS2)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys2.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_CODESYS2_PKGDIR) && \
	  tar xvzf $(PP_CODESYS2_PLATFORMCONFIGPACKAGEDIR)/$(PP_CODESYS2_PACKAGE_NAME).tgz -C $(PP_CODESYS2_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_CODESYS2)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_CODESYS2_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_CODESYS2_PKGDIR) && tar cvzf $(PP_CODESYS2_PLATFORMCONFIGPACKAGEDIR)/$(PP_CODESYS2_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_codesys2)
	@$(call install_fixup, pp_codesys2,PRIORITY,optional)
	@$(call install_fixup, pp_codesys2,SECTION,base)
	@$(call install_fixup, pp_codesys2,AUTHOR,"WAGO Kontakttechnik GmbH \& Co. KG")
	@$(call install_fixup, pp_codesys2,DESCRIPTION,missing)

	@$(call install_copy, pp_codesys2, 0, 0, 0755, -, /usr/sbin/pp_codesys2)
	@$(call install_copy, pp_codesys2, 0, 0, 0750, -, /etc/init.d/pp_codesys2)

	@$(call install_link, pp_codesys2, /etc/init.d/pp_codesys2, /etc/rc.d/S99_pp_codesys2)

	@$(call install_finish, pp_codesys2)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_codesys2.clean:
	@$(call targetinfo)
	@if [ -d $(PP_CODESYS2_DIR) ]; then \
		$(PP_CODESYS2_MAKE_ENV) $(PP_CODESYS2_PATH) $(MAKE) $(MFLAGS) -C $(PP_CODESYS2_DIR) clean; \
	fi
	@$(call clean_pkg, PP_CODESYS2)
	@rm -rf $(PP_CODESYS2_BUILDROOT_DIR)

# vim: syntax=make
