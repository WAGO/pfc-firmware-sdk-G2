# -*-makefile-*-
#
# Copyright (C) 2004 by BSP
#           (C) 2010 by Michael Olbrich <m.olbrich@pengutronix.de>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_SUDO) += sudo

#
# Paths and names
#
SUDO_VERSION	:= 1.9.15p5
SUDO_MD5	:= 4166279cb188ecb6641c7a2ba5f68270
SUDO		:= sudo-$(SUDO_VERSION)
SUDO_SUFFIX	:= tar.gz
SUDO_URL	:= \
	http://www.sudo.ws/sudo/dist/$(SUDO).$(SUDO_SUFFIX) \
	http://www.sudo.ws/sudo/dist/OLD/$(SUDO).$(SUDO_SUFFIX)
SUDO_SOURCE	:= $(SRCDIR)/$(SUDO).$(SUDO_SUFFIX)
SUDO_DIR	:= $(BUILDDIR)/$(SUDO)
SUDO_LICENSE	:= ISC AND BSD-3-Clause AND BSD-2-Clause-NetBSD AND Zlib
SUDO_LICENSE_FILES := file://LICENSE.md;md5=5100e20d35f9015f9eef6bdb27ba194f

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

SUDO_CONF_ENV	:= \
	$(CROSS_ENV) \
	sudo_cv_func_fnmatch=yes \
	sudo_cv_func_unsetenv_void=no \
	ac_cv_have_working_snprintf=yes \
	ac_cv_have_working_vsnprintf=yes

#
# autoconf
#
SUDO_CONF_TOOL	:= autoconf
SUDO_CONF_OPT	:= \
	$(CROSS_AUTOCONF_USR) \
	--enable-authentication \
	--disable-root-mailer \
	--enable-setreuid \
	--enable-setresuid \
	--enable-shadow \
	--enable-root-sudo \
	--disable-log-host \
	--enable-noargs-shell \
	--disable-shell-sets-home \
	--disable-path-info \
	--disable-env-debug \
	--enable-zlib \
	--disable-env-reset \
	--enable-warnings \
	--disable-werror \
	--disable-openssl \
	--disable-gcrypt \
	--enable-hardening \
	--enable-pie \
	--enable-poll \
	--disable-admin-flag \
	--disable-nls \
	--disable-rpath \
	--enable-static-sudoers \
	--disable-shared-libutil \
	--disable-tmpfiles.d \
	--disable-devsearch \
	--disable-sasl \
	--disable-offensive-insults \
	--disable-package-build \
	--disable-gss-krb5-ccache-name \
	--disable-pvs-studio \
	--disable-log-server \
	--enable-log-client \
	--disable-sia \
	$(GLOBAL_LARGE_FILE_OPTION) \
	--$(call ptx/endis, PTXCONF_SUDO_PAM)-pam-session \
	--disable-python \
	--disable-kerb5-instance \
	--without-AFS \
	--without-DCE \
	--without-logincap \
	--without-bsdauth \
	--without-project \
	--without-lecture \
	--with-ignore-dot \
	--$(call ptx/wwo, PTXCONF_SUDO_PAM)-pam

ifdef PTXCONF_SUDO_USE_SENDMAIL
SUDO_CONF_OPT += --with-sendmail
else
SUDO_CONF_OPT += --without-sendmail
endif

ifdef PTXCONF_SUDO_INSTALL_VISUDO
ifneq ($(PTXCONF_SUDO_DEFAULT_EDITOR),"")
SUDO_CONF_OPT += --with-editor=$(PTXCONF_SUDO_DEFAULT_EDITOR)
endif
endif

ifdef PTXCONF_SUDO_USE_ENV_EDITOR
SUDO_CONF_OPT += --with-env-editor
else
SUDO_CONF_OPT += --without-env-editor
endif

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/sudo.targetinstall:
	@$(call targetinfo)

	@$(call install_init,  sudo)
	@$(call install_fixup, sudo,PRIORITY,optional)
	@$(call install_fixup, sudo,SECTION,base)
	@$(call install_fixup, sudo,AUTHOR,"Carsten Schlote <c.schlote@konzeptpark.de>")
	@$(call install_fixup, sudo,DESCRIPTION,missing)

	@$(call install_copy, sudo, 0, 0, 7755, -, /usr/bin/sudo)
	@$(call install_link, sudo, sudo, /usr/bin/sudoedit)

	@$(call install_copy, sudo, 0, 0, 0644, -, \
		/usr/libexec/sudo/sudo_noexec.so)
	@$(call install_copy, sudo, 0, 0, 0644, -, \
		/usr/libexec/sudo/group_file.so)
	@$(call install_copy, sudo, 0, 0, 0644, -, \
                /usr/libexec/sudo/system_group.so)

ifdef PTXCONF_SUDO_INSTALL_ETC_SUDOERS
	@$(call install_alternative, sudo, 0, 0, 0440, /etc/sudoers, n)
	@$(call install_copy, sudo, 0, 0, 0755, /etc/sudoers.d)
endif

ifdef PTXCONF_SUDO_INSTALL_VISUDO
	@$(call install_copy, sudo, 0, 0, 0755, -, /usr/sbin/visudo)
endif

ifdef PTXCONF_SUDO_PAM
	@$(call install_alternative, sudo, 0, 0, 0644, /etc/pam.d/sudo)
endif

	@$(call install_finish, sudo)
	@$(call touch)

# vim: syntax=make
