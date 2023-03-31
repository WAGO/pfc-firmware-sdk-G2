# -*-makefile-*-
#
# Copyright (C) 2021 by WAGO GmbH & Co. KG
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SSSD) += sssd

#
# Paths and names
#
SSSD_VERSION	:= 2.6.3
SSSD_MD5	:= 1c74763e6caccb78f8fe9c8c4dc65689
SSSD		:= sssd-$(SSSD_VERSION)
SSSD_SUFFIX	:= tar.gz
SSSD_URL	:= https://github.com/SSSD/sssd/archive/refs/tags/$(SSSD_VERSION).$(SSSD_SUFFIX)
SSSD_SOURCE	:= $(SRCDIR)/$(SSSD).$(SSSD_SUFFIX)
SSSD_DIR	:= $(BUILDDIR)/$(SSSD)
SSSD_LICENSE	:= GPL-3.0-or-later

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

#$(SSSD_SOURCE):
#	@$(call targetinfo)
#	@$(call get, SSSD)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/sssd.extract: $(STATEDIR)/autogen-tools
	@$(call targetinfo)
	@$(call clean, $(SSSD_DIR))
	@$(call extract, SSSD)
	@$(call patchin, SSSD)
	@cd $(SSSD_DIR) && [ -f configure ] || (autoreconf --install --force)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SSSD_CONF_ENV	:= $(CROSS_ENV)

#
# autoconf
#
SSSD_CONF_TOOL	:= autoconf

# NOTE: idmap interface version cannot be determed by autoconf,
#       as we are cross compiling, and thus no binary can be
#       executed to identify the version.
#       We do it by manually scraping it from source...
#       samba-4.9.5/source3/include/idmap.h
#           #define SMB_IDMAP_INTERFACE_VERSION 6

SSSD_CONF_OPT	:=  \
	$(CROSS_AUTOCONF_USR) \
	--exec-prefix=/usr \
	--disable-nls \
	--disable-rpath \
	--enable-nsslibdir=/usr/lib/ \
	--enable-pammoddir=/usr/lib/security/ \
	--disable-all-experimental-features \
	--disable-cifs-idmap-plugin \
	--disable-valgrind \
	--disable-valgrind-memcheck \
	--disable-valgrind-helgrind \
	--disable-valgrind-drd \
	--disable-valgrind-sgcheck \
	--with-gnu-ld \
	--without-nfsv4-idmapd-plugin \
	--without-manpages \
	--without-python2-bindings \
	--without-python3-bindings \
	--$(call ptx/wwo, PTXCONF_GLOBAL_SELINUX)-selinux \
	--$(call ptx/wwo, PTXCONF_GLOBAL_SELINUX)-semanage \
	--without-subid \
	--with-syslog=syslog \
	--with-samba \
	--with-ldb-lib-dir=/usr/modules/ldb/ \
	--with-smb-idmap-interface-version=6 \
	--without-libnl \
	--with-initscript=sysv \
	NSUPDATE=/usr/sbin/nsupdate

SSSD_LDFLAGS	:=  \
	-Wl,-rpath-link,$(PTXCONF_SYSROOT_TARGET)/usr/lib/samba

#$(STATEDIR)/sssd.prepare:
#	@$(call targetinfo)
#	@$(call clean, $(SSSD_DIR)/config.cache)
#	cd $(SSSD_DIR) && \
#		$(SSSD_PATH) $(SSSD_CONF_ENV) \
#		./configure $(SSSD_CONF_OPT) --prefix=$(PTXCONF_SYSROOT_TARGET)
#	#@$(call world/prepare, SSSD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd.compile:
#	@$(call targetinfo)
#	@$(call world/compile, SSSD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd.install:
#	@$(call targetinfo)
#	@$(call world/install, SSSD)
#	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sssd.targetinstall:
	@$(call targetinfo)

	@$(call install_init, sssd)
	@$(call install_fixup, sssd,PRIORITY,optional)
	@$(call install_fixup, sssd,SECTION,base)
	@$(call install_fixup, sssd,AUTHOR,"WAGO GmbH \& Co. KG")
	@$(call install_fixup, sssd,DESCRIPTION,missing)

#	#@$(call install_copy, sssd, 0, 0, 0755, /etc/dbus-1/system.d/)
#	#@$(call install_copy, sssd, 0, 0, 0755, /etc/dbus-1/system.d/org.freedesktop.sssd.infopipe.conf)

#	#@$(call install_copy, sssd, 0, 0, 0755, /usr/share/dbus-1/system-services/)
#	#@$(call install_copy, sssd, 0, 0, 0755, /usr/share/dbus-1/system-services/org.freedesktop.sssd.infopipe.service)

#	#
#	# /var/lib/sss/*
#	#
	@$(call install_copy, sssd, 0, 0, 0750, /var/lib/sss/pipes/private/)
	@$(call install_copy, sssd, 0, 0, 0700, /var/lib/sss/db/)
	@$(call install_copy, sssd, 0, 0, 0700, /var/lib/sss/keytabs/)
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/mc/)
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/pipes/)
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/pubconf/)
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/pubconf/krb5.include.d/)
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/gpo_cache/)
#	# FIXME: CHECK PERMISSIONS
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/deskprofile/)
#	# FIXME: CHECK PERMISSIONS
	@$(call install_copy, sssd, 0, 0, 0755, /var/lib/sss/secrets/)

#	#
#	# /var/lib/sssd/*
#	#
#	# FIXME: CHECK PERMISSIONS
	@$(call install_copy, sssd, 0, 0, 0755, /usr/lib/sssd/conf/)

#	#
#	# /etc/sssd/*
#	#
	@$(call install_copy, sssd, 0, 0, 0711, /etc/sssd/)
#	# FIXME: CHECK PERMISSIONS
	@$(call install_alternative, sssd, 0, 0, 0600, /etc/sssd/sssd.conf)
	@$(call install_copy, sssd, 0, 0, 0711, /etc/sssd/conf.d/)
	@$(call install_copy, sssd, 0, 0, 0711, /etc/sssd/pki/)

#	#
#	# /usr/share/sssd/*
#	#
	@$(call install_copy, sssd, 0, 0, 0711, /usr/share/sssd/)

	@$(call install_copy, sssd, 0, 0, 0600, -, /usr/share/sssd/cfg_rules.ini)

	@$(call install_copy, sssd, 0, 0, 0600, -, /usr/share/sssd/sssd.api.conf)
	@$(call install_copy, sssd, 0, 0, 0711, /usr/share/sssd/sssd.api.d/)
	@$(call install_tree, sssd, 0, 0, -, /usr/share/sssd/sssd.api.d/)

#	#
#	# /usr/lib/*
#	#
	@$(call install_lib, sssd, 0, 0, 0644, libipa_hbac)
	@$(call install_lib, sssd, 0, 0, 0644, libsss_idmap)
	@$(call install_lib, sssd, 0, 0, 0644, libsss_nss_idmap)
	@$(call install_lib, sssd, 0, 0, 0644, libsss_certmap)
	@$(call install_lib, sssd, 0, 0, 0644, libsss_simpleifp)
	@$(call install_lib, sssd, 0, 0, 0644, libsss_sudo)
	@$(call install_lib, sssd, 0, 0, 0644, libnss_sss)

#	#
#	# /usr/lib/*
#	#
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_util.so, /usr/lib/libsss_util.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_cert.so, /usr/lib/libsss_cert.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_crypt.so, /usr/lib/libsss_crypt.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_child.so, /usr/lib/libsss_child.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_debug.so, /usr/lib/libsss_debug.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_iface.so, /usr/lib/libsss_iface.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_iface_sync.so, /usr/lib/libsss_iface_sync.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_sbus.so, /usr/lib/libsss_sbus.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_ldap_common.so, /usr/lib/libsss_ldap_common.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_krb5_common.so, /usr/lib/libsss_krb5_common.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_sbus_sync.so, /usr/lib/libsss_sbus_sync.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libifp_iface.so, /usr/lib/libifp_iface.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libifp_iface_sync.so, /usr/lib/libifp_iface_sync.so)
	@$(call install_copy, sssd, 0, 0, 0644, $(SSSD_DIR)/.libs/libsss_semanage.so, /usr/lib/libsss_semanage.so)

#	#
#	# /usr/lib/sssd/*
#	#
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_simple.so)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_ldap.so)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_files.so)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_krb5.so)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_proxy.so)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_ad.so)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/libsss_ipa.so)

#	#
#	# /usr/lib/samba/idmap/*
#	#
	@$(call install_copy, sssd, 0, 0, 0755, /usr/lib/samba/idmap/)
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/samba/idmap/sss.so)

#	#
#	# /usr/bin/*
#	#
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/bin/sss_ssh_authorizedkeys)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/bin/sss_ssh_knownhostsproxy)

#	#
#	# /usr/sbin/*
#	#
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/sbin/sssd)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/sbin/sss_cache)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/sbin/sss_override)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/sbin/sss_seed)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/sbin/sssctl)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/sbin/sss_debuglevel)

#	#
#	# /usr/libexec/sssd/*
#	#
	@$(call install_copy, sssd, 0, 0, 0755, /usr/libexec/sssd/)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_nss)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_pam)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_be)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/krb5_child)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/ldap_child)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/proxy_child)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sss_signal)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_sudo)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_autofs)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_ssh)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_ifp)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/gpo_child)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/p11_child)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_kcm)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sssd_pac)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/libexec/sssd/sss_analyze)

#	#
#	# /usr/lib/sssd/modules/*
#	#
	@$(call install_copy, sssd, 0, 0, 0755, /usr/lib/sssd/modules/)
	@$(call install_copy, sssd, 0, 0, 0755, -, \
					/usr/lib/sssd/modules/sssd_krb5_localauth_plugin.so)

#	#
#	# install "autofs" module
#	#
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/lib/sssd/modules/libsss_autofs.so)

#	#
#	# configuration for Kerberos cache
#	#
	@$(call install_copy, sssd, 0, 0, 0755, /usr/share/sssd-kcm/)
	@$(call install_alternative, sssd, 0, 0, 0600, /usr/share/sssd-kcm/kcm_default_ccache)

#	#
#	# /usr/lib/krb5/plugins/* 
#	#
	@$(call install_copy, sssd, 0, 0, 0755, /usr/lib/krb5/plugins/authdata/)
	@$(call install_copy, sssd, 0, 0, 0644, -, \
					/usr/lib/krb5/plugins/authdata/sssd_pac_plugin.so)
	@$(call install_copy, sssd, 0, 0, 0755, /usr/lib/krb5/plugins/libkrb5/)
	@$(call install_copy, sssd, 0, 0, 0644, -, \
					/usr/lib/krb5/plugins/libkrb5/sssd_krb5_locator_plugin.so)

#	#
#	# install module(s) for "ldb"
#	#
	@$(call install_copy, sssd, 0, 0, 0644, -, /usr/modules/ldb/memberof.so)

#	#
#	# install Linux-PAM modules to /usr/lib/security/
#	#
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/lib/security/pam_sss.so)
	@$(call install_copy, sssd, 0, 0, 0755, -, /usr/lib/security/pam_sss_gss.so)

#	#
#	# busybox init: start script
#	#
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_SSSD_STARTSCRIPT
	@$(call install_alternative, sssd, 0, 0, 0755, /etc/init.d/sssd)

ifneq ($(call remove_quotes, $(PTXCONF_SSSD_BBINIT_LINK)),)
	@$(call install_link, sssd, ../init.d/sssd, /etc/rc.d/$(PTXCONF_SSSD_BBINIT_LINK))
endif
endif
endif

	@$(call install_finish, sssd)

	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/sssd.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, SSSD)

# vim: syntax=make
