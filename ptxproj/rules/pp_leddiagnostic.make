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
PACKAGES-$(PTXCONF_PP_LEDDIAGNOSTIC) += pp_leddiagnostic

#
# Paths and names
#
PP_LEDDIAGNOSTIC_VERSION        := 1.0.1
PP_LEDDIAGNOSTIC_MD5            :=
PP_LEDDIAGNOSTIC_BASE           := pp_leddiagnostic
PP_LEDDIAGNOSTIC                := pp_leddiagnostic-$(PP_LEDDIAGNOSTIC_VERSION)
PP_LEDDIAGNOSTIC_BUILDCONFIG    := Release
PP_LEDDIAGNOSTIC_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_LEDDIAGNOSTIC_BASE)
PP_LEDDIAGNOSTIC_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_LEDDIAGNOSTIC)
PP_LEDDIAGNOSTIC_DIR            := $(PP_LEDDIAGNOSTIC_BUILDROOT_DIR)/src
PP_LEDDIAGNOSTIC_BUILD_DIR      := $(PP_LEDDIAGNOSTIC_BUILDROOT_DIR)/bin/$(PP_LEDDIAGNOSTIC_BUILDCONFIG)
PP_LEDDIAGNOSTIC_LICENSE        := WAGO
PP_LEDDIAGNOSTIC_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_LEDDIAGNOSTIC_BUILDCONFIG) \
BIN_DIR=$(PP_LEDDIAGNOSTIC_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_LEDDIAGNOSTIC_PACKAGE_NAME             := $(PP_LEDDIAGNOSTIC_BASE)_$(PP_LEDDIAGNOSTIC_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_LEDDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_leddiagnostic.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_LEDDIAGNOSTIC_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_LEDDIAGNOSTIC_DIR) ]; then \
	  ln -s $(PP_LEDDIAGNOSTIC_SRC_DIR) $(PP_LEDDIAGNOSTIC_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PP_LEDDIAGNOSTIC_CONF_TOOL	:= NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_leddiagnostic.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_leddiagnostic.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_leddiagnostic.install:
	@$(call targetinfo)
	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, PP_LEDDIAGNOSTIC)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_LEDDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(PP_LEDDIAGNOSTIC_PKGDIR) && tar cvzf $(PP_LEDDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/$(PP_LEDDIAGNOSTIC_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(PP_LEDDIAGNOSTIC_PKGDIR) && \
	  tar xvzf $(PP_LEDDIAGNOSTIC_PLATFORMCONFIGPACKAGEDIR)/$(PP_LEDDIAGNOSTIC_PACKAGE_NAME).tgz -C $(PP_LEDDIAGNOSTIC_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_leddiagnostic.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_leddiagnostic)
	@$(call install_fixup, pp_leddiagnostic,PRIORITY,optional)
	@$(call install_fixup, pp_leddiagnostic,SECTION,base)
	@$(call install_fixup, pp_leddiagnostic,AUTHOR,"DMa - WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_leddiagnostic,DESCRIPTION,missing)

ifdef PTXCONF_PP_LEDDIAGNOSTIC_DAEMON
	@$(call install_copy, pp_leddiagnostic, 0, 0, 0750, -, /usr/sbin/pp_leddiagnostic)

# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PP_LEDDIAGNOSTIC_DAEMON
	@$(call install_copy, pp_leddiagnostic, 0, 0, 0700, -, /etc/init.d/pp_leddiagnostic)

ifneq ($(call remove_quotes, $(PTXCONF_PP_LEDDIAGNOSTIC_DAEMON_BBINIT_LINK)),)
	@$(call install_link, pp_leddiagnostic, \
	  /etc/init.d/pp_leddiagnostic, \
	  /etc/rc.d/$(PTXCONF_PP_LEDDIAGNOSTIC_DAEMON_BBINIT_LINK))
endif
endif # ifdef PTXCONF_PP_LEDDIAGNOSTIC_DAEMON
endif # PTXCONF_INITMETHOD_BBINIT

endif

	@$(call install_finish, pp_leddiagnostic)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_leddiagnostic.clean:
	@$(call targetinfo)
	@if [ -d $(PP_LEDDIAGNOSTIC_DIR) ]; then \
	  $(PP_LEDDIAGNOSTIC_MAKE_ENV) $(PP_LEDDIAGNOSTIC_PATH) $(MAKE) $(MFLAGS) -C $(PP_LEDDIAGNOSTIC_DIR) clean; \
	fi
	@$(call clean_pkg, PP_LEDDIAGNOSTIC)
	@rm -rf $(PP_LEDDIAGNOSTIC_BUILDROOT_DIR)

# vim: syntax=make
