# -*-makefile-*-
#
# Copyright (C) 2019 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WRETAIN) += wretain

#
# Paths and names
#
WRETAIN_MAJOR_VERSION := 1
WRETAIN_MINOR_VERSION := 1
WRETAIN_PATCH_VERSION := 0

WRETAIN		          := wretain
WRETAIN_SO_VERSION	  := $(WRETAIN_MAJOR_VERSION).$(WRETAIN_MINOR_VERSION).$(WRETAIN_PATCH_VERSION)
WRETAIN_VERSION	      := $(WRETAIN_SO_VERSION)

WRETAIN_URL		      := file://wago_intern/wretain
WRETAIN_SUFFIX        := $(suffix $(WRETAIN_URL))
WRETAIN_MD5           :=

WRETAIN_DIR		      := $(BUILDDIR)/$(WRETAIN)
WRETAIN_SRC_DIR        := $(PTXDIST_WORKSPACE)/wago_intern/$(WRETAIN)

WRETAIN_LICENSE	      := unknown


WRETAIN_CMAKE_OPTS =
ifdef PTXCONF_WRETAIN_BUILD_TESTING
	WRETAIN_CMAKE_OPTS +=-DBUILD_TESTING=ON
endif
ifeq ($(PTXCONF_ARCH_STRING), arm64)
	WRETAIN_CMAKE_OPTS +=-DUSE_BUILTIN_MEMSET=OFF
endif


WRETAIN_PACKAGE_NAME := wretain_$(WRETAIN_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
WRETAIN_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages


# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wretain.get:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wretain.extract:
	@$(call targetinfo)
	@mkdir -p ${WRETAIN_DIR}
	@$(call touch)
endif


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#WRETAIN_PATH	:= PATH=$(CROSS_PATH)
WRETAIN_CONF_TOOL	:= cmake
WRETAIN_MAKE_ENV	:= $(CROSS_ENV)
WRETAIN_CONF_OPT	:= $(CROSS_CMAKE_USR) $(WRETAIN_CMAKE_OPTS)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wretain.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/wretain.compile:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wretain.install:
	@$(call targetinfo)
 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
#   # BSP mode: install ipk contents to packages/wretain
	@mkdir -p $(WRETAIN_PKGDIR) && \
    tar xvzf $(WRETAIN_PLATFORMCONFIGPACKAGEDIR)/$(WRETAIN_PACKAGE_NAME).tgz -C $(WRETAIN_PKGDIR)
else
 
#   # "normal" mode: call "make install"
	@$(call world/install, WRETAIN)
 
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save install directory contents for BSP
	@cd $(WRETAIN_PKGDIR) && tar cvzf $(WRETAIN_PLATFORMCONFIGPACKAGEDIR)/$(WRETAIN_PACKAGE_NAME).tgz *
endif
 
 
endif
 
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wretain.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wretain)
	@$(call install_fixup, wretain,PRIORITY,optional)
	@$(call install_fixup, wretain,SECTION,base)
	@$(call install_fixup, wretain,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, wretain,DESCRIPTION,missing)

ifdef PTXCONF_WRETAIN_LINK_TYPE_DIRECT
	@$(call install_lib, wretain, 0, 0, 0644, libwretain_direct)
	@$(call install_alternative, wretain, 0, 0, 0640, /etc/wretain_direct.conf)
	@$(call install_link, wretain, libwretain_direct.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so)
	@$(call install_link, wretain, libwretain_direct.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so.$(WRETAIN_MAJOR_VERSION))
	@$(call install_link, wretain, libwretain_direct.so.$(WRETAIN_SO_VERSION), /usr/lib/libwretain.so.$(WRETAIN_SO_VERSION))
endif

	@$(call install_copy, wretain, 0, 0, 0755, -, /usr/sbin/retain_data)

	@$(call install_finish, wretain)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/wretain.clean:
	@$(call targetinfo)
	@-cd $(WRETAIN_DIR)-build && \
		$(WRETAIN_ENV) $(WRETAIN_PATH) $(MAKE) clean
	@$(call clean_pkg, WRETAIN)

# vim: syntax=make
