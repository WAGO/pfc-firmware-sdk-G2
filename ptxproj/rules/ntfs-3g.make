# -*-makefile-*-
#
# Copyright (C) 2017 by Thomas Haemmerle <thomas.haemmerle@wolfvision.net>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NTFS_3G) += ntfs-3g

#
# Paths and names
#
NTFS_3G_VERSION	:= 2022.10.3
NTFS_3G_MD5	:= a038af61be7584b79f8922ff11244090
NTFS_3G		:= ntfs-3g_ntfsprogs-$(NTFS_3G_VERSION)
NTFS_3G_SUFFIX	:= tgz
NTFS_3G_URL	:= https://tuxera.com/opensource/$(NTFS_3G).$(NTFS_3G_SUFFIX)
NTFS_3G_SOURCE	:= $(SRCDIR)/$(NTFS_3G).$(NTFS_3G_SUFFIX)
NTFS_3G_DIR	:= $(BUILDDIR)/$(NTFS_3G)
NTFS_3G_LICENSE	:= GPL-2.0-or-later AND LGPL-2.0-only
NTFS_3G_LICENSE_FILES	:= \
	file://README;startline=39;endline=46;md5=a3204cfad57631a62a347096d10f3498 \
	file://COPYING;md5=59530bdf33659b29e73d4adb9f9f6552 \
	file://COPYING.LIB;md5=f30a9716ef3762e3467a2f62bf790f0a

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
NTFS_3G_CONF_TOOL	:= autoconf
NTFS_3G_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--exec-prefix=/usr \
	--disable-debug \
	--enable-warnings \
	--disable-pedantic \
	--disable-really-static \
	--disable-mount-helper \
	--disable-ldscript \
	--disable-ldconfig \
	--disable-library \
	--disable-mtab \
	--disable-posix-acls \
	--disable-xattr-mappings \
	--disable-plugins \
	--enable-device-default-io-ops \
	--enable-ntfs-3g \
	--disable-ntfsprogs \
	--disable-crypto \
	--disable-extras \
	--disable-quarantined \
	--disable-nfconv \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--with-fuse=external \
	--with-uuid \
	--without-hd

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ntfs-3g.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ntfs-3g)
	@$(call install_fixup, ntfs-3g,PRIORITY,optional)
	@$(call install_fixup, ntfs-3g,SECTION,base)
	@$(call install_fixup, ntfs-3g,AUTHOR,"Thomas Haemmerle <thomas.haemmerle@wolfvision.net>")
	@$(call install_fixup, ntfs-3g,DESCRIPTION,missing)

	@$(call install_copy, ntfs-3g, 0, 0, 0755, -, /usr/bin/ntfs-3g)

#	# ntfs-g3 creates the links in /sbin. Create our own links.
	@$(call install_link, ntfs-3g, ../bin/ntfs-3g, /usr/sbin/mount.ntfs)
	@$(call install_link, ntfs-3g, ../bin/ntfs-3g, /usr/sbin/mount.ntfs-3g)

	@$(call install_finish, ntfs-3g)

	@$(call touch)

# vim: syntax=make
