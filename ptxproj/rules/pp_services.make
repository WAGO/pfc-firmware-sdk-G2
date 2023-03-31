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
PACKAGES-$(PTXCONF_PP_SERVICES) += pp_services

#
# Paths and names
#
PP_SERVICES_VERSION        := 1.0.0
PP_SERVICES_MD5            :=
PP_SERVICES                := pp_services
PP_SERVICES_BUILDCONFIG    := Release
PP_SERVICES_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_SERVICES)
PP_SERVICES_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_SERVICES)
PP_SERVICES_DIR            := $(PP_SERVICES_BUILDROOT_DIR)/src
PP_SERVICES_BUILD_DIR      := $(PP_SERVICES_BUILDROOT_DIR)/bin/$(PP_SERVICES_BUILDCONFIG)
PP_SERVICES_LICENSE        := WAGO
PP_SERVICES_CONF_TOOL      := NO
PP_SERVICES_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_SERVICES_BUILDCONFIG) \
BIN_DIR=$(PP_SERVICES_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_SERVICES_PACKAGE_NAME             := $(PP_SERVICES)_$(PP_SERVICES_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_SERVICES_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_services.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_SERVICES_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_SERVICES_DIR) ]; then \
	  ln -s $(PP_SERVICES_SRC_DIR) $(PP_SERVICES_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_services.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, PP_SERVICES)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_services.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, PP_SERVICES)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_services.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(PP_SERVICES_PKGDIR) && \
	  tar xvzf $(PP_SERVICES_PLATFORMCONFIGPACKAGEDIR)/$(PP_SERVICES_PACKAGE_NAME).tgz -C $(PP_SERVICES_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, PP_SERVICES)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_SERVICES_PLATFORMCONFIGPACKAGEDIR)
	@cd $(PP_SERVICES_PKGDIR) && tar cvzf $(PP_SERVICES_PLATFORMCONFIGPACKAGEDIR)/$(PP_SERVICES_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_services.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_services)
	@$(call install_fixup, pp_services,PRIORITY,optional)
	@$(call install_fixup, pp_services,SECTION,base)
	@$(call install_fixup, pp_services,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_services,DESCRIPTION,missing)

ifdef PTXCONF_PP_SERVICES_DAEMON
	@$(call install_copy, pp_services, 0, 0, 0750, -, /usr/sbin/pp_services)

ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PP_SERVICES_STARTSCRIPT
	@$(call install_alternative, pp_services, 0, 0, 0755, /etc/init.d/pp_services)

ifneq ($(call remove_quotes,$(PTXCONF_PP_SERVICES_BBINIT_LINK)),)
	@$(call install_link, pp_services, \
		../init.d/pp_services, \
		/etc/rc.d/$(PTXCONF_PP_SERVICES_BBINIT_LINK))
endif # ifneq ...
endif # ifdef PTXCONF_PP_SERVICES_STARTSCRIPT
endif # ifdef PTXCONF_INITMETHOD_BBINIT

endif

	@$(call install_finish, pp_services)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_services.clean:
	@$(call targetinfo)
	@if [ -d $(PP_SERVICES_DIR) ]; then \
		$(PP_SERVICES_MAKE_ENV) $(PP_SERVICES_PATH) $(MAKE) $(MFLAGS) -C $(PP_SERVICES_DIR) clean; \
	fi
	@$(call clean_pkg, PP_SERVICES)
	@rm -rf $(PP_SERVICES_BUILDROOT_DIR)

# vim: syntax=make
