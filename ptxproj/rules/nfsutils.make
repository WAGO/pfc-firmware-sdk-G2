# -*-makefile-*-
#
# Copyright (C) 2003-2006 by Pengutronix e.K., Hildesheim, Germany
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_NFSUTILS) += nfsutils

#
# Paths and names
#
NFSUTILS_VERSION	:= 2.5.4
NFSUTILS_MD5		:= 10f0f87e6889db976a1db94435af0bca
NFSUTILS		:= nfs-utils-$(NFSUTILS_VERSION)
NFSUTILS_SUFFIX		:= tar.gz
NFSUTILS_URL		:= $(call ptx/mirror, KERNEL, utils/nfs-utils/$(NFSUTILS_VERSION)/$(NFSUTILS).$(NFSUTILS_SUFFIX))
NFSUTILS_SOURCE		:= $(SRCDIR)/$(NFSUTILS).$(NFSUTILS_SUFFIX)
NFSUTILS_DIR		:= $(BUILDDIR)/$(NFSUTILS)
NFSUTILS_LICENSE	:= GPL-2.0-or-later AND BSD-3-Clause
NFSUTILS_LICENSE_FILES	:= \
	file://COPYING;md5=95f3a93a5c3c7888de623b46ea085a84 \
	file://utils/showmount/showmount.c;startline=5;endline=13;md5=d3811a49409f2f42614bb59fd6d68bb4 \
	file://utils/statd/COPYING;md5=0636e73ff0215e8d672dc4c32c317bb3 \
	file://utils/statd/hostname.c;startline=6;endline=14;md5=3607f10ab196fbbf8bacb79042f718e7 \
	file://utils/statd/svc_run.c;startline=13;endline=34;md5=78baf6ac3cfbb3cb057a7668cb93f64b \
	file://utils/mount/mount.c;startline=9;endline=17;md5=d3811a49409f2f42614bb59fd6d68bb4 \

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

NFSUTILS_CONF_ENV	:= \
	$(CROSS_ENV) \
	knfsd_cv_bsd_signals=no

#
# autoconf
#
NFSUTILS_CONF_TOOL	:= autoconf
NFSUTILS_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--exec-prefix=/usr \
	--disable-nfsv4 \
	--disable-nfsv41 \
	--disable-gss \
	--disable-svcgss \
	--disable-kprefix \
	--disable-uuid \
	--$(call ptx/endis, PTXCONF_NFSUTILS_CLIENT)-mount \
	--$(call ptx/endis, PTXCONF_NFSUTILS_CLIENT)-libmount-mount \
	--disable-junction \
	--enable-tirpc \
	$(GLOBAL_IPV6_OPTION) \
	--disable-mountconfig \
	--disable-nfsdcld \
	--disable-nfsdcltrack \
	--disable-nfsv4server \
	--disable-caps \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--disable-ldap \
	--disable-gums \
	--with-statedir=/var/lib/nfs \
	--with-statdpath=/var/lib/nfs \
	--with-statduser=rpcuser \
	--with-systemd=/usr/lib/systemd/system \
	--with-rpcgen=$(PTXDIST_SYSROOT_HOST)/bin/rpcgen \
	--without-mountfile \
	--without-tcp-wrappers \
	--without-krb5 \
	--without-gssglue

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/nfsutils.targetinstall:
	@$(call targetinfo)

	@$(call install_init, nfsutils)
	@$(call install_fixup, nfsutils,PRIORITY,optional)
	@$(call install_fixup, nfsutils,SECTION,base)
	@$(call install_fixup, nfsutils,AUTHOR,"Robert Schwebel <r.schwebel@pengutronix.de>")
	@$(call install_fixup, nfsutils,DESCRIPTION,"Network Filesystem Support")

	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/nfsstat)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/showmount)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpcdebug)

	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpc.statd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/start-statd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/sm-notify)

	@$(call install_copy, nfsutils, rpcuser, 0, 0755, \
		/var/lib/nfs)

ifdef PTXCONF_NFSUTILS_SYSTEMD_UNIT
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/tmpfiles.d/nfs.conf)
else
	@$(call install_copy, nfsutils, rpcuser, 0, 0644, -, \
		/var/lib/nfs/etab)
	@$(call install_copy, nfsutils, rpcuser, 0, 0644, -, \
		/var/lib/nfs/rmtab)
	@$(call install_copy, nfsutils, rpcuser, 0, 0644, -, \
		/var/lib/nfs/state)
	@$(call install_copy, nfsutils, rpcuser, 0, 0755, \
		/var/lib/nfs/sm)
	@$(call install_copy, nfsutils, rpcuser, 0, 0755, \
		/var/lib/nfs/sm.bak)
endif

ifdef PTXCONF_NFSUTILS_CLIENT
	@$(call install_copy, nfsutils, 0, 0, 0755, $(NFSUTILS_DIR)/utils/mount/mount.nfs,  /usr/sbin/mount.nfs)
	@$(call install_link, nfsutils, mount.nfs, /usr/sbin/umount.nfs)
endif

ifdef PTXCONF_NFSUTILS_SERVER
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpc.nfsd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/rpc.mountd)
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/exportfs)

	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/etc/exports)
endif

ifdef PTXCONF_NFSUTILS_PYTHON
	@$(call install_copy, nfsutils, 0, 0, 0755, -, /usr/sbin/nfsiostat)
endif

#	#
#	# busybox init: start scripts
#	#
ifdef PTXCONF_NFSUTILS_NFSD_STARTSCRIPT
	@$(call install_alternative, nfsutils, 0, 0, 0755, /etc/init.d/nfsd)

ifneq ($(call remove_quotes,$(PTXCONF_NFSUTILS_NFSD_BBINIT_LINK)),)
	@$(call install_link, nfsutils, \
		../init.d/nfsd, \
		/etc/rc.d/$(PTXCONF_NFSUTILS_NFSD_BBINIT_LINK))
endif
endif

ifdef PTXCONF_NFSUTILS_SYSTEMD_UNIT
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/rpc-statd.service)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/rpc-statd-notify.service)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/var-lib-nfs.mount)
	@$(call install_link, nfsutils, ../var-lib-nfs.mount, \
		/usr/lib/systemd/system/local-fs.target.requires/var-lib-nfs.mount)

ifdef PTXCONF_NFSUTILS_CLIENT
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/nfs-client.target)
	@$(call install_link, nfsutils, ../nfs-client.target, \
		/usr/lib/systemd/system/multi-user.target.wants/nfs-client.target)
	@$(call install_link, nfsutils, ../nfs-client.target, \
		/usr/lib/systemd/system/remote-fs.target.wants/nfs-client.target)
endif
ifdef PTXCONF_NFSUTILS_SERVER
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/proc-fs-nfsd.mount)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/nfs-mountd.service)
	@$(call install_alternative, nfsutils, 0, 0, 0644, \
		/usr/lib/systemd/system/nfs-server.service)
	@$(call install_link, nfsutils, ../nfs-server.service, \
		/usr/lib/systemd/system/multi-user.target.wants/nfs-server.service)
endif
endif
	@$(call install_finish, nfsutils)
	@$(call touch)

# vim: syntax=make
