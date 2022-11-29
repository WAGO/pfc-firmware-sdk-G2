# -*-makefile-*-
#
# Copyright (C) 2022 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_WGETRC) += wgetrc

WGETRC_VERSION	:= 1.0.0

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(STATEDIR)/wgetrc.get:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

#$(STATEDIR)/wgetrc.extract:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/wgetrc.prepare:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/wgetrc.compile:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/wgetrc.install:
#	@$(call targetinfo)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/wgetrc.targetinstall:
	@$(call targetinfo)

	@$(call install_init, wgetrc)
	@$(call install_fixup,wgetrc,PRIORITY,optional)
	@$(call install_fixup,wgetrc,SECTION,base)
	@$(call install_fixup,wgetrc,AUTHOR,"<>")
	@$(call install_fixup,wgetrc,DESCRIPTION,missing)
#
	@$(call install_alternative, wgetrc, 0, 0, 0644, /etc/wgetrc)

	@$(call install_finish,wgetrc)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/wgetrc.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, WGETRC)

# vim: syntax=make
