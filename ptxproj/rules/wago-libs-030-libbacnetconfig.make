# -*-makefile-*-
#
# Copyright (C) 2012 by <WAGO GmbH & Co. KG>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
#--- provided package --------------------------------------------------------
#
PACKAGES-$(PTXCONF_LIBBACNETCONFIG) += libbacnetconfig
#
#--- paths and names --------------------------------------------------------- 
#
LIBBACNETCONFIG                   := libbacnetconfig
LIBBACNETCONFIG_FOLDER						:= libbacnetconfig_git

# select BACnet stack revision
ifdef PTXCONF_LIBBACNETSTACK_SOURCE_DEV
BACNETSTACK_REVISION := 24
# configure BACnet version (IPK)
BACNET_VERSION       := 1.7.0
LIBBACNETCONFIG_VERSION        := 1.6.1
endif

ifdef PTXCONF_LIBBACNETSTACK_SOURCE_RELEASED
BACNETSTACK_REVISION := 24
# configure BACnet version (IPK)
BACNET_VERSION       := 1.7.0
LIBBACNETCONFIG_VERSION        := 1.7.2
endif

ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_DEV
LIBBACNETCONFIG_GIT_URL           := ssh://svtfs01007:22/tfs/ProductDevelopment/BACnet_Stack/_git/LibbacnetConfig
endif

LIBBACNETCONFIG_REL_PATH          := wago_intern/device/bacnet/$(LIBBACNETCONFIG_FOLDER)
LIBBACNETCONFIG_SRC_DIR           := $(PTXDIST_WORKSPACE)/$(LIBBACNETCONFIG_REL_PATH)

ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_RELEASED
LIBBACNETCONFIG_URL               := $(call jfrog_template_to_url, LIBBACNETCONFIG)
else
LIBBACNETCONFIG_URL               := file://$(LIBBACNETCONFIG_REL_PATH)
endif

LIBBACNETCONFIG_SUFFIX            := $(suffix $(LIBBACNETCONFIG_URL))
LIBBACNETCONFIG_MD5                = $(shell [ -f $(LIBBACNETCONFIG_MD5_FILE) ] && cat $(LIBBACNETCONFIG_MD5_FILE))
LIBBACNETCONFIG_MD5_FILE          := wago_intern/artifactory_sources/$(LIBBACNETCONFIG)$(LIBBACNETCONFIG_SUFFIX).md5
LIBBACNETCONFIG_ARTIFACT           = $(call jfrog_get_filename,$(LIBBACNETCONFIG_URL))
ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_RELEASED
LIBBACNETCONFIG_ARCHIVE           := $(LIBBACNETCONFIG)-$(LIBBACNETCONFIG_VERSION)$(LIBBACNETCONFIG_SUFFIX)
endif

LIBBACNETCONFIG_BUILDROOT_DIR     := $(BUILDDIR)/$(LIBBACNETCONFIG)
LIBBACNETCONFIG_DIR               := $(LIBBACNETCONFIG_BUILDROOT_DIR)/$(LIBBACNETCONFIG)
LIBBACNETCONFIG_LICENSE           := unknown

LIBBACNETCONFIG_BUILDCONFIG       := Release
LIBBACNETCONFIG_BUILD_DIR         := $(LIBBACNETCONFIG_BUILDROOT_DIR)/bin/$(LIBBACNETCONFIG_BUILDCONFIG)
LIBBACNETCONFIG_BIN               := $(LIBBACNETCONFIG).so.$(LIBBACNETCONFIG_VERSION)
LIBBACNETCONFIG_SO_NAME           := $(LIBBACNETCONFIG).so
LIBBACNETCONFIG_CONF_TOOL         := NO
LIBBACNETCONFIG_MAKE_ENV          := $(CROSS_ENV) \
                                     BUILDCONFIG=$(LIBBACNETCONFIG_BUILDCONFIG) \
                                     BIN_DIR=$(LIBBACNETCONFIG_BUILD_DIR) \
                                     TARGET_ARCH=$(PTXCONF_ARCH_STRING) \
                                     ARM_ARCH_VERSION=7 \
                                     BACNET_VERSION=$(BACNET_VERSION) \
                                     LIBBACNETSTACK_REV=$(BACNETSTACK_REVISION) \
                                     LIBBACNETCONFIG_VERSION_CT_BUILD=$(LIBBACNETCONFIG_VERSION) \
                                     SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build

LIBBACNETCONFIG_PATH              := PATH=$(CROSS_PATH)
LIBBACNETCONFIG_PACKAGE_NAME      := $(LIBBACNETCONFIG)_$(LIBBACNETCONFIG_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_DEV

$(LIBBACNETCONFIG_SRC_DIR):
	{ cd $(PTXDIST_WORKSPACE)/wago_intern && git clone $(LIBBACNETCONFIG_GIT_URL) $(LIBBACNETCONFIG_SRC_DIR); } \
    || rm -fr $(LIBBACNETCONFIG_SRC_DIR)
ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_DEV_BRANCH
	{ cd $(LIBBACNETCONFIG_SRC_DIR) && git checkout $(PTXCONF_LIBBACNETCONFIG_SOURCE_DEV_BRANCH); } \
    || rm -fr $(LIBBACNETCONFIG_SRC_DIR)
endif

$(STATEDIR)/libbacnetconfig.get: | $(LIBBACNETCONFIG_SRC_DIR)

endif

ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_RELEASED
$(STATEDIR)/libbacnetconfig.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(LIBBACNETCONFIG_URL)' wago_intern/artifactory_sources/$(LIBBACNETCONFIG_ARCHIVE) '$(LIBBACNETCONFIG_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.extract:
	@$(call targetinfo)
	@mkdir -p $(LIBBACNETCONFIG_BUILDROOT_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifdef PTXCONF_LIBBACNETCONFIG_SOURCE_RELEASED
	@mkdir -p $(LIBBACNETCONFIG_DIR)
	@tar xvf wago_intern/artifactory_sources/$(LIBBACNETCONFIG_ARCHIVE) -C $(LIBBACNETCONFIG_DIR) --strip-components=1
	@$(call patchin, LIBBACNET)
endif
ifndef PTXCONF_LIBBACNETCONFIG_SOURCE_RELEASED
	@if [ ! -L $(LIBBACNETCONFIG_DIR) ]; then \
	  	ln -s $(LIBBACNETCONFIG_SRC_DIR) $(LIBBACNETCONFIG_DIR); \
	fi
endif
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract.post
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/libbacnetconfig.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES 
	@$(call world/prepare, LIBBACNETCONFIG)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/libbacnetconfig.compile:
	@$(call targetinfo)
	
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, LIBBACNETCONFIG)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.install:
	@$(call targetinfo)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   BSP mode: install by extracting tgz file
	@mkdir -p $(LIBBACNETCONFIG_PKGDIR) && \
  	tar xvzf $(LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETCONFIG_PACKAGE_NAME).tgz -C $(LIBBACNETCONFIG_PKGDIR)
else	
# 	normal mode, call "make install"

	@$(call world/install, LIBBACNETCONFIG)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory to tgz for BSP mode
	@mkdir -p $(LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR)
	@cd $(LIBBACNETCONFIG_PKGDIR) && tar cvzf $(LIBBACNETCONFIG_PLATFORMCONFIGPACKAGEDIR)/$(LIBBACNETCONFIG_PACKAGE_NAME).tgz *
endif
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libbacnetconfig.targetinstall:
	@$(call targetinfo)
	@$(call install_init, libbacnetconfig)


	@$(call install_fixup, libbacnetconfig, PRIORITY, optional)
	@$(call install_fixup, libbacnetconfig, SECTION, base)
	@$(call install_fixup, libbacnetconfig, AUTHOR, "Nico Baade <nico.baade@wago.com>")
	@$(call install_fixup, libbacnetconfig, DESCRIPTION, "BACnet configuration library for the PFC controller" )

	@$(call install_lib, libbacnetconfig, 0, 0, 0644, libbacnetconfig)
	@$(call install_link, libbacnetconfig, $(LIBBACNETCONFIG_BIN), /usr/lib/$(LIBBACNETCONFIG_SO_NAME))
	
	@mkdir -p $(LIBBACNETCONFIG_PKGDIR)/etc/
	@$(call install_tree, libbacnetconfig, 0, 0, -, /etc/)
#ifdef PTXCONF_CT_BACNET_CONFIG
#	@$(call install_copy, libbacnetconfig, 0, 0, 0750, -, /etc/config-tools/bacnet_config)
#endif
	
#ifdef PTXCONF_CT_BACNET_BACKUP_RESTORE
#	@$(call install_copy, libbacnetconfig, 0, 0, 0750, -, /etc/config-tools/bacnet_backup_restore)
#endif



#	@$(call install_copy, libbacnetconfig, 0, 0, 0750, -, /etc/config-tools/bacnet_sc_gen_csr)
#	@$(call install_copy, libbacnetconfig, 0, 0, 0444, -, /etc/sudoers.d/bacnet_sc_gen_csr)

#	@$(call install_copy, libbacnetconfig, 0, 0, 0750, -, /etc/config-tools/bacnet_dl_config)
#	@$(call install_copy, libbacnetconfig, 0, 0, 0444, -, /etc/sudoers.d/bacnet_dl_config)


	@$(call install_finish, libbacnetconfig)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

PTX_ROOT=$(PTXCONF_SYSROOT_TARGET)/../root
PTX_BACNETCONFIG=$(PTXCONF_SYSROOT_TARGET)/usr/lib/

$(STATEDIR)/libbacnetconfig.clean:
	@$(call targetinfo)
	@if [ -d $(LIBBACNETCONFIG_DIR) ]; then \
		$(LIBBACNETCONFIG_MAKE_ENV) $(LIBBACNETCONFIG_PATH) $(MAKE) $(MFLAGS) -C $(LIBBACNETCONFIG_DIR) clean; \
	fi
	@$(call clean_pkg, LIBBACNETCONFIG)
	@rm -rf $(LIBBACNETCONFIG_BUILDROOT_DIR)
	
# vim: syntax=make
