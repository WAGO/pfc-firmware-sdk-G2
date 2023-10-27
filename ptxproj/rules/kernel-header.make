# -*-makefile-*-
#
# Copyright (C) 2002-2009 by Pengutronix e.K., Hildesheim, Germany
#               2010 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_KERNEL_HEADER) += kernel-header


#
# when using a production release,
# we use the precompiled kernel from /opt
#
ifdef PTXCONF_PROJECT_USE_PRODUCTION
KERNEL_HEADER_BDIR := $(PTXDIST_BASE_PLATFORMDIR)/build-target
else
KERNEL_HEADER_BDIR := $(BUILDDIR)
endif

#
# Paths and names
#

KERNEL_HEADER		:= linux-$(KERNEL_HEADER_VERSION)
ifdef PTXCONF_KERNEL_ARTIFACTORY
KERNEL_HEADER_VERSION	:= $(call remove_quotes, $(PTXCONF_KERNEL_VERSION)-$(PTXCONF_KERNEL_RT_PATCH)-$(PTXCONF_KERNEL_LOCALVERSION))
KERNEL_HEADER_MD5_FILE	:= src/linux-$(KERNEL_HEADER_VERSION).tgz.md5
KERNEL_HEADER_SUFFIX	:= tgz
KERNEL_HEADER_URL	:= $(call jfrog_template_to_url, KERNEL)
KERNEL_HEADER_ARTIFACT	:= $(call jfrog_get_filename,$(KERNEL_HEADER_URL))
else
KERNEL_HEADER_SUFFIX	:= tar.xz
KERNEL_HEADER_URL	:= $(call kernel-url, KERNEL)
endif
KERNEL_HEADER_MD5	:= $(call remove_quotes,$(PTXCONF_KERNEL_HEADER_MD5))
KERNEL_HEADER_DIR	:= $(BUILDDIR)/kernel-header-$(KERNEL_HEADER_VERSION)
KERNEL_HEADER_BUILD_DIR	:= $(KERNEL_HEADER_DIR)-build
KERNEL_HEADER_PKGDIR	:= $(PKGDIR)/kernel-header-$(KERNEL_HEADER_VERSION)
KERNEL_HEADER_CONFIG	:= $(call remove_quotes, $(PTXDIST_PLATFORMCONFIGDIR)/$(PTXCONF_KERNEL_HEADER_CONFIG))
KERNEL_HEADER_LICENSE	:= GPL-2.0
KERNEL_HEADER_ARCHIVE	:= linux-$(KERNEL_HEADER_VERSION).$(KERNEL_HEADER_SUFFIX)
KERNEL_HEADER_SOURCE	:= $(SRCDIR)/linux-$(KERNEL_HEADER_VERSION).$(KERNEL_HEADER_SUFFIX)
KERNEL_HEADER_DEVPKG	:= NO
KERNEL_HEADER_BUILD_OOT	:= KEEP


# ----------------------------------------------------------------------------
# Include
# ----------------------------------------------------------------------------

include $(call ptx/in-path, PTXDIST_PATH, rules/other/definitions.make)
# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------
#
ifdef PTXCONF_KERNEL_ARTIFACTORY
$(KERNEL_HEADER_SOURCE):
	@$(call targetinfo)
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
		'$(KERNEL_URL)' src/linux-$(KERNEL_HEADER_VERSION).$(KERNEL_HEADER_SUFFIX) '$(KERNEL_HEADER_MD5_FILE)'
	@$(call touch)
endif
#
# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

KERNEL_HEADER_CONF_TOOL     := NO

$(STATEDIR)/kernel-header.prepare:
	@$(call targetinfo)
	@$(call world/prepare, $(PTX_MAP_TO_PACKAGE_kernel-header))

	@$(call touch)

$(STATEDIR)/kernel-header.compile:
	@$(call targetinfo)

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

KERNEL_HEADER_INSTALL_OPT   := \
    -C $(KERNEL_HEADER_DIR) \
    V=$(PTXDIST_VERBOSE) \
    O=$(KERNEL_HEADER_BUILD_DIR) \
    HOSTCC=$(HOSTCC) \
    ARCH=$(GENERIC_KERNEL_ARCH) \
    CROSS_COMPILE=$(PTXCONF_COMPILER_PREFIX) \
    INSTALL_HDR_PATH=$(KERNEL_HEADER_PKGDIR)/kernel-headers \
    headers_install

world/install.pack = \
    $(call world/env, $1) \
    ptxd_make_world_install_pack

#
# $1: label of the packet
# $2: optional: alternative directory
# $3: optional: "h" = install as a host tool
# $4: optional: args to pass to make install call
#
install = \
    pkg_deprecated_install_builddir="$(call ptx/escape, $(2))" \
    pkg_deprecated_install_hosttool="$(call ptx/escape, $(3))" \
    pkg_deprecated_install_opt="$(call ptx/escape, $(4))" \
    $(call world/install, $(1))


$(STATEDIR)/kernel-header.install:
	@$(call targetinfo)
	@$(call world/install, $(PTX_MAP_TO_PACKAGE_kernel-header))
	@$(call touch)

$(STATEDIR)/kernel-header.install.pack:
	@$(call targetinfo)
	@$(call world/install.pack, $(PTX_MAP_TO_PACKAGE_kernel-header))
	@$(call touch)


# ----------------------------------------------------------------------------
# tags
# ----------------------------------------------------------------------------

$(STATEDIR)/kernel-header.tags:
	@$(call targetinfo)
	cd $(KERNEL_HEADER_DIR) && $(KERNEL_HEADER_PATH) $(KERNEL_HEADER_ENV) $(MAKE) \
		$(KERNEL_HEADER_MAKEVARS) tags TAGS cscope

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------


$(STATEDIR)/kernel-header.clean: FORCE
	@$(call targetinfo)
	@$(call world/clean, $(PTX_MAP_TO_PACKAGE_kernel-header))

#
# clean
#
#world/clean = \
#	$(call world/image/env, $(1)) \
#	ptxd_make_world_clean

#
# clean_pkg
#
# remove all temporary files for the current package
#
#clean_pkg = \
#   $(call world/clean, $(1))
# vim: syntax=make
