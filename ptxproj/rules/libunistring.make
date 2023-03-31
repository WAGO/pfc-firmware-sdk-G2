# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH & Co. KG
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBUNISTRING) += libunistring

#
# Paths and names
#
LIBUNISTRING_VERSION	:= 1.0
LIBUNISTRING_MD5	:= 342070c1e18e74c66a836f79f334c4eb
LIBUNISTRING		:= libunistring-$(LIBUNISTRING_VERSION)
LIBUNISTRING_SUFFIX	:= tar.gz
LIBUNISTRING_URL	:= https://ftp.gnu.org/gnu/libunistring/$(LIBUNISTRING).$(LIBUNISTRING_SUFFIX)
LIBUNISTRING_SOURCE	:= $(SRCDIR)/$(LIBUNISTRING).$(LIBUNISTRING_SUFFIX)
LIBUNISTRING_DIR	:= $(BUILDDIR)/$(LIBUNISTRING)
LIBUNISTRING_LICENSE		:= LGPLv3+ or GPLv3+
LIBUNISTRING_LICENSE_FILES	:= file://COPYING.LIB;md5=6a6a8e020838b23406c81b19c1d46df6 \
				   file://COPYING;md5=d32239bcb673463ab874e80d47fae504

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(LIBUNISTRING_SOURCE):
#	@$(call targetinfo)
#	@$(call get, LIBUNISTRING)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBUNISTRING_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
LIBUNISTRING_CONF_TOOL	:= autoconf
LIBUNISTRING_CONF_OPT	:=  \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-rpath

#$(STATEDIR)/libunistring.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBUNISTRING)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libunistring.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBUNISTRING)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libunistring.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBUNISTRING)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libunistring.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libunistring)
	@$(call install_fixup, libunistring,PRIORITY,optional)
	@$(call install_fixup, libunistring,SECTION,base)
	@$(call install_fixup, libunistring,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, libunistring,DESCRIPTION,missing)

	@$(call install_lib, libunistring, 0, 0, 0644, libunistring)

	@$(call install_finish, libunistring)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/libunistring.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, LIBUNISTRING)

# vim: syntax=make
