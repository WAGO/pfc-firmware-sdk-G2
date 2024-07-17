# -*-makefile-*-
#
# Copyright (C) 2023 by WAGO GmbH \& Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_LIBCONFIGSTP) += libconfigstp

#
# Paths and names
#
LIBCONFIGSTP_VERSION        := 1.0.0
LIBCONFIGSTP_MD5            :=
LIBCONFIGSTP                := libconfigstp
LIBCONFIGSTP_BUILDCONFIG    := Release
#LIBCONFIGSTP_SRC_DIR       := $(call ptx/in-path, PTXDIST_PATH, local_src/$(LIBCONFIGSTP))
LIBCONFIGSTP_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/$(LIBCONFIGSTP)
LIBCONFIGSTP_BUILDROOT_DIR  := $(BUILDDIR)/$(LIBCONFIGSTP)
LIBCONFIGSTP_DIR            := $(LIBCONFIGSTP_BUILDROOT_DIR)/src
LIBCONFIGSTP_BUILD_DIR      := $(LIBCONFIGSTP_BUILDROOT_DIR)/bin/$(LIBCONFIGSTP_BUILDCONFIG)
LIBCONFIGSTP_LICENSE        := MPL-2.0
LIBCONFIGSTP_CONF_TOOL      := NO
LIBCONFIGSTP_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(LIBCONFIGSTP_BUILDCONFIG) \
BIN_DIR=$(LIBCONFIGSTP_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build \
#PTXDIST_PACKAGE_MK_FILE=$(call ptx/in-path, PTXDIST_PATH, rules/libconfigstp.make)

#LIBCONFIGSTP_PACKAGE_NAME             := $(LIBCONFIGSTP)_$(LIBCONFIGSTP_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
#LIBCONFIGSTP_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(LIBCONFIGSTP_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; \
	exit 2

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfigstp.extract: | $(LIBCONFIGSTP_SRC_DIR)
	@$(call targetinfo)
	@echo "create link ($(LIBCONFIGSTP_DIR) -> $(LIBCONFIGSTP_SRC_DIR))"
	@mkdir -p $(LIBCONFIGSTP_BUILDROOT_DIR)
	@if [ ! -L $(LIBCONFIGSTP_DIR) ]; then \
	  ln -s $(LIBCONFIGSTP_SRC_DIR) $(LIBCONFIGSTP_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#$(STATEDIR)/libconfigstp.prepare:
#	@$(call targetinfo)
#	@$(call world/prepare, LIBCONFIGSTP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/libconfigstp.compile:
#	@$(call targetinfo)
#	@$(call world/compile, LIBCONFIGSTP)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/libconfigstp.install:
#	@$(call targetinfo)
#
#	@$(call world/install, LIBCONFIGSTP)
#

#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfigstp.targetinstall:
	@$(call targetinfo)

	@$(call install_init, libconfigstp)
	@$(call install_fixup, libconfigstp,PRIORITY,optional)
	@$(call install_fixup, libconfigstp,SECTION,base)
	@$(call install_fixup, libconfigstp,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, libconfigstp,DESCRIPTION,missing)

	@$(call install_lib, libconfigstp, 0, 0, 0644, libconfigstp)

	@$(call install_finish, libconfigstp)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/libconfigstp.clean:
	@$(call targetinfo)
	@if [ -d $(LIBCONFIGSTP_DIR) ]; then \
		$(LIBCONFIGSTP_MAKE_ENV) $(LIBCONFIGSTP_PATH) $(MAKE) $(MFLAGS) -C $(LIBCONFIGSTP_DIR) clean; \
	fi
	@$(call clean_pkg, LIBCONFIGSTP)
	@rm -rf $(LIBCONFIGSTP_BUILDROOT_DIR)

# vim: syntax=make
