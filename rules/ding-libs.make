# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH & Co. KG
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DING_LIBS) += ding-libs

#
# Paths and names
#
DING_LIBS_VERSION	:= 0.6.1
DING_LIBS_MD5		:= 141ffba92d7703b7efc2595971305de7
DING_LIBS		:= ding-libs-$(DING_LIBS_VERSION)
DING_LIBS_SUFFIX	:= tar.gz
DING_LIBS_URL		:= https://releases.pagure.org/SSSD/ding-libs//$(DING_LIBS).$(DING_LIBS_SUFFIX)
DING_LIBS_SOURCE	:= $(SRCDIR)/$(DING_LIBS).$(DING_LIBS_SUFFIX)
DING_LIBS_DIR		:= $(BUILDDIR)/$(DING_LIBS)
DING_LIBS_LICENSE	:= LGPL-3.0-or-later
DING_LIBS_LICENSE_FILES	:= file://COPYING.LESSER;md5=0ce682cee9e5b35c71c890b0458423f1

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(DING_LIBS_SOURCE):
#	@$(call targetinfo)
#	@$(call get, DING_LIBS)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#DING_LIBS_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
DING_LIBS_CONF_TOOL	:= autoconf
#DING_LIBS_CONF_OPT	:=  \
#	$(CROSS_AUTOCONF_USR)

#$(STATEDIR)/ding-libs.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, DING_LIBS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/ding-libs.compile:
#	@$(call targetinfo)
#	@$(call world/compile, DING_LIBS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/ding-libs.install:
#	@$(call targetinfo)
#	@$(call world/install, DING_LIBS)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ding-libs.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ding-libs)
	@$(call install_fixup, ding-libs,PRIORITY,optional)
	@$(call install_fixup, ding-libs,SECTION,base)
	@$(call install_fixup, ding-libs,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, ding-libs,DESCRIPTION,missing)

	@$(call install_lib, ding-libs, 0, 0, 0644, libpath_utils)
	@$(call install_lib, ding-libs, 0, 0, 0644, libdhash)
	@$(call install_lib, ding-libs, 0, 0, 0644, libcollection)
	@$(call install_lib, ding-libs, 0, 0, 0644, libref_array)
	@$(call install_lib, ding-libs, 0, 0, 0644, libbasicobjects)
	@$(call install_lib, ding-libs, 0, 0, 0644, libini_config)

	@$(call install_finish, ding-libs)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/ding-libs.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, DING_LIBS)

# vim: syntax=make
