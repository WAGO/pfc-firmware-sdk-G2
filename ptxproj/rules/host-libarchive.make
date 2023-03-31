# -*-makefile-*-
#
# Copyright (C) 2015 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
HOST_PACKAGES-$(PTXCONF_HOST_LIBARCHIVE) += host-libarchive

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

HOST_LIBARCHIVE_CONF_ENV	:= \
	$(HOST_ENV) \
	ac_cv_have_decl_EXT2_IOC_GETFLAGS=no \
	ac_cv_header_ext2fs_ext2_fs_h=no

#
# autoconf
#
HOST_LIBARCHIVE_CONF_TOOL	:= autoconf
HOST_LIBARCHIVE_CONF_OPT	:= \
	$(HOST_AUTOCONF) \
	--disable-static \
	--disable-bsdtar \
	--disable-bsdcat \
	--disable-bsdcpio \
	--disable-rpath \
	--enable-posix-regex-lib=libc \
	--disable-xattr \
	--disable-acl \
	--with-zlib \
	--without-bz2lib \
	--without-iconv \
	--without-lz4 \
	--without-lzma \
	--without-lzo2 \
	--without-cng \
	--without-nettle \
	--without-openssl \
	--without-xml2 \
	--without-expat


# vim: syntax=make
