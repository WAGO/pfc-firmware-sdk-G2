# -*-makefile-*-
#
# Copyright (C) 2022 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PCRE2) += pcre2

#
# Paths and names
#
PCRE2_VERSION		:= 10.42
PCRE2_MD5		:= a8e9ab2935d428a4807461f183034abe
PCRE2			:= pcre2-$(PCRE2_VERSION)
PCRE2_SUFFIX		:= tar.bz2
PCRE2_URL		:= https://github.com/PCRE2Project/pcre2/releases/download/$(PCRE2)/$(PCRE2).$(PCRE2_SUFFIX)
PCRE2_SOURCE		:= $(SRCDIR)/$(PCRE2).$(PCRE2_SUFFIX)
PCRE2_DIR		:= $(BUILDDIR)/$(PCRE2)
PCRE2_LICENSE		:= BSD-3-Clause
PCRE2_LICENSE_FILES	:= file://LICENCE;md5=41bfb977e4933c506588724ce69bf5d2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
PCRE2_CONF_TOOL	:= autoconf
PCRE2_CONF_OPT	:=  \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/endis,PTXCONF_PCRE2_8)-pcre2-8 \
	--$(call ptx/endis,PTXCONF_PCRE2_16)-pcre2-16 \
	--$(call ptx/endis,PTXCONF_PCRE2_32)-pcre2-32 \
	--disable-debug \
	--disable-jit \
	--disable-jit-sealloc \
	--enable-pcre2grep-jit \
	--enable-pcre2grep-callout \
	--enable-pcre2grep-callout-fork \
	--disable-rebuild-chartables \
	--enable-unicode \
	--enable-newline-is-lf \
	--disable-bsr-anycrlf \
	--disable-never-backslash-C \
	--disable-ebcdic \
	--disable-ebcdic-nl25 \
	--disable-pcre2grep-libz \
	--disable-pcre2grep-libbz2 \
	--disable-pcre2test-libedit \
	--disable-pcre2test-libreadline \
	--disable-valgrind \
	--disable-coverage \
	--disable-fuzz-support \
	--enable-percent-zt

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pcre2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pcre2)
	@$(call install_fixup, pcre2,PRIORITY,optional)
	@$(call install_fixup, pcre2,SECTION,base)
	@$(call install_fixup, pcre2,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, pcre2,DESCRIPTION,missing)

ifdef PTXCONF_PCRE2_8
	@$(call install_lib, pcre2, 0, 0, 0644, libpcre2-8)
endif
ifdef PTXCONF_PCRE2_16
	@$(call install_lib, pcre2, 0, 0, 0644, libpcre2-16)
endif
ifdef PTXCONF_PCRE2_32
	@$(call install_lib, pcre2, 0, 0, 0644, libpcre2-32)
endif

	@$(call install_finish, pcre2)

	@$(call touch)

# vim: syntax=make
