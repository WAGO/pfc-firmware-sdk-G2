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
PACKAGES-$(PTXCONF_TEVENT) += tevent

#
# Paths and names
#
TEVENT_VERSION		:= 0.10.2
TEVENT_MD5		:= 105c7a4dbb96f1751eb27dfd05e7fa84
TEVENT			:= tevent-$(TEVENT_VERSION)
TEVENT_SUFFIX		:= tar.gz
TEVENT_URL		:= https://www.samba.org/ftp/tevent/$(TEVENT).$(TEVENT_SUFFIX)
TEVENT_SOURCE		:= $(SRCDIR)/$(TEVENT).$(TEVENT_SUFFIX)
TEVENT_DIR		:= $(BUILDDIR)/$(TEVENT)
TEVENT_LICENSE		:= LGPL-3.0-or-later
# cross-compile runtime checks. Initial file generated with
# --cross-execute=$(PTXDIST_SYSROOT_CROSS)/bin/qemu-cross
TEVENT_CONFIG		 = $(call ptx/get-alternative, config/tevent, cross-answers)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

TEVENT_CONF_TOOL	:= NO
TEVENT_CONF_OPT	:=  \
	--without-gettext \
	--disable-python \
	--prefix=/usr \
	--sysconfdir=/etc \
	--localstatedir=/var \
	--libdir=/usr/lib \
	--bundled-libraries=NONE \
	--disable-rpath \
	--disable-rpath-install \
	--disable-rpath-private-install \
	--disable-warnings-as-errors \
	--fatal-errors \
	--enable-gccdeps \
	--pedantic \
	--cross-compile \
	--cross-execute=/does/not/exist/and/triggers/exceptions \
	--cross-answers=$(TEVENT_DIR)/cross-answers


$(STATEDIR)/tevent.prepare:
	@$(call targetinfo)
	@UNAME_M=$(PTXCONF_ARCH_STRING) \
		UNAME_R=$(KERNEL_VERSION) \
		UNAME_V=$(if $(KERNEL_HEADER_VERSION),$(KERNEL_HEADER_VERSION),$(KERNEL_VERSION)) \
		HAS_64BIT=$(call ptx/ifdef,PTXCONF_ARCH_LP64,OK,NO) \
		ptxd_replace_magic $(TEVENT_CONFIG) > $(TEVENT_DIR)/cross-answers
	@$(call world/execute, TEVENT, $(SYSTEMPYTHON3) ./buildtools/bin/waf configure $(TEVENT_CONF_OPT))
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tevent.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tevent)
	@$(call install_fixup, tevent,PRIORITY,optional)
	@$(call install_fixup, tevent,SECTION,base)
	@$(call install_fixup, tevent,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, tevent,DESCRIPTION,missing)

	@$(call install_lib, tevent, 0, 0, 0644, libtevent)

	@$(call install_finish, tevent)

	@$(call touch)

# vim: syntax=make
