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
HOST_PACKAGES-$(PTXCONF_HOST_PCRE2) += host-pcre2

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
HOST_PCRE2_CONF_TOOL	:= autoconf
HOST_PCRE2_CONF_OPT	:=  \
	$(HOST_AUTOCONF) \
	--enable-pcre2-8 \
	--enable-pcre2-16 \
	--disable-pcre2-32 \
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

# vim: syntax=make
