# -*-makefile-*-
#
# Copyright (C) 2009 by Marc Kleine-Budde <mkl@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LSOF) += lsof

#
# Paths and names
#
LSOF_VERSION	:= 4.93.2+dfsg
LSOF_MD5	:= c58f7c40631ebc666f04e944f35db9a5
LSOF_SUFFIX	:= tar.xz
LSOF		:= lsof-$(LSOF_VERSION)
LSOF_TARBALL	:= lsof_$(LSOF_VERSION).orig.$(LSOF_SUFFIX)
LSOF_URL	:= https://snapshot.debian.org/archive/debian/20190908T172415Z/pool/main/l/lsof/$(LSOF_TARBALL)
LSOF_SOURCE	:= $(SRCDIR)/$(LSOF_TARBALL)
LSOF_DIR	:= $(BUILDDIR)/$(LSOF)
LSOF_LICENSE	:= custom
LSOF_LICENSE_FILES	:= file://00README;startline=645;endline=676;md5=3161a245910921b0f58644299e268d39

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LSOF_CONF_ENV	:= \
	$(CROSS_ENV) \
	LSOF_INCLUDE=$(PTXDIST_SYSROOT_TARGET)/usr/include \
	LINUX_HASSELINUX=N \
	LSOF_AR="$(CROSS_AR) cr"

LSOF_CONF_OPT := \
	-n linux

$(STATEDIR)/lsof.prepare:
	@$(call targetinfo)
	@$(call world/execute, LSOF, ./Configure $(LSOF_CONF_OPT))
	@$(call touch)

LSOF_MAKE_OPT	:= \
	$(CROSS_ENV_CC) \
	LSOF_USER=none \
	DEBUG=-O2 \
	RANLIB="$(CROSS_RANLIB) liblsof.a"

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lsof.install:
	@$(call targetinfo)
	install -D -m 755 "$(LSOF_DIR)/lsof" "$(LSOF_PKGDIR)/usr/bin/lsof"
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/lsof.targetinstall:
	@$(call targetinfo)

	@$(call install_init, lsof)
	@$(call install_fixup, lsof,PRIORITY,optional)
	@$(call install_fixup, lsof,SECTION,base)
	@$(call install_fixup, lsof,AUTHOR,"Marc Kleine-Budde <mkl@pengutronix.de>")
	@$(call install_fixup, lsof,DESCRIPTION,missing)

	@$(call install_copy, lsof, 0, 0, 0755, -, /usr/bin/lsof)

	@$(call install_finish, lsof)

	@$(call touch)

# vim: syntax=make
