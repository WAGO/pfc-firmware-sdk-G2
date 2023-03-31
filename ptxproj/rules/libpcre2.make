# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH & Co. KG
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBPCRE2) += libpcre2

#
# Paths and names
#
LIBPCRE2_VERSION	:= 10.39
LIBPCRE2_MD5		:= c296b9ad214136b814e9ecbbf22d3c42
LIBPCRE2		:= pcre2-$(LIBPCRE2_VERSION)
LIBPCRE2_SUFFIX		:= tar.gz
LIBPCRE2_URL		:= \
	https://github.com/PhilipHazel/pcre2/archive/refs/tags/$(LIBPCRE2).$(LIBPCRE2_SUFFIX)
LIBPCRE2_SOURCE		:= $(SRCDIR)/$(LIBPCRE2).$(LIBPCRE2_SUFFIX)
LIBPCRE2_DIR		:= $(BUILDDIR)/$(LIBPCRE2)
LIBPCRE2_LICENSE	:= BSD-3-Clause
LIBPCRE2_LICENSE_FILES	:= file://LICENCE;md5=43cfa999260dd853cd6cb174dc396f3d

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libpcre2.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(LIBPCRE2_DIR))
	@$(call extract, LIBPCRE2)
	@$(call patchin, LIBPCRE2)
	@cd $(LIBPCRE2_DIR) && [ -f configure ] || (autoreconf --install --force)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
LIBPCRE2_CONF_TOOL	:= autoconf
LIBPCRE2_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
 	--libdir=/usr/lib \
 	--build=x86_64-host-linux-gnu \
 	--host=arm-linux-gnueabihf \
	--enable-pcre2-8 \
	--enable-pcre2-16 \
	--enable-pcre2-32 \
	--disable-debug \
 	--disable-jit \
	--disable-jit-sealloc \
	--disable-pcre2grep-jit \
	--disable-pcre2grep-callout \
	--disable-pcre2grep-callout-fork \
 	--disable-rebuild-chartables \
	--$(call ptx/endis, PTXCONF_LIBPCRE2_ENABLE_UTF8)-unicode \
 	--disable-newline-is-cr \
	--$(call ptx/disen, PTXCONF_LIBPCRE2_ENABLE_NEWLINE_IS_ANYCRLF)-newline-is-lf \
 	--disable-newline-is-crlf \
	--$(call ptx/endis, PTXCONF_LIBPCRE2_ENABLE_NEWLINE_IS_ANYCRLF)-newline-is-anycrlf \
 	--disable-newline-is-any \
	--disable-newline-is-nul \
 	--disable-bsr-anycrlf \
	--disable-never-backslash-C \
 	--disable-ebcdic \
 	--disable-ebcdic-nl25 \
	--$(call ptx/endis, PTXCONF_LIBPCRE2_ENABLE_PCREGREP_LIBZ)-pcre2grep-libz \
	--$(call ptx/endis, PTXCONF_LIBPCRE2_ENABLE_PCREGREP_LIBBZ2)-pcre2grep-libbz2 \
	--disable-pcre2test-libedit \
	--disable-pcre2test-libreadline \
 	--disable-valgrind \
 	--disable-coverage \
	--with-pcre2grep-bufsize=20480 \
	--disable-fuzz-support \
	--disable-percent-zt \
 	--with-link-size=2 \
 	--with-parens-nest-limit=250 \
 	--with-match-limit=10000000 \
	--with-match-limit-depth=10000000

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libpcre2.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libpcre2)
	@$(call install_fixup, libpcre2,PRIORITY,optional)
	@$(call install_fixup, libpcre2,SECTION,base)
	@$(call install_fixup, libpcre2,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, libpcre2,DESCRIPTION,missing)

	@$(call install_lib, libpcre2, 0, 0, 0644, libpcre2-8)
	@$(call install_lib, libpcre2, 0, 0, 0644, libpcre2-16)
	@$(call install_lib, libpcre2, 0, 0, 0644, libpcre2-32)

ifdef PTXCONF_LIBPCRE2_LIBPCREPOSIX
	@$(call install_lib, libpcre2, 0, 0, 0644, libpcre2-posix)
endif

ifdef PTXCONF_LIBPCRE2_PCREGREP
	@$(call install_copy, libpcre2, 0, 0, 0755, -, /usr/bin/pcre2grep)
endif

	@$(call install_finish, libpcre2)

	@$(call touch)

# vim: syntax=make
