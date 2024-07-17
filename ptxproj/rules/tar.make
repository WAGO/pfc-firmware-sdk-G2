# -*-makefile-*-
#
# Copyright (C) 2012 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_TAR) += tar

#
# Paths and names
#
TAR_VERSION	:= 1.35
TAR_MD5		:= a2d8042658cfd8ea939e6d911eaf4152
TAR		:= tar-$(TAR_VERSION)
TAR_SUFFIX	:= tar.xz
TAR_URL		:= $(call ptx/mirror, GNU, tar/$(TAR).$(TAR_SUFFIX)) \
			   https://ftp.gnu.org/gnu/tar/$(TAR).$(TAR_SUFFIX)
TAR_SOURCE	:= $(SRCDIR)/$(TAR).$(TAR_SUFFIX)
TAR_DIR		:= $(BUILDDIR)/$(TAR)
TAR_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
TAR_CONF_TOOL	:= autoconf
TAR_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-acl \
	--disable-rpath \
	--disable-nls \
	--disable-backup-scripts \
	--with-included-regex \
	--without-selinux \
	--with-xattrs

# tar has a silent dependency to libacl! if libacl was build and
# "with-posix-acls" is set in configure, then tar will have a
# runtime dependency to libacl which might not be installed within
# targetimage, e.g. libacl is build as module
ifeq ($(PTXCONF_ACL),y)
	TAR_CONF_OPT	+= --with-posix-acls
else
	TAR_CONF_OPT	+= --without-posix-acls
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tar.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tar)
	@$(call install_fixup, tar,PRIORITY,optional)
	@$(call install_fixup, tar,SECTION,base)
	@$(call install_fixup, tar,AUTHOR,"Michael Olbrich <m.olbrich@pengutronix.de>")
	@$(call install_fixup, tar,DESCRIPTION,missing)

	@$(call install_copy, tar, 0, 0, 0755, -, /usr/bin/tar)

	@$(call install_finish, tar)

	@$(call touch)

# vim: syntax=make
