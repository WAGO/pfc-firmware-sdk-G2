## -*-makefile-*-
# $Id$
#
# Copyright (C) 2013 by WAGO GmbH & Co. KG
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_CODESYS3) += codesys3

CODESYS3_VERSION    := 6.3.0.5
CODESYS3            := codesys-3
CODESYS3_DIR        := $(BUILDDIR)/$(CODESYS3)
ifdef PTXCONF_CODESYS3_SOURCE_LOCAL
CODESYS3_SRC        := $(PTXCONF_CODESYS3_SOURCE_LOCAL_PATH)/
else
CODESYS3_SRC_RELATIVE := wago_intern/plc/codesys/$(CODESYS3)/
CODESYS3_SRC        := $(PTXDIST_WORKSPACE)/$(CODESYS3_SRC_RELATIVE)
endif

CODESYS3_PACKAGE_NAME := codesys3_$(CODESYS3_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CODESYS3_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CODESYS3_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CODESYS3_PACKAGE_NAME)
CODESYS3_INCLUDE := $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3

ifdef PTXCONF_CODESYS3_SOURCE_GIT
CODESYS3_GIT_URL         := https://tfs-eng:8081/tfs/ProductDevelopment/eRuntime/_git/eRuntime
endif

ifdef PTXCONF_CODESYS3_SOURCE_ARTIFACTORY
CODESYS3_URL        := $(call jfrog_template_to_url, CODESYS3)

CODESYS3_SUFFIX          := $(suffix $(CODESYS3_URL))
CODESYS3_MD5              = $(shell [ -f $(CODESYS3_MD5_FILE) ] && cat $(CODESYS3_MD5_FILE))
CODESYS3_MD5_FILE        := wago_intern/artifactory_sources/$(CODESYS3)$(CODESYS3_SUFFIX).md5
CODESYS3_ARCHIVE         := $(CODESYS3)-$(CODESYS3_VERSION)$(CODESYS3_SUFFIX)
endif

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

ifdef PTXCONF_CODESYS3_SOURCE_GIT

$(STATEDIR)/codesys3.get: | $(CODESYS3_SRC)
	@$(call targetinfo)

$(CODESYS3_SRC):
	{ git clone --branch $(PTXCONF_CODESYS3_SOURCE_GIT_BRANCH) $(CODESYS3_GIT_URL) $(CODESYS3_SRC); } \
	|| rm -rf $(CODESYS3_SRC)
	@$(call touch)

	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    "https://artifactory.wago.local/artifactory/codesys-generic-prod-local/3S/WAGO_BUNDLE/WAGO_BUNDLE_$$(cat $(CODESYS3_SRC)/wago_bundle_version.txt).zip" \
	'$(CODESYS3_SRC_RELATIVE)/jenkins/wago_bundle.zip' \
	'$(CODESYS3_SRC_RELATIVE)/jenkins/wago_bundle.md5'

	@$(CODESYS3_SRC)/jenkins/buildjob.sh $(CODESYS3_SRC)/jenkins/wago_bundle.zip > /dev/null
endif

ifdef PTXCONF_CODESYS3_SOURCE_ARTIFACTORY
$(STATEDIR)/codesys3.get:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call ptx/in-path, PTXDIST_PATH, scripts/wago/artifactory.sh) fetch \
    '$(CODESYS3_URL)' 'wago_intern/artifactory_sources/$(CODESYS3_ARCHIVE)' '$(CODESYS3_MD5_FILE)'
endif

	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/codesys3.extract:
	@$(call targetinfo)

	mkdir -p $(CODESYS3_DIR)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

ifdef PTXCONF_CODESYS3_SOURCE_ARTIFACTORY
	@tar xvf wago_intern/artifactory_sources/$(CODESYS3_ARCHIVE) -C $(CODESYS3_DIR)
else
	rsync --copy-unsafe-links -a --exclude=Documentation \
		--exclude=Templates/ \
		--exclude=RtsConfigurator/ \
		--exclude=Placeholder \
		--exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CODESYS3_SRC) $(CODESYS3_DIR)
endif
endif
	@$(call patchin, CODESYS3)
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# AGa: only include CROSS_ENV_PROGS to env as 3S' Makefile doesn't support providing CFLAGs and friends
#     via command line out of the box. Try to alter 3S' code as little as possible (override directive
#     would be way to go otherwise).
#

$(STATEDIR)/codesys3.prepare:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES    
	@cd $(CODESYS3_DIR)
	grep -e PTXCONF_CDS3_RTS_ $(PTXDIST_PTXCONFIG) | \
		sed -e 's/PTXCONF_CDS3_RTS_/RTS_/g' > $(CODESYS3_DIR)/buildconfig
ifeq ($(PTXCONF_PLATFORM), vtp-ctp)
	sed -i s/"cflags QtGui QtWebKit"/"cflags Qt5Gui Qt5Widgets"/ $(CODESYS3_DIR)/script/visu.mk
	sed -i "s,moc ,$(PTXCONF_SYSROOT_TARGET)/usr/bin/qt5/moc ,g" $(CODESYS3_DIR)/script/visu.mk
endif

endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CODESYS3_MAKE_ENV:= $(CROSS_ENV) ARCH=$(PTXCONF_ARCH_STRING)
$(STATEDIR)/codesys3.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, CODESYS3)
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/codesys3.install:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call world/install, CODESYS3)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup headers for later use in configs/@platform@/packages
	cd $(PKGDIR)/$(CODESYS3)/ && \
	tar -czvf $(CODESYS3).tgz * && \
	mv $(CODESYS3).tgz $(CODESYS3_PLATFORMCONFIGPACKAGEDIR)/
endif
	# Install codesys3 header files 
	mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3
	rsync -a --include='*.h' \
	      --include='*.m4' \
	      -f 'hide,! */' \
	      $(CODESYS3_DIR)/Components/ $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/

	rsync -a --include='*.h' \
	      --include='*.m4' \
	       -f 'hide,! */' \
	       $(CODESYS3_DIR)/Platforms/Linux/ $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/

	# Install m4 definitions
	mkdir -p $(PTXCONF_SYSROOT_TARGET)/usr/share/M4Defs
	rsync -a --include='*.m4' \
                $(CODESYS3_DIR)/BuildUtils/M4Defs/ $(PTXCONF_SYSROOT_TARGET)/usr/share/M4Defs
endif
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# Recover header from archive in configs/@platform@/packages
	mkdir -p $(PKGDIR)/$(CODESYS3)
	tar -xzvf $(CODESYS3_PLATFORMCONFIGPACKAGEDIR)/$(CODESYS3).tgz -C $(PKGDIR)/$(CODESYS3)
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------


ifdef PTXCONF_CDS3_RTS_CGROUPS_SUPPORT
CGROUP_ENV="if [ -e /etc/cgroup.env ]; then\n\tsource /etc/cgroup.env\nfi\nexport PTHREADCGROUP_DEFAULT_PATH=rts/def"
else
CGROUP_ENV=
endif

$(STATEDIR)/codesys3.targetinstall:
	@$(call targetinfo)
	@$(call install_init, codesys3)

	@$(call install_fixup,codesys3,PRIORITY,optional)
	@$(call install_fixup,codesys3,SECTION,base)
	@$(call install_fixup,codesys3,AUTHOR,"3S")
	@$(call install_fixup,codesys3,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CODESYS3_PACKAGE_DIR)
	cd $(CODESYS3_PACKAGE_DIR) && \
	ar -xov $(CODESYS3_PLATFORMCONFIGPACKAGEDIR)/$(CODESYS3_PACKAGE_NAME).ipk
	@$(call install_archive, codesys3, -, -, $(CODESYS3_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@$(call install_copy, codesys3, 0, 0, 0644, $(CODESYS3_DIR)/CoDeSysSP.cfg, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(PTXCONF_CDS3_PLCCONFIGFILE), n)
	@$(call install_copy, codesys3, 0, 0, 0755, $(CODESYS3_DIR)/bin/codesys3, /usr/bin/codesys3, y)
#ifdef PTXCONF_INITMETHOD_BBINIT
#	@$(call install_replace, codesys3, /etc/init.d/runtime, @CDS3_PLCCONFIGFILE@, $(PTXCONF_CDS3_PLCCONFIGFILE));
#	@$(call install_replace, codesys3, /etc/init.d/runtime, @CDS3_CGROUPS@, $(CGROUP_ENV))
#endif

ifdef PTXCONF_INITMETHOD_SYSTEMD
	@$(call install_alternative, codesys3, 0, 0, 0755, /etc/systemd/system/codesys3.service, n)
# comment the following line to have codesys removed from systemd autostart
	@$(call install_link, codesys3, ../codesys3.service, /etc/systemd/system/multi-user.target.wants/codesys3.service)
#	@$(call install_alternative, codesys3, 0, 0, 0755, /etc/systemd/codesys3.env, n)
#	@$(call install_replace, codesys3, /etc/systemd/codesys3.env, @CDS3_PLCCONFIGFILE@, $(PTXCONF_CDS3_PLCCONFIGFILE))
endif

	# Install universal 3S.dat
	@$(call install_alternative, codesys3, 0, 0, 0444, /usr/share/codesys3/3S.default.dat, n)

    # Codesys home
ifdef PTXCONF_CDS3_HOME
# Set setgid bit so that every file created in /home/codesys_root will have group 'admin'. 
	@$(call install_copy, codesys3, 0, 0, 0755, /home)
	@$(call install_copy, codesys3, 0, $(PTXCONF_ROOTFS_PASSWD_ADMIN_GID), 2775, /home/codesys_root)
	@$(call install_link, codesys3, /home/codesys_root, /home/codesys)
	@$(call install_replace, codesys3, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(PTXCONF_CDS3_PLCCONFIGFILE), @CDS3_HOME@, $(PTXCONF_CDS3_HOME_PATH));

# NOTE: To preserve the permissions set above for /home/codesys_root all files in this directory have to be installed within one ipkg package
	@$(call install_copy, codesys3, ${PTXCONF_ROOTFS_PASSWD_ADMIN_UID}, ${PTXCONF_ROOTFS_PASSWD_ADMIN_GID}, \
     600, $(PTXDIST_WORKSPACE)/projectroot/root/.profile, /home/codesys_root/.profile)
endif

ifdef PTXCONF_CDS3_RETAIN_TYPE_CFG
	@$(call install_replace, codesys3, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(PTXCONF_CDS3_PLCCONFIGFILE), @CDS3_RETAIN_TYPE@, $(PTXCONF_CDS3_RETAIN_TYPE_CFG));
endif

ifdef PTXCONF_CDS3_IEC_CORE_SET
	@$(call install_replace, codesys3, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(PTXCONF_CDS3_PLCCONFIGFILE), @CDS3_IEC_CORE_SET@, $(PTXCONF_CDS3_IEC_CORE_SET));
endif

#ifdef PTXCONF_CDS3_SECURE_LOGIN_ENABLE
#	@$(call install_replace, codesys3, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(PTXCONF_CDS3_PLCCONFIGFILE), @CDS3_SECURE_LOGIN_ENABLE@, $(PTXCONF_CDS3_SECURE_LOGIN_ENABLE));
#endif


	@cd $(CODESYS3_DIR)/cmp/ && \
	for file in `find -name "*.so*"`; do \
	if [[ "$$(basename $$file)" == *"libCmpOPCUA"* ]] && [[ $(call ptx/endis, PTXCONF_OPCUASERVER) == "enable" ]]; then \
	    echo "INFO wago opcua server is enabled: did not install: "$$(basename $$file); \
	  else \
	    if [[ -h $$file ]]; then \
	      if [[ "$$(basename $$file)" != "libCmpBlkDrvCom.so" ]]; then \
	        $(call install_link, codesys3, ./$$(readlink $$file), /usr/lib/$$file); \
	       fi \
	    elif [[ -f $$file ]]; then \
	      $(call install_copy, codesys3, 0, 0, 0755, $(CODESYS3_DIR)/cmp/$$file, /usr/lib/$$file); \
	    fi; \
	    if [[ $$file == *.so ]]; then \
	      if [[ "$$(basename $$file)" != "libCmpBlkDrvCom.so" ]]; then \
	        $(call install_link, codesys3, ../$$file, /usr/lib/cds3-custom-components/$$file); \
	      fi; \
	    fi; \
	  fi; \
	done

	@cd $(CODESYS3_DIR)/lib/ && \
	if [ -h libcds3sys.so ]; then \
	  $(call install_copy, codesys3, 0, 0, 0750, $$(readlink libcds3sys.so), /usr/lib/libcds3sys.so); \
	fi

	@cd $(CODESYS3_DIR)/lib/ && \
	if [ -h libcds3uastack.so ]; then \
	  $(call install_copy, codesys3, 0, 0, 0750, $(CODESYS3_DIR)/lib/$$(readlink libcds3uastack.so), /usr/lib/libcds3uastack.so); \
	fi

endif
	@$(call install_finish,codesys3)
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CODESYS3_PACKAGE_NAME).ipk $(CODESYS3_PLATFORMCONFIGPACKAGEDIR)/
endif

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
ifdef PTXCONF_CDS3_RTS_FEATURE_OPCUA
	$(PTXDIST_WORKSPACE)/scripts/opcuaserver-helpers/make-metaipk-3s.sh
endif
endif
	@$(call touch)
	


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/codesys3.clean:
	rm -rf $(STATEDIR)/codesys3.*
	rm -rf $(PKGDIR)/codesys3_*
	rm -rf $(CODESYS3_DIR)
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/tests
	rm -rf $(PTXCONF_SYSROOT_TARGET)/usr/lib/codesys3/tests
ifdef PTXCONF_CODESYS3_SOURCE_ARTIFACTORY
	rm -rf $(PTXDIST_WORKSPACE)/wago_intern/artifactory_sources/$(CODESYS3_ARCHIVE)
endif

