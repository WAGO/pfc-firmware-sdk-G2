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
PACKAGES-$(PTXCONF_PP_HOSTDOMAINNAME) += pp_hostdomainname

#
# Paths and names
#
PP_HOSTDOMAINNAME_VERSION        := 1.0.1
PP_HOSTDOMAINNAME_MD5            :=
PP_HOSTDOMAINNAME                := pp_hostdomainname
PP_HOSTDOMAINNAME_BUILDCONFIG    := Release
PP_HOSTDOMAINNAME_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_HOSTDOMAINNAME)
PP_HOSTDOMAINNAME_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_HOSTDOMAINNAME)
PP_HOSTDOMAINNAME_DIR            := $(PP_HOSTDOMAINNAME_BUILDROOT_DIR)/src
PP_HOSTDOMAINNAME_BUILD_DIR      := $(PP_HOSTDOMAINNAME_BUILDROOT_DIR)/bin/$(PP_HOSTDOMAINNAME_BUILDCONFIG)
PP_HOSTDOMAINNAME_LICENSE        := WAGO
PP_HOSTDOMAINNAME_CONF_TOOL      := NO
PP_HOSTDOMAINNAME_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_HOSTDOMAINNAME_BUILDCONFIG) \
BIN_DIR=$(PP_HOSTDOMAINNAME_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_HOSTDOMAINNAME_PACKAGE_NAME             := $(PP_HOSTDOMAINNAME)_$(PP_HOSTDOMAINNAME_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_HOSTDOMAINNAME_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_hostdomainname.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_HOSTDOMAINNAME_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_HOSTDOMAINNAME_DIR) ]; then \
	  ln -s $(PP_HOSTDOMAINNAME_SRC_DIR) $(PP_HOSTDOMAINNAME_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_hostdomainname.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_HOSTDOMAINNAME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_hostdomainname.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_HOSTDOMAINNAME)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_hostdomainname.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_HOSTDOMAINNAME_PKGDIR) && \
	  tar xvzf $(PP_HOSTDOMAINNAME_PLATFORMCONFIGPACKAGEDIR)/$(PP_HOSTDOMAINNAME_PACKAGE_NAME).tgz -C $(PP_HOSTDOMAINNAME_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_HOSTDOMAINNAME)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_HOSTDOMAINNAME_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_HOSTDOMAINNAME_PKGDIR) && tar cvzf $(PP_HOSTDOMAINNAME_PLATFORMCONFIGPACKAGEDIR)/$(PP_HOSTDOMAINNAME_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/pp_hostdomainname.targetinstall:
#	@$(call targetinfo)
#
#	@$(call install_init, pp_hostdomainname)
#	@$(call install_fixup, pp_hostdomainname,PRIORITY,optional)
#	@$(call install_fixup, pp_hostdomainname,SECTION,base)
#	@$(call install_fixup, pp_hostdomainname,AUTHOR,"WAGO GmbH \& Co. KG")
#	@$(call install_fixup, pp_hostdomainname,DESCRIPTION,missing)
#
#	@$(call install_lib, pp_hostdomainname, 0, 0, 0644, libpp_hostdomainname)
#	@$(call install_copy, pp_hostdomainname, 0, 0, 0755, -, /usr/bin/pp_hostdomainname)
#
#	@$(call install_finish, pp_hostdomainname)
#
#	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_hostdomainname.clean:
	@$(call targetinfo)
	@if [ -d $(PP_HOSTDOMAINNAME_DIR) ]; then \
		$(PP_HOSTDOMAINNAME_MAKE_ENV) $(PP_HOSTDOMAINNAME_PATH) $(MAKE) $(MFLAGS) -C $(PP_HOSTDOMAINNAME_DIR) clean; \
	fi
	@$(call clean_pkg, PP_HOSTDOMAINNAME)
	@rm -rf $(PP_HOSTDOMAINNAME_BUILDROOT_DIR)

# vim: syntax=make
