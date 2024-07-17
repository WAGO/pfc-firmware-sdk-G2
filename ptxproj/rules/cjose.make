# -*-makefile-*-
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CJOSE) += cjose

#
# Paths and names
#
CJOSE_VERSION    	:= 0.6.2.2
CJOSE_MD5		:= f0eb45d7fe058cc55898c67bb3b0cb8f
CJOSE			:= cjose-$(CJOSE_VERSION)
CJOSE_SUFFIX		:= tar.gz
CJOSE_URL		:= https://github.com/OpenIDC/cjose/releases/download/v$(CJOSE_VERSION)/$(CJOSE).$(CJOSE_SUFFIX)
CJOSE_SOURCE		:= $(SRCDIR)/$(CJOSE).$(CJOSE_SUFFIX)
CJOSE_DIR		:= $(BUILDDIR)/$(CJOSE)
CJOSE_LICENSE		:= MIT
CJOSE_LICENSE_FILE := LICENSE
CJOSE_LICENSE_FILES	:= file://$(CJOSE_LICENSE_FILE);md5=7249e2f9437adfb8c88d870438042f0e

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

CJOSE_AUTOCONF := $(CROSS_AUTOCONF_USR) \
	--disable-shared \
	CFLAGS="-fPIC"

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cjose.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cjose)
	@$(call install_fixup, cjose,PRIORITY,optional)
	@$(call install_fixup, cjose,SECTION,base)
	@$(call install_fixup, cjose,AUTHOR,"<Name> <Mail>")
	@$(call install_fixup, cjose,DESCRIPTION,missing)

#	@$(call install_lib, cjose, 0, 0, 0644, libcjose)
	@$(call install_copy, cjose, 0, 0, 0644, $(CJOSE_DIR)/$(CJOSE_LICENSE_FILE), /usr/share/licenses/oss/license.cjose_$(CJOSE_VERSION).txt)

	@$(call install_finish, cjose)
	@$(call touch)

# vim: syntax=make
