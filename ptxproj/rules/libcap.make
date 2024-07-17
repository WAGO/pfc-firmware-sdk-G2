# -*-makefile-*-
#
# Copyright (C) 2010 by Robert Schwebel <r.schwebel@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCAP) += libcap

#
# Paths and names
#
LIBCAP_VERSION	:= 2.69
LIBCAP_MD5	:= 4667bacb837f9ac4adb4a1a0266f4b65
#LIBCAP_VERSION	:= 2.66
#LIBCAP_MD5	:= 00afd6e13bc94b2543b1a70770bdb41f
LIBCAP		:= libcap-$(LIBCAP_VERSION)
LIBCAP_SUFFIX	:= tar.xz
LIBCAP_URL	:= \
	$(call ptx/mirror, KERNEL, libs/security/linux-privs/libcap2/$(LIBCAP).$(LIBCAP_SUFFIX))
LIBCAP_SOURCE	:= $(SRCDIR)/$(LIBCAP).$(LIBCAP_SUFFIX)
LIBCAP_DIR	:= $(BUILDDIR)/$(LIBCAP)
LIBCAP_LICENSE	:= BSD-3-Clause OR GPL-2.0-only
LIBCAP_LICENSE_FILES := file://License;md5=2965a646645b72ecee859b43c592dcaa

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBCAP_MAKE_OPT	:= \
	prefix=/usr lib=lib \
	CROSS_COMPILE=$(COMPILER_PREFIX) \
	BUILD_CC=$(HOSTCC) \
	DYNAMIC=yes \
	GOLANG=no \
	LIBATTR=$(call ptx/yesno, PTXCONF_LIBCAP_SETCAP) \
	PAM_CAP=$(call ptx/yesno, PTXCONF_GLOBAL_PAM)

LIBCAP_INSTALL_OPT :=  \
	$(LIBCAP_MAKE_OPT) \
	RAISE_SETFCAP=no \
	install

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libcap.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  libcap)
	@$(call install_fixup, libcap,PRIORITY,optional)
	@$(call install_fixup, libcap,SECTION,base)
	@$(call install_fixup, libcap,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, libcap,DESCRIPTION,missing)

	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/getpcaps)
	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/capsh)
	@$(call install_lib,  libcap, 0, 0, 0644, libcap)
ifdef PTXCONF_GLOBAL_PAM
	@$(call install_copy, libcap, 0, 0, 0755, -, \
		/usr/lib/security/pam_cap.so)
endif
ifdef PTXCONF_LIBCAP_SETCAP
	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/setcap)
	@$(call install_copy, libcap, 0, 0, 0755, -, /usr/sbin/getcap)
endif
	@$(call install_finish, libcap)

	@$(call touch)

# vim: syntax=make

