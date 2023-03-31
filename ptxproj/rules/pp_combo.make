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
PACKAGES-$(PTXCONF_PP_COMBO) += pp_combo

#
# Paths and names
#
PP_COMBO_VERSION        := 1.0.0
PP_COMBO_MD5            :=
PP_COMBO_BASE           := pp_combo
PP_COMBO                := pp_combo-$(PP_COMBO_VERSION)
PP_COMBO_BUILDCONFIG    := Release
PP_COMBO_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_COMBO_BASE)
PP_COMBO_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_COMBO)
PP_COMBO_DIR            := $(PP_COMBO_BUILDROOT_DIR)/src
PP_COMBO_BUILD_DIR      := $(PP_COMBO_BUILDROOT_DIR)/bin/$(PP_COMBO_BUILDCONFIG)
PP_COMBO_LICENSE        := WAGO
PP_COMBO_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_COMBO_BUILDCONFIG) \
BIN_DIR=$(PP_COMBO_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_COMBO_PACKAGE_NAME             := $(PP_COMBO_BASE)_$(PP_COMBO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_COMBO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_combo.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_COMBO_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_COMBO_DIR) ]; then \
	  ln -s $(PP_COMBO_SRC_DIR) $(PP_COMBO_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PP_COMBO_CONF_TOOL	:= NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_combo.prepare:
	@$(call targetinfo)
	@$(call touch)
endif


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_combo.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_combo.install:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, PP_COMBO)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_COMBO_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(PP_COMBO_PKGDIR) && tar cvzf $(PP_COMBO_PLATFORMCONFIGPACKAGEDIR)/$(PP_COMBO_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(PP_COMBO_PKGDIR) && \
	  tar xvzf $(PP_COMBO_PLATFORMCONFIGPACKAGEDIR)/$(PP_COMBO_PACKAGE_NAME).tgz -C $(PP_COMBO_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_combo.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_combo)
	@$(call install_fixup, pp_combo,PRIORITY,optional)
	@$(call install_fixup, pp_combo,SECTION,base)
	@$(call install_fixup, pp_combo,AUTHOR,"OG - WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_combo,DESCRIPTION,missing)

ifdef PTXCONF_PP_COMBO_DAEMON
	@$(call install_copy, pp_combo, 0, 0, 0750, -, /usr/sbin/pp_combo)

# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PP_COMBO_DAEMON
	@$(call install_copy, pp_combo, 0, 0, 0700, -, /etc/init.d/pp_combo)

ifneq ($(call remove_quotes, $(PTXCONF_PP_COMBO_DAEMON_BBINIT_LINK)),)
	@$(call install_link, pp_combo, \
	  /etc/init.d/pp_combo, \
	  /etc/rc.d/$(PTXCONF_PP_COMBO_DAEMON_BBINIT_LINK))
endif
endif # ifdef PTXCONF_PP_COMBO_DAEMON
endif # PTXCONF_INITMETHOD_BBINIT

endif

	@$(call install_finish, pp_combo)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_combo.clean:
	@$(call targetinfo)
	@if [ -d $(PP_COMBO_DIR) ]; then \
		$(PP_COMBO_MAKE_ENV) $(PP_COMBO_PATH) $(MAKE) $(MFLAGS) -C $(PP_COMBO_DIR) clean; \
	fi
	@$(call clean_pkg, PP_COMBO)
	@rm -rf $(PP_COMBO_BUILDROOT_DIR)

# vim: syntax=make
