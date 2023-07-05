# -*-makefile-*-
#
# Copyright (C) 2022-2023 by WAGO GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_DOCKER) += docker


#
# Paths and names
#
DOCKER_IPK_SUFFIX		:= armhf.ipk
DOCKER_BASE_VERSION	:= 20.10.21+wago.2
DOCKER					:= docker
DOCKER_VERSION	:= $(DOCKER_BASE_VERSION)

DOCKER_DIR			:= $(BUILDDIR)/$(DOCKER)
DOCKER_LICENSE	:= Apache-2.0

DOCKER_PACKAGE_NAME  := $(DOCKER)_$(DOCKER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.get:
	@$(call targetinfo)
	@$(call touch)


# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.prepare:
	@$(call targetinfo)
ifdef PTXCONF_DOCKER_INSTALL_TO_HOMEDIR
	# create archive with docker binaries
	# fetch all dependencies
	mkdir -p $(DOCKER_DIR) && \
		cp $(PTXDIST_SYSROOT_TARGET)/bin/docker/* $(DOCKER_DIR)/ ; \
	cd $(DOCKER_DIR) \
    && tar -cvf docker-binaries_$(DOCKER_VERSION).tar.xz --exclude=docker-binaries_$(DOCKER_VERSION).tar.xz --use-compress-program='xz -9' * \
    && find . -type f -not -name "*.tar.xz" -delete
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.compile:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.install:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.targetinstall:
	@$(call targetinfo)
	
	@$(call install_init, docker)
	@$(call install_fixup,docker,PRIORITY,optional)
	@$(call install_fixup,docker,SECTION,base)
	@$(call install_fixup,docker,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup,docker,DESCRIPTION,"copies docker ipk to /opt/wago-docker directory")
	
ifdef PTXCONF_DOCKER_INSTALL_TO_HOMEDIR
	@$(call install_copy, docker, 0, 0, 0755, $(DOCKER_DIR)/docker-binaries_$(DOCKER_VERSION).tar.xz, /opt/wago-docker/docker-binaries_$(DOCKER_VERSION).tar.xz)
endif
	
	@$(call install_alternative, docker, 0, 0, 0750, /usr/sbin/settings_backup_docker_status)

	@$(call install_alternative, docker, 0, 0, 0755, /etc/init.d/dockerd)
	@$(call install_alternative, docker, 0, 0, 0755, /etc/docker/daemon.json)
	
	@$(call install_alternative, docker, 0, 0, 0755, /etc/config-tools/events/firewall/iptables/docker)
	@$(call install_alternative, docker, 0, 0, 0755, /opt/wago-docker/sbin/iptables)
	
	@$(call install_alternative, docker, 0, 0, 0750, /etc/config-tools/config_docker);
	@$(call install_alternative, docker, 0, 0, 0750, /etc/config-tools/get_docker_config);

ifdef PTXCONF_DOCKER_INSTALL_TO_HOMEDIR
	@$(call install_link, docker, /home/wago-docker/containerd, /usr/bin/containerd)
	@$(call install_link, docker, /home/wago-docker/containerd-shim, /usr/bin/containerd-shim)
	@$(call install_link, docker, /home/wago-docker/ctr, /usr/bin/ctr)
	@$(call install_link, docker, /home/wago-docker/docker, /usr/bin/docker)
	@$(call install_link, docker, /home/wago-docker/dockerd, /usr/bin/dockerd)
	@$(call install_link, docker, /home/wago-docker/docker-init, /usr/bin/docker-init)
	@$(call install_link, docker, /home/wago-docker/docker-proxy, /usr/bin/docker-proxy)
	@$(call install_link, docker, /home/wago-docker/runc, /usr/bin/runc)
else
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/containerd, /usr/bin/containerd)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/containerd-shim, /usr/bin/containerd-shim)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/ctr, /usr/bin/ctr)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/docker, /usr/bin/docker)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/dockerd, /usr/bin/dockerd)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/docker-init, /usr/bin/docker-init)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/docker-proxy, /usr/bin/docker-proxy)
	@$(call install_copy, docker, 0, 0, 0755, $(PTXDIST_SYSROOT_TARGET)/bin/docker/runc, /usr/bin/runc)
endif

	@$(call install_link, docker, /etc/init.d/dockerd, /etc/rc.d/disabled/S99_docker)
	
	@$(call install_finish, docker)
	
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/docker.clean:
	@$(call targetinfo)
	@$(call clean_pkg, DOCKER)

# vim: syntax=make
