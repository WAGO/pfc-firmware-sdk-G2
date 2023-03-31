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
PACKAGES-$(PTXCONF_TDB) += tdb

#
# Paths and names
#
TDB_VERSION		:= 1.4.3
TDB_MD5			:= e638e8890f743624a754304b3f994f4d
TDB			:= tdb-$(TDB_VERSION)
TDB_SUFFIX		:= tar.gz
TDB_URL			:= https://www.samba.org/ftp/tdb/$(TDB).$(TDB_SUFFIX)
TDB_SOURCE		:= $(SRCDIR)/$(TDB).$(TDB_SUFFIX)
TDB_DIR			:= $(BUILDDIR)/$(TDB)
TDB_LICENSE		:= LGPL-3.0-or-later
# cross-compile runtime checks. Initial file generated with
# --cross-execute=$(PTXDIST_SYSROOT_CROSS)/bin/qemu-cross
TDB_CONFIG		 = $(call ptx/get-alternative, config/tdb, cross-answers)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

TDB_CONF_TOOL	:= NO
TDB_CONF_OPT	:=  \
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
	--cross-answers=$(TDB_DIR)/cross-answers


$(STATEDIR)/tdb.prepare:
	@$(call targetinfo)
	@UNAME_M=$(PTXCONF_ARCH_STRING) \
		UNAME_R=$(KERNEL_VERSION) \
		UNAME_V=$(if $(KERNEL_HEADER_VERSION),$(KERNEL_HEADER_VERSION),$(KERNEL_VERSION)) \
		HAS_64BIT=$(call ptx/ifdef,PTXCONF_ARCH_LP64,OK,NO) \
		ptxd_replace_magic $(TDB_CONFIG) > $(TDB_DIR)/cross-answers
	@$(call world/execute, TDB, $(SYSTEMPYTHON3) ./buildtools/bin/waf configure $(TDB_CONF_OPT))
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/tdb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, tdb)
	@$(call install_fixup, tdb,PRIORITY,optional)
	@$(call install_fixup, tdb,SECTION,base)
	@$(call install_fixup, tdb,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, tdb,DESCRIPTION,missing)

	@$(call install_lib, tdb, 0, 0, 0644, libtdb)

	@$(call install_finish, tdb)

	@$(call touch)

# vim: syntax=make
