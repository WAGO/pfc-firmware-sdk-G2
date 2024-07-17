# -*-makefile-*-
#
# Copyright (C) 2022 by <Michael Long>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_FFF) += fff

#FFF_VERSION	:= 1.1
#FFF_MD5         := a4398bda87d5dade1fed229209f42e81
FFF_VERSION	:= 0.0snapshot
FFF_MD5         := 27149dfb5827ba2b63cc49771e7dffb5
FFF             := fff-$(FFF_VERSION)
FFF_SUFFIX      := tar.gz
FFF_URL         := https://github.com/meekrosoft/fff/archive/refs/tags/v$(FFF_VERSION).$(FFF_SUFFIX)
FFF_SOURCE      := $(SRCDIR)/$(FFF).$(FFF_SUFFIX)
FFF_DIR         := $(BUILDDIR)/$(FFF)
FFF_LICENSE     := MIT

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/fff.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/fff.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/fff.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/fff.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fff.install:
	@$(call targetinfo)
	mkdir -p $(PTXDIST_SYSROOT_HOST)/usr/include
	install -c -m 644 $(FFF_DIR)/fff.h $(PTXDIST_SYSROOT_HOST)/usr/include
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/fff.targetinstall:
	@$(call targetinfo)

	@$(call install_init, fff)
	@$(call install_fixup,fff,PRIORITY,optional)
	@$(call install_fixup,fff,SECTION,base)
	@$(call install_fixup,fff,AUTHOR,"<Michael Long>")
	@$(call install_fixup,fff,DESCRIPTION,"Fake Function Framework (fff)")

	@$(call install_finish,fff)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/fff.clean:
	@$(call targetinfo)
	@rm -f $(PTXDIST_SYSROOT_HOST)/usr/include/fff.h
	@$(call clean_pkg, FFF)

# vim: syntax=make
