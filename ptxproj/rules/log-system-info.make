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
PACKAGES-$(PTXCONF_LOG_SYSTEM_INFO) += log-system-info

#
# Paths and names
#
LOG_SYSTEM_INFO_VERSION        := 0.1.0
LOG_SYSTEM_INFO_MD5            :=
LOG_SYSTEM_INFO                := log-system-info
LOG_SYSTEM_INFO_BUILDCONFIG    := Debug
LOG_SYSTEM_INFO_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(LOG_SYSTEM_INFO)
LOG_SYSTEM_INFO_BUILDROOT_DIR  := $(BUILDDIR)/$(LOG_SYSTEM_INFO)
LOG_SYSTEM_INFO_DIR            := $(LOG_SYSTEM_INFO_BUILDROOT_DIR)/src
LOG_SYSTEM_INFO_BUILD_DIR      := $(LOG_SYSTEM_INFO_BUILDROOT_DIR)/bin/$(LOG_SYSTEM_INFO_BUILDCONFIG)
LOG_SYSTEM_INFO_LICENSE        := WAGO
LOG_SYSTEM_INFO_CONF_TOOL      := NO
LOG_SYSTEM_INFO_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LOG_SYSTEM_INFO_BUILDCONFIG) \
BIN_DIR=$(LOG_SYSTEM_INFO_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LOG_SYSTEM_INFO_PACKAGE_NAME             := $(LOG_SYSTEM_INFO)_$(LOG_SYSTEM_INFO_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LOG_SYSTEM_INFO_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------


$(STATEDIR)/log-system-info.extract: 
	@$(call targetinfo)
	@mkdir -p $(LOG_SYSTEM_INFO_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LOG_SYSTEM_INFO_DIR) ]; then \
	  ln -s $(LOG_SYSTEM_INFO_SRC_DIR) $(LOG_SYSTEM_INFO_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/log-system-info.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LOG_SYSTEM_INFO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/log-system-info.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LOG_SYSTEM_INFO)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/log-system-info.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(LOG_SYSTEM_INFO_PKGDIR) && \
	  tar xvzf $(LOG_SYSTEM_INFO_PLATFORMCONFIGPACKAGEDIR)/$(LOG_SYSTEM_INFO_PACKAGE_NAME).tgz -C $(LOG_SYSTEM_INFO_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, LOG_SYSTEM_INFO)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(LOG_SYSTEM_INFO_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LOG_SYSTEM_INFO_PKGDIR) && tar cvzf $(LOG_SYSTEM_INFO_PLATFORMCONFIGPACKAGEDIR)/$(LOG_SYSTEM_INFO_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/log-system-info.targetinstall:
	@$(call targetinfo)

	@$(call install_init, log-system-info)
	@$(call install_fixup, log-system-info,PRIORITY,optional)
	@$(call install_fixup, log-system-info,SECTION,base)
	@$(call install_fixup, log-system-info,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, log-system-info,DESCRIPTION,missing)

	@$(call install_copy, log-system-info, 0, 0, 0755, -, /usr/bin/log-system-info)
	@$(call install_copy, log-system-info, 0, 0, 0755, -, /etc/syslog-ng.conf.d/syslog_system_info.conf)

	# busybox init
ifdef PTXCONF_INITMETHOD_BBINIT
	@$(call install_copy, log-system-info, 0, 0, 0700, -, /etc/init.d/log_system_info)
	@$(call install_link, log-system-info, /etc/init.d/log_system_info, /etc/rc.d/S99_log-system-info)
endif # PTXCONF_INITMETHOD_BBINIT

	@$(call install_finish, log-system-info)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/log-system-info.clean:
	@$(call targetinfo)
	@if [ -d $(LOG_SYSTEM_INFO_DIR) ]; then \
		$(LOG_SYSTEM_INFO_MAKE_ENV) $(LOG_SYSTEM_INFO_PATH) $(MAKE) $(MFLAGS) -C $(LOG_SYSTEM_INFO_DIR) clean; \
	fi
	@$(call clean_pkg, LOG_SYSTEM_INFO)
	@rm -rf $(LOG_SYSTEM_INFO_BUILDROOT_DIR)

# vim: syntax=make
