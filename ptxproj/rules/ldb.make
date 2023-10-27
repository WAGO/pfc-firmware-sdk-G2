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
PACKAGES-$(PTXCONF_LDB) += ldb

#
# Paths and names
#
LDB_VERSION		:= 2.2.0
LDB_MD5			:= 8b26c52a86826044b5cf145bb8242b93
LDB			:= ldb-$(LDB_VERSION)
LDB_SUFFIX		:= tar.gz
LDB_URL			:= https://www.samba.org/ftp/ldb/$(LDB).$(LDB_SUFFIX)
LDB_SOURCE		:= $(SRCDIR)/$(LDB).$(LDB_SUFFIX)
LDB_DIR			:= $(BUILDDIR)/$(LDB)
LDB_LICENSE		:= LGPL-3.0-or-later
# cross-compile runtime checks. Initial file generated with
# --cross-execute=$(PTXDIST_SYSROOT_CROSS)/bin/qemu-cross
LDB_CONFIG		 = $(call ptx/get-alternative, config/ldb, cross-answers)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

LDB_CONF_TOOL	:= NO
LDB_CONF_OPT	:=  \
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
	--cross-answers=$(LDB_DIR)/cross-answers \
	--without-ldb-lmdb


$(STATEDIR)/ldb.prepare:
	@$(call targetinfo)
	@UNAME_M=$(PTXCONF_ARCH_STRING) \
		UNAME_R=$(KERNEL_VERSION) \
		UNAME_V=$(if $(KERNEL_HEADER_VERSION),$(KERNEL_HEADER_VERSION),$(KERNEL_VERSION)) \
		HAS_64BIT=$(call ptx/ifdef,PTXCONF_ARCH_LP64,OK,NO) \
		ptxd_replace_magic $(LDB_CONFIG) > $(LDB_DIR)/cross-answers
	@$(call world/execute, LDB, $(SYSTEMPYTHON3) ./buildtools/bin/waf configure $(LDB_CONF_OPT))
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/ldb.targetinstall:
	@$(call targetinfo)

	@$(call install_init, ldb)
	@$(call install_fixup, ldb,PRIORITY,optional)
	@$(call install_fixup, ldb,SECTION,base)
	@$(call install_fixup, ldb,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, ldb,DESCRIPTION,missing)

#	#
#	# install /usr/lib/*
#	#
	@$(call install_lib, ldb, 0, 0, 0644, libldb)

#	#
#	# install /usr/lib/*
#	#
	@$(call install_copy, ldb, 0, 0, 0644, $(LDB_DIR)/bin/default/libldb-cmdline.so, /usr/lib/libldb-cmdline.so)
	@$(call install_copy, ldb, 0, 0, 0644, $(LDB_DIR)/bin/default/libldb-key-value.so, /usr/lib/libldb-key-value.so)
	@$(call install_copy, ldb, 0, 0, 0644, $(LDB_DIR)/bin/default/libldb-tdb-err-map.so, /usr/lib/libldb-tdb-err-map.so)
	@$(call install_copy, ldb, 0, 0, 0644, $(LDB_DIR)/bin/default/libldb-tdb-int.so, /usr/lib/libldb-tdb-int.so)

#	#
#	# install /usr/modules/ldb/*
#	#
	@$(call install_copy, ldb, 0, 0, 0755, /usr/modules/ldb/)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/ldap.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/asq.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/server_sort.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/paged_searches.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/rdn_name.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/sample.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/skel.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/tdb.so)
	@$(call install_copy, ldb, 0, 0, 0644, -, /usr/modules/ldb/ldb.so)

#	#
#	# install /usr/bin/*
#	#
	@$(call install_copy, ldb, 0, 0, 0755, -, /usr/bin/ldbadd)
	@$(call install_copy, ldb, 0, 0, 0755, -, /usr/bin/ldbsearch)
	@$(call install_copy, ldb, 0, 0, 0755, -, /usr/bin/ldbdel)
	@$(call install_copy, ldb, 0, 0, 0755, -, /usr/bin/ldbmodify)
	@$(call install_copy, ldb, 0, 0, 0755, -, /usr/bin/ldbedit)
	@$(call install_copy, ldb, 0, 0, 0755, -, /usr/bin/ldbrename)

	@$(call install_finish, ldb)

	@$(call touch)

# vim: syntax=make
