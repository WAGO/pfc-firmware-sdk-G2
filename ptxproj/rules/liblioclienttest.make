# -*-makefile-*-
#
# Copyright (C) 2013 by <elrest>
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBLIOCLIENTTEST) += liblioclienttest

#
# Paths and names
#
LIBLIOCLIENT		:= liblioclient
LIBLIOCLIENTTEST_VERSION	:= 0.0.1
LIBLIOCLIENTTEST_MD5		:=
LIBLIOCLIENTTEST		:= lioclient
LIBLIOCLIENTTEST_URL		:= lndir://$(PTXDIST_WORKSPACE)/wago_intern/$(LIBLIOCLIENT)/test
LIBLIOCLIENTTEST_DIR		:= $(BUILDDIR)/$(LIBLIOCLIENT)/test
LIBLIOCLIENTTEST_LICENSE	:= unknown

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/liblioclienttest.extract:
#	@$(call targetinfo)
#	@$(call clean, $(LIBLIOCLIENTTEST_DIR))
#	@$(call extract, LIBLIOCLIENTTEST)
#	@$(call patchin, LIBLIOCLIENTTest)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#LIBLIOCLIENTTEST_PATH	:= PATH=$(CROSS_PATH)
LIBLIOCLIENTTEST_CONF_TOOL	:= NO
LIBLIOCLIENTTEST_MAKE_ENV	:= $(CROSS_ENV)

#$(STATEDIR)/liblioclienttest.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBLIOCLIENTTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/liblioclienttest.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBLIOCLIENTTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/liblioclienttest.install:
#	@$(call targetinfo)
#	@$(call world/install, LIBLIOCLIENTTEST)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclienttest.targetinstall:
	@$(call targetinfo)

	@$(call install_init, liblioclienttest)
	@$(call install_fixup, liblioclienttest,PRIORITY,optional)
	@$(call install_fixup, liblioclienttest,SECTION,base)
	@$(call install_fixup, liblioclienttest,AUTHOR,"<elrest>")
	@$(call install_fixup, liblioclienttest,DESCRIPTION,missing)

#
# TODO: Add here all files that should be copied to the target
# Note: Add everything before(!) call to macro install_finish
#
	@$(call install_copy, liblioclienttest, 0, 0, 0755, $(LIBLIOCLIENTTEST_DIR)/lioclient, /usr/bin/lioclient)
	#@cp $(LIBLIOCLIENTTEST_DIR)/lioclient /media/sf_shared
	@$(call install_finish, liblioclienttest)
	@$(call touch)


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/liblioclienttest.clean:
	@$(call targetinfo)
	@-cd $(LIBLIOCLIENTTEST_DIR) && \
		$(LIBLIOCLIENTTEST_MAKE_ENV) $(LIBLIOCLIENTTEST_PATH) $(MAKE) clean
	@$(call clean_pkg, LIBLIOCLIENTTEST)

# vim: syntax=make
