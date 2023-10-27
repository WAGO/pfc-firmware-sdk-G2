# -*-makefile-*-
#
# Copyright (C) 2020 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

world/dtb/env = \
	$(call world/env, $(1)) \
	pkg_dts_path="$($(1)_DTS_PATH)" \
	pkg_dts="$($(1)_DTS)" \
	pkg_arch="$(GENERIC_KERNEL_ARCH)"

world/dtb = \
	$(call world/dtb/env,$(strip $(1))) \
	ptxd_make_world_dtb

# vim: syntax=make
