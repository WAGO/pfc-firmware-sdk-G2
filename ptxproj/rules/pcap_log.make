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
PACKAGES-$(PTXCONF_PCAP_LOG) += pcap_log

#
# Paths and names
#
PCAP_LOG_VERSION        := 1.1.0
PCAP_LOG_MD5            :=
PCAP_LOG                := pcap_log
PCAP_LOG_BUILDCONFIG    := Release
PCAP_LOG_SRC_DIR        := $(PTXDIST_WORKSPACE)/local_src/config-tools/$(PCAP_LOG)
PCAP_LOG_BUILDROOT_DIR  := $(BUILDDIR)/$(PCAP_LOG)
PCAP_LOG_DIR            := $(PCAP_LOG_BUILDROOT_DIR)/src
PCAP_LOG_BUILD_DIR      := $(PCAP_LOG_BUILDROOT_DIR)/bin/$(PCAP_LOG_BUILDCONFIG)
PCAP_LOG_LICENSE        := WAGO
PCAP_LOG_CONF_TOOL      := NO
PCAP_LOG_MAKE_ENV       := $(CROSS_ENV) \
BUILDCONFIG=$(PCAP_LOG_BUILDCONFIG) \
BIN_DIR=$(PCAP_LOG_BUILD_DIR) \
SCRIPT_DIR=$(PTXDIST_SYSROOT_HOST)/lib/ct-build


# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

# During BSP creation local_src is deleted and the source code directories are
# copied on demand. To handle this condition an order-only dependency on
# the source code directory is created. When it is missing, the target below
# is executed and an error message is generated.
$(PCAP_LOG_SRC_DIR):
	@echo "Error: $@: directory not found!" >&2; exit 2

$(STATEDIR)/pcap_log.extract:  | $(PCAP_LOG_SRC_DIR)
	@$(call targetinfo)
	@mkdir -p $(PCAP_LOG_BUILDROOT_DIR)
	@if [ ! -L $(PCAP_LOG_DIR) ]; then \
	  ln -s $(PCAP_LOG_SRC_DIR) $(PCAP_LOG_DIR); \
	fi
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/pcap_log.prepare:
	@$(call targetinfo)
	@$(call world/prepare, PCAP_LOG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/pcap_log.compile:
	@$(call targetinfo)
	@$(call world/compile, PCAP_LOG)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pcap_log.install:
	@$(call targetinfo)

	@$(call world/install, PCAP_LOG)


	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/pcap_log.targetinstall:
	@$(call targetinfo)

	@$(call install_init, pcap_log)
	@$(call install_fixup, pcap_log,PRIORITY,optional)
	@$(call install_fixup, pcap_log,SECTION,base)
	@$(call install_fixup, pcap_log,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, pcap_log,DESCRIPTION,missing)

	@$(call install_copy, pcap_log, 0, 0, 0755, $(PCAP_LOG_BUILD_DIR)/pcap_log.elf, /etc/config-tools/pcap_log)
	@$(call install_copy, pcap_log, 0, 0, 0444, $(PCAP_LOG_DIR)/etc/sudoers.d/pcap_log, /etc/sudoers.d/pcap_log)

	@$(call install_finish, pcap_log)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/pcap_log.clean:
	@$(call targetinfo)
	@if [ -d $(PCAP_LOG_DIR) ]; then \
		$(PCAP_LOG_MAKE_ENV) $(PCAP_LOG_PATH) $(MAKE) $(MFLAGS) -C $(PCAP_LOG_DIR) clean; \
	fi
	@$(call clean_pkg, PCAP_LOG)
	@rm -rf $(PCAP_LOG_BUILDROOT_DIR)

# vim: syntax=make
