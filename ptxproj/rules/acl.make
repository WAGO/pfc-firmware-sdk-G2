# -*-makefile-*-
#
# Copyright (C) 2009 by Carsten Schlote <c.schlote@konzeptpark.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_ACL) += acl

#
# Paths and names
#
ACL_VERSION	:= 2.3.1
ACL_MD5		:= 3cecb80cb0a52a0b273e6698ba642263
ACL		:= acl-$(ACL_VERSION)
ACL_SUFFIX	:= tar.gz
ACL_URL		:= http://download.savannah.gnu.org/releases/acl/$(ACL).$(ACL_SUFFIX)
ACL_SOURCE	:= $(SRCDIR)/$(ACL).$(ACL_SUFFIX)
ACL_DIR		:= $(BUILDDIR)/$(ACL)
ACL_LICENSE	:= LGPL-2.1-or-later
ACL_LICENSE_FILES:= file://doc/COPYING.LGPL;md5=9e9a206917f8af112da634ce3ab41764
ifdef PTXCONF_ACL_TOOLS
ACL_LICENSE+= AND GPL-2.0.or-later
ACL_LICENSE_FILES+= file://doc/COPYING;md5=c781d70ed2b4d48995b790403217a249
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
ACL_CONF_TOOL	:= autoconf
ACL_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-nls \
	--disable-rpath \
	--disable-debug

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/acl.targetinstall:
	@$(call targetinfo)

	@$(call install_init, acl)
	@$(call install_fixup, acl,PRIORITY,optional)
	@$(call install_fixup, acl,SECTION,base)
	@$(call install_fixup, acl,AUTHOR,"Carsten Schlote <c.schlote@konzeptpark.de>")
	@$(call install_fixup, acl,DESCRIPTION,missing)

ifdef PTXCONF_ACL_TOOLS
	@$(call install_copy, acl, 0, 0, 0755, -, /usr/bin/chacl)
	@$(call install_copy, acl, 0, 0, 0755, -, /usr/bin/setfacl)
	@$(call install_copy, acl, 0, 0, 0755, -, /usr/bin/getfacl)
endif
	@$(call install_lib, acl, 0, 0, 0644, libacl)

	@$(call install_finish, acl)

	@$(call touch)

# vim: syntax=make