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
PACKAGES-$(PTXCONF_CYRUS_SASL) += cyrus-sasl

#
# Paths and names
#
CYRUS_SASL_VERSION		:= 2.1.28
CYRUS_SASL_MD5			:= 6f228a692516f5318a64505b46966cfa
CYRUS_SASL			:= cyrus-sasl-$(CYRUS_SASL_VERSION)
CYRUS_SASL_SUFFIX		:= tar.gz
CYRUS_SASL_URL			:= https://github.com/cyrusimap/cyrus-sasl/releases/download/$(CYRUS_SASL)/$(CYRUS_SASL).$(CYRUS_SASL_SUFFIX)
CYRUS_SASL_SOURCE		:= $(SRCDIR)/$(CYRUS_SASL).$(CYRUS_SASL_SUFFIX)
CYRUS_SASL_DIR			:= $(BUILDDIR)/$(CYRUS_SASL)
CYRUS_SASL_LICENSE		:= MIT-CMU
CYRUS_SASL_LICENSE_FILES	:= file://COPYING;md5=3f55e0974e3d6db00ca6f57f2d206396

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(CYRUS_SASL_SOURCE):
#	@$(call targetinfo)
#	@$(call get, CYRUS_SASL)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cyrus-sasl.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(CYRUS_SASL_DIR))
	@$(call extract, CYRUS_SASL)
	@$(call patchin, CYRUS_SASL)
	cd $(CYRUS_SASL_DIR) && (autoreconf --install --force)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#CYRUS_SASL_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
CYRUS_SASL_CONF_TOOL	:= autoconf
CYRUS_SASL_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-spnego=no

#$(STATEDIR)/cyrus-sasl.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, CYRUS_SASL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/cyrus-sasl.compile:
#	@$(call targetinfo)
#	@$(call world/compile, CYRUS_SASL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/cyrus-sasl.install:
#	@$(call targetinfo)
#	@$(call world/install, CYRUS_SASL)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cyrus-sasl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, cyrus-sasl)
	@$(call install_fixup, cyrus-sasl,PRIORITY,optional)
	@$(call install_fixup, cyrus-sasl,SECTION,base)
	@$(call install_fixup, cyrus-sasl,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, cyrus-sasl,DESCRIPTION,missing)

#	#
#	# install /usr/lib/*
#	#
	@$(call install_lib, cyrus-sasl, 0, 0, 0644, libsasl2)

#	#
#	# install /usr/lib/sasl2/*
#	#
	@$(call install_copy, cyrus-sasl, 0, 0, 0755, /usr/lib/sasl2/)
	@$(call install_copy, cyrus-sasl, 0, 0, 0644, -, /usr/lib/sasl2/libcrammd5.so)
	@$(call install_copy, cyrus-sasl, 0, 0, 0644, -, /usr/lib/sasl2/libdigestmd5.so)
	@$(call install_copy, cyrus-sasl, 0, 0, 0644, -, /usr/lib/sasl2/libplain.so)
	@$(call install_copy, cyrus-sasl, 0, 0, 0644, -, /usr/lib/sasl2/libanonymous.so)

#	#
#	# install /usr/sbin/*
#	#
	@$(call install_copy, cyrus-sasl, 0, 0, 0755, -, /usr/sbin/pluginviewer)
	@$(call install_copy, cyrus-sasl, 0, 0, 0755, -, /usr/sbin/saslauthd)
	@$(call install_copy, cyrus-sasl, 0, 0, 0755, -, /usr/sbin/testsaslauthd)

	@$(call install_finish, cyrus-sasl)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/cyrus-sasl.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, CYRUS_SASL)

# vim: syntax=make
