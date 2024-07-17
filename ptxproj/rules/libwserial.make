# -*-makefile-*-
#
# Copyright (C) 2023 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBWSERIAL) += libwserial

#
# Paths and names
#
LIBWSERIAL_VERSION        := 0.2.2
LIBWSERIAL_MD5            :=
LIBWSERIAL                := libwserial
LIBWSERIAL_BUILDCONFIG    := Release
LIBWSERIAL_SRC_DIR        := $(call ptx/in-path, PTXDIST_PATH, wago_intern/$(LIBWSERIAL))
LIBWSERIAL_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBWSERIAL)
LIBWSERIAL_DIR            := $(LIBWSERIAL_BUILDROOT_DIR)/src
LIBWSERIAL_BUILD_DIR      := $(LIBWSERIAL_BUILDROOT_DIR)/bin/$(LIBWSERIAL_BUILDCONFIG)
LIBWSERIAL_LICENSE        := WAGO
LIBWSERIAL_CONF_TOOL      := NO
LIBWSERIAL_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBWSERIAL_BUILDCONFIG) \
BIN_DIR=$(LIBWSERIAL_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/libwserial.make)

LIBWSERIAL_PACKAGE_NAME             := $(LIBWSERIAL)_$(LIBWSERIAL_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBWSERIAL_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libwserial.extract: 
	@$(call targetinfo)
	@mkdir -p $(LIBWSERIAL_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@if [ ! -L $(LIBWSERIAL_DIR) ]; then \
	  ln -s $(LIBWSERIAL_SRC_DIR) $(LIBWSERIAL_DIR); \
	fi
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libwserial.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/prepare, LIBWSERIAL)
	@$(call xslt_compile, $(LIBWSERIAL_SRC_DIR)/src/led-message-ids.xml)
	@echo "Create serial led log header file in sysroot-target/usr/include/ directory by using of XSLT-script"
	@rm 	$(LIBWSERIAL_SRC_DIR)/src/diagnostics.dtd
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/libwserial.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBWSERIAL)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwserial.install:
	@$(call targetinfo)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
# BSP mode: install by extracting tgz file
	@mkdir -p $(LIBWSERIAL_PKGDIR) && \
	  tar xvzf $(LIBWSERIAL_PLATFORMCONFIGPACKAGEDIR)/$(LIBWSERIAL_PACKAGE_NAME).tgz -C $(LIBWSERIAL_PKGDIR)
else
# normal mode, call "make install"

	@$(call world/install, LIBWSERIAL)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
# save install directory to tgz for BSP mode
	@mkdir -p $(LIBWSERIAL_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBWSERIAL_PKGDIR) && tar cvzf $(LIBWSERIAL_PLATFORMCONFIGPACKAGEDIR)/$(LIBWSERIAL_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libwserial.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libwserial)
	@$(call install_fixup, libwserial,PRIORITY,optional)
	@$(call install_fixup, libwserial,SECTION,base)
	@$(call install_fixup, libwserial,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, libwserial,DESCRIPTION,missing)

	@$(call install_lib, libwserial, 0, 0, 0644, libwserial)
	@$(call install_copy, libwserial, 0, 0, 0750, -, /etc/config-tools/config_serial_interface)
	@$(call install_copy, libwserial, 0, 0, 0644, -, /etc/specific/serial.conf)
	@$(call install_copy, libwserial, 0, 0, 0444, -, /etc/sudoers.d/config_serial_interface)
	@$(call install_copy, libwserial, 0, 0, 0750, -, /etc/config-tools/backup-restore/backup_serial_interface)

	@$(call install_finish, libwserial)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libwserial.clean:
	@$(call targetinfo)
	@if [ -d $(LIBWSERIAL_DIR) ]; then \
		$(LIBWSERIAL_MAKE_ENV) $(LIBWSERIAL_PATH) $(MAKE) $(MFLAGS) -C $(LIBWSERIAL_DIR) clean; \
	fi
	@$(call clean_pkg, LIBWSERIAL)
	@rm -rf $(LIBWSERIAL_BUILDROOT_DIR)

# vim: syntax=make
