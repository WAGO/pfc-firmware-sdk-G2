# -*-makefile-*-
#
# Copyright (C) 2020 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# add an arbitrary string to the <PKG>_CFGHASH source
# If the string changes, then the package will be rebuilt
#
ifdef PTXDIST_SETUP_ONCE
define ptx/cfghash
$(call ptx/file,>>$(PTXDIST_TEMPDIR)/pkghash-$(strip $(1)),$(strip $(2)))
endef

define ptx/cfghash-file
$(call ptx/file,>>$(PTXDIST_TEMPDIR)/pkghash.list,CONFIG: $(strip $(1)) $(strip $(2)))
endef
else
ptx/cfghash :=
ptx/cfghash-file :=
endif

#
# Add the specified file to the cfghash for the package.
# Only add files from the BSP, not the build tree.
#
define ptx/dts-cfghash-file
$(foreach dts,$(filter-out $(PTXDIST_PLATFORMDIR)/%,
$(call ptx/in-path,$(strip $(1))_DTS_PATH,$(2))),
$(call ptx/cfghash-file,$(1),$(dts)))
endef

define world/dts-cfghash-file
$(foreach dts,$($(strip $(1))_DTS),
$(call ptx/dts-cfghash-file,$(strip $(1)),$(dts)))
endef

# vim: syntax=make
