# -*-makefile-*-
#
# Copyright (C) 2024 by WAGO GmbH & Co. KG
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PCAPPLUSPLUS) += pcapplusplus

#
# Paths and names
#
PCAPPLUSPLUS_VERSION	:= 23.09
PCAPPLUSPLUS_MD5		:= d570784b59f8486ae6bf3ef06c0bb367
PCAPPLUSPLUS			:= pcapplusplus-$(PCAPPLUSPLUS_VERSION)
PCAPPLUSPLUS_SUFFIX		:= tar.gz
PCAPPLUSPLUS_URL		:= https://github.com/seladb/PcapPlusPlus/archive/v$(PCAPPLUSPLUS_VERSION).$(PCAPPLUSPLUS_SUFFIX)
PCAPPLUSPLUS_SOURCE		:= $(SRCDIR)/$(PCAPPLUSPLUS).$(PCAPPLUSPLUS_SUFFIX)
PCAPPLUSPLUS_DIR		:= $(BUILDDIR)/$(PCAPPLUSPLUS)
PCAPPLUSPLUS_LICENSE	:= Unlicense
PCAPPLUSPLUS_LICENSE_FILES	:= file://LICENSE;md5=911690f51af322440237a253d695d19f

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(PCAPPLUSPLUS_SOURCE):
#	@$(call targetinfo)
#	@$(call get, PCAPPLUSPLUS)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

PCAPPLUSPLUS_CONF_TOOL	:= cmake
PCAPPLUSPLUS_CONF_OPT	:= \
	$(CROSS_CMAKE_USR) \
	-DPCAPPP_BUILD_EXAMPLES=$(call ptx/ifdef, PTXCONF_PCAPPLUSPLUS_EXAMPLES,ON,OFF) \
	-DPCAPPP_BUILD_TESTS=OFF \
	-DPCAPPP_BUILD_TUTORIALS=$(call ptx/ifdef, PTXCONF_PCAPPLUSPLUS_EXAMPLES,ON,OFF) \
	-DBUILD_SHARED_LIBS=ON

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pcapplusplus.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pcapplusplus)
	@$(call install_fixup, pcapplusplus, PRIORITY, optional)
	@$(call install_fixup, pcapplusplus, SECTION, base)
	@$(call install_fixup, pcapplusplus, AUTHOR, "WAGO GmbH \& Co. KG")
	@$(call install_fixup, pcapplusplus, DESCRIPTION, missing)

	@$(call install_lib, pcapplusplus, 0, 0, 0644, libCommon++)
	@$(call install_lib, pcapplusplus, 0, 0, 0644, libPacket++)
	@$(call install_lib, pcapplusplus, 0, 0, 0644, libPcap++)

ifdef PTXCONF_PCAPPLUSPLUS_EXAMPLES
	@$(call install_tree, pcapplusplus, 0, 0, -, /usr/bin)
endif

	@$(call install_copy, pcapplusplus, 0, 0, 0644, $(PCAPPLUSPLUS_DIR)/LICENSE, /usr/share/licenses/oss/license.pcapplusplus_$(PCAPPLUSPLUS_VERSION).txt)
	@$(call install_finish, pcapplusplus)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/pcapplusplus.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, PCAPPLUSPLUS)

# vim: syntax=make
