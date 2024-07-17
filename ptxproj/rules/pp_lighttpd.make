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
PACKAGES-$(PTXCONF_PP_LIGHTTPD) += pp_lighttpd

#
# Paths and names
#
PP_LIGHTTPD_VERSION        := 2.0.3
PP_LIGHTTPD_MD5            :=
PP_LIGHTTPD_BASE           := pp_lighttpd
PP_LIGHTTPD                := pp_lighttpd-$(PP_LIGHTTPD_VERSION)
PP_LIGHTTPD_BUILDCONFIG    := Release
PP_LIGHTTPD_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(PP_LIGHTTPD_BASE)
PP_LIGHTTPD_BUILDROOT_DIR  := $(BUILDDIR)/$(PP_LIGHTTPD)
PP_LIGHTTPD_DIR            := $(PP_LIGHTTPD_BUILDROOT_DIR)/src
PP_LIGHTTPD_BUILD_DIR      := $(PP_LIGHTTPD_BUILDROOT_DIR)/bin/$(PP_LIGHTTPD_BUILDCONFIG)
PP_LIGHTTPD_LICENSE        := WAGO
PP_LIGHTTPD_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PP_LIGHTTPD_BUILDCONFIG) \
BIN_DIR=$(PP_LIGHTTPD_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

PP_LIGHTTPD_PACKAGE_NAME             := $(PP_LIGHTTPD_BASE)_$(PP_LIGHTTPD_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
PP_LIGHTTPD_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/pp_lighttpd.extract: 
	@$(call targetinfo)
	@mkdir -p $(PP_LIGHTTPD_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(PP_LIGHTTPD_DIR) ]; then \
	  ln -s $(PP_LIGHTTPD_SRC_DIR) $(PP_LIGHTTPD_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PP_LIGHTTPD_CONF_TOOL	:= NO

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_lighttpd.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/pp_lighttpd.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_lighttpd.install:
	@$(call targetinfo)
	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# "normal" mode: call "install"
	@$(call world/install, PP_LIGHTTPD)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(PP_LIGHTTPD_PLATFORMCONFIGPACKAGEDIR) && \
	  cd $(PP_LIGHTTPD_PKGDIR) && tar cvzf $(PP_LIGHTTPD_PLATFORMCONFIGPACKAGEDIR)/$(PP_LIGHTTPD_PACKAGE_NAME).tgz *
endif

else
# BSP mode: install ipk contents to packages/<project name>
	@mkdir -p $(PP_LIGHTTPD_PKGDIR) && \
	  tar xvzf $(PP_LIGHTTPD_PLATFORMCONFIGPACKAGEDIR)/$(PP_LIGHTTPD_PACKAGE_NAME).tgz -C $(PP_LIGHTTPD_PKGDIR)

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_lighttpd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pp_lighttpd)
	@$(call install_fixup, pp_lighttpd,PRIORITY,optional)
	@$(call install_fixup, pp_lighttpd,SECTION,base)
	@$(call install_fixup, pp_lighttpd,AUTHOR,"PEn - WAGO GmbH \& Co. KG")
	@$(call install_fixup, pp_lighttpd,DESCRIPTION,missing)

ifdef PTXCONF_PP_LIGHTTPD_DAEMON
	@$(call install_copy, pp_lighttpd, 0, 0, 0750, -, /usr/sbin/pp_lighttpd)

# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PP_LIGHTTPD_DAEMON
	@$(call install_copy, pp_lighttpd, 0, 0, 0700, -, /etc/init.d/pp_lighttpd)

ifneq ($(call remove_quotes, $(PTXCONF_PP_LIGHTTPD_DAEMON_BBINIT_LINK)),)
	@$(call install_link, pp_lighttpd, \
	  /etc/init.d/pp_lighttpd, \
	  /etc/rc.d/$(PTXCONF_PP_LIGHTTPD_DAEMON_BBINIT_LINK))
endif
endif # ifdef PTXCONF_PP_LIGHTTPD_DAEMON
endif # PTXCONF_INITMETHOD_BBINIT

endif

	@$(call install_finish, pp_lighttpd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pp_lighttpd.clean:
	@$(call targetinfo)
	@if [ -d $(PP_LIGHTTPD_DIR) ]; then \
	  $(PP_LIGHTTPD_MAKE_ENV) $(PP_LIGHTTPD_PATH) $(MAKE) $(MFLAGS) -C $(PP_LIGHTTPD_DIR) clean; \
	fi
	@$(call clean_pkg, PP_LIGHTTPD)
	@rm -rf $(PP_LIGHTTPD_BUILDROOT_DIR)

# vim: syntax=make
