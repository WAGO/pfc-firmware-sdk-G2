# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBARCHIVE) += libarchive

#
# Paths and names
#
LIBARCHIVE_VERSION	:= 3.6.1
LIBARCHIVE_MD5		:= 802a56ef9eaa0b8776296ba78a6d0c2c
LIBARCHIVE		:= libarchive-$(LIBARCHIVE_VERSION)
LIBARCHIVE_SUFFIX	:= tar.gz
LIBARCHIVE_URL		:= https://www.libarchive.org/downloads/$(LIBARCHIVE).$(LIBARCHIVE_SUFFIX)
LIBARCHIVE_SOURCE	:= $(SRCDIR)/$(LIBARCHIVE).$(LIBARCHIVE_SUFFIX)
LIBARCHIVE_DIR		:= $(BUILDDIR)/$(LIBARCHIVE)
LIBARCHIVE_LICENSE	:= BSD-2-Clause AND BSD-3-Clause AND public_domain AND \
			   (CC-0-1.0 OR OpenSSL OR Apache-2.0)
LIBARCHIVE_LICENSE_FILES	:= file://COPYING;md5=d499814247adaee08d88080841cb5665

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LIBARCHIVE_CONF_ENV	:= \
	$(CROSS_ENV) \
	ac_cv_have_decl_EXT2_IOC_GETFLAGS=no \
	ac_cv_header_ext2fs_ext2_fs_h=no \
	ac_cv_lib_md_MD5Init=no

#
# autoconf
#
LIBARCHIVE_CONF_TOOL	:= autoconf
LIBARCHIVE_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--$(call ptx/endis, PTXCONF_LIBARCHIVE_BSDTAR)-bsdtar \
	--$(call ptx/endis, PTXCONF_LIBARCHIVE_BSDCAT)-bsdcat \
	--$(call ptx/endis, PTXCONF_LIBARCHIVE_BSDCPIO)-bsdcpio \
	--disable-rpath \
	--enable-posix-regex-lib=libc \
	--disable-xattr \
	--disable-acl \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--with-zlib \
	--$(call ptx/wwo, PTXCONF_LIBARCHIVE_BZIP2)-bz2lib \
	--without-libb2 \
	--without-iconv \
	--without-lz4 \
	--without-zstd \
	--$(call ptx/wwo, PTXCONF_LIBARCHIVE_LZMA)-lzma \
	--without-lzo2 \
	--without-cng \
	--without-mbedtls \
	--without-nettle \
	--without-openssl \
	--without-xml2 \
	--without-expat

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libarchive.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libarchive)
	@$(call install_fixup, libarchive,PRIORITY,optional)
	@$(call install_fixup, libarchive,SECTION,base)
	@$(call install_fixup, libarchive,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, libarchive,DESCRIPTION,missing)

	@$(call install_lib, libarchive, 0, 0, 0644, libarchive)
ifdef PTXCONF_LIBARCHIVE_BSDTAR
	@$(call install_copy, libarchive, 0, 0, 0755, -, /usr/bin/bsdtar)
endif
ifdef PTXCONF_LIBARCHIVE_BSDCAT
	@$(call install_copy, libarchive, 0, 0, 0755, -, /usr/bin/bsdcat)
endif
ifdef PTXCONF_LIBARCHIVE_BSDCPIO
	@$(call install_copy, libarchive, 0, 0, 0755, -, /usr/bin/bsdcpio)
endif

	@$(call install_finish, libarchive)

	@$(call touch)

# vim: syntax=make
