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
PACKAGES-$(PTXCONF_PP_SERIALINTERFACE) += pp_serialinterface

#
# Paths and names
#
PP_SERIALINTERFACE_VERSION        := 1.2.0
PP_SERIALINTERFACE_MD5            :=
PP_SERIALINTERFACE                := pp_serialinterface
PP_SERIALINTERFACE_BUILDCONFIG    := Release
PP_SERIALINTERFACE_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SERIALINTERFACE)
PP_SERIALINTERFACE_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SERIALINTERFACE)
PP_SERIALINTERFACE_DIR            := $(PP_SERIALINTERFACE_BUILDROOT_DIR)/src
PP_SERIALINTERFACE_BUILD_DIR      := $(PP_SERIALINTERFACE_BUILDROOT_DIR)/bin/$(PP_SERIALINTERFACE_BUILDCONFIG)
PP_SERIALINTERFACE_LICENSE        := WAGO
PP_SERIALINTERFACE_CONF_TOOL      := NO
PP_SERIALINTERFACE_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SERIALINTERFACE_BUILDCONFIG) \
BIN_DIR=$(PP_SERIALINTERFACE_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SERIALINTERFACE_PACKAGE_NAME             := $(PP_SERIALINTERFACE)_$(PP_SERIALINTERFACE_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SERIALINTERFACE_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_serialinterface.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_SERIALINTERFACE_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SERIALINTERFACE_DIR) ]; then \
	  ln -s $(PP_SERIALINTERFACE_SRC_DIR) $(PP_SERIALINTERFACE_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_serialinterface.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SERIALINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_serialinterface.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SERIALINTERFACE)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_serialinterface.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SERIALINTERFACE_PKGDIR) && \
	  tar xvzf $(PP_SERIALINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(PP_SERIALINTERFACE_PACKAGE_NAME).tgz -C $(PP_SERIALINTERFACE_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SERIALINTERFACE)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SERIALINTERFACE_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SERIALINTERFACE_PKGDIR) && tar cvzf $(PP_SERIALINTERFACE_PLATFORMCONFIGPACKAGEDIR)/$(PP_SERIALINTERFACE_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_serialinterface.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_serialinterface)
	@$(call install_fixup, pp_serialinterface,PRIORITY,optional)
	@$(call install_fixup, pp_serialinterface,SECTION,base)
	@$(call install_fixup, pp_serialinterface,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_serialinterface,DESCRIPTION,missing)

	@$(call install_copy, pp_serialinterface, 0, 0, 0755, -, /usr/sbin/pp_serialinterface)
	@$(call install_copy, pp_serialinterface, 0, 0, 0750, -, /etc/init.d/pp_serialinterface)

	@$(call install_link, pp_serialinterface, /etc/init.d/pp_serialinterface, /etc/rc.d/S99_pp_serialinterface)

	@$(call install_finish, pp_serialinterface)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_serialinterface.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SERIALINTERFACE_DIR) ]; then \
		$(PP_SERIALINTERFACE_MAKE_ENV) $(PP_SERIALINTERFACE_PATH) $(MAKE) $(MFLAGS) -C $(PP_SERIALINTERFACE_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SERIALINTERFACE)
	@rm -rf $(PP_SERIALINTERFACE_BUILDROOT_DIR)

# vim: syntax=make
