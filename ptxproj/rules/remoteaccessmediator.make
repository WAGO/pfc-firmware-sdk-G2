# -*-makefile-*-
#
# Copyright (C) 2018 by M&M Software GmbH
#
# See CREDITS for details about who has contributed to this project.
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
ifndef PTXCONF_REMOTEACCESSMEDIATOR_SKIP_TARGETINSTALL
PACKAGES-$(PTXCONF_REMOTEACCESSMEDIATOR) += remoteaccessmediator
else
LAZY_PACKAGES-$(PTXCONF_REMOTEACCESSMEDIATOR) += remoteaccessmediator
endif

#
# Paths and names
#
# Version will be replaced automatically by Rebuild script. The version has only to be updated in buil/app_version.txt
REMOTEACCESSMEDIATOR_VERSION	:= 0.12.0.0
REMOTEACCESSMEDIATOR_MD5	:=
REMOTEACCESSMEDIATOR		:= remoteaccessmediator-$(REMOTEACCESSMEDIATOR_VERSION)
REMOTEACCESSMEDIATOR_URL	:= file://wago_intern/remoteaccessmediator
REMOTEACCESSMEDIATOR_DIR	:= $(BUILDDIR)/$(REMOTEACCESSMEDIATOR)
REMOTEACCESSMEDIATOR_SRC	:= $(PTXDIST_WORKSPACE)/wago_intern/remoteaccessmediator
REMOTEACCESSMEDIATOR_BUILDCONFIG:= Release
REMOTEACCESSMEDIATOR_LICENSE	:= proprietary
REMOTEACCESSMEDIATOR_IN_PKGDIR	:= $(PKGDIR)/$(REMOTEACCESSMEDIATOR)
REMOTEACCESSMEDIATOR_PACKAGE_NAME := $(REMOTEACCESSMEDIATOR)_$(PTXDIST_IPKG_ARCH_STRING)
REMOTEACCESSMEDIATOR_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages

#
# cmake
#
REMOTEACCESSMEDIATOR_CONF_ENV	:= $(CROSS_ENV)  
REMOTEACCESSMEDIATOR_CONF_TOOL	:= cmake
REMOTEACCESSMEDIATOR_CONF_OPT	:= $(CROSS_CMAKE_USR) -D"CMAKE_CXX_FLAGS=--std=c++11"  

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/remoteaccessmediator.extract.post:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/remoteaccessmediator.prepare:
	@$(call targetinfo)
	@$(call touch)
endif

# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
$(STATEDIR)/remoteaccessmediator.compile:
	@$(call targetinfo)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	@$(call world/compile, REMOTEACCESSMEDIATOR)
endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------
$(STATEDIR)/remoteaccessmediator.install:
	@$(call targetinfo)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
$(STATEDIR)/remoteaccessmediator.extract: 
	@$(call targetinfo)
#   # BSP mode: install ipk contents to packages/remoteaccessmediator
	@mkdir -p $(REMOTEACCESSMEDIATOR_IN_PKGDIR) && tar xvzf $(REMOTEACCESSMEDIATOR_PLATFORMCONFIGPACKAGEDIR)/$(REMOTEACCESSMEDIATOR_PACKAGE_NAME).tgz -C $(REMOTEACCESSMEDIATOR_IN_PKGDIR)
	@$(call touch)

else

#   # "normal" mode: call "make install"
	@mkdir -p $(REMOTEACCESSMEDIATOR_IN_PKGDIR)
	@install -m 0644 $(REMOTEACCESSMEDIATOR_DIR)-build/remoteaccessmediator $(REMOTEACCESSMEDIATOR_IN_PKGDIR)
	@install -m 0644 $(REMOTEACCESSMEDIATOR_SRC)/etc/logging.config $(REMOTEACCESSMEDIATOR_IN_PKGDIR)
	@install -m 0644 $(REMOTEACCESSMEDIATOR_SRC)/etc/config.json $(REMOTEACCESSMEDIATOR_IN_PKGDIR)
	@install -m 0644 $(REMOTEACCESSMEDIATOR_SRC)/etc/ramediator $(REMOTEACCESSMEDIATOR_IN_PKGDIR)
	@install -m 0644 $(REMOTEACCESSMEDIATOR_SRC)/websocketpp/license.txt $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/websocketpp_license.txt
	@install -m 0644 $(REMOTEACCESSMEDIATOR_SRC)/easylogging/license.txt $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/easylogging_license.txt
	@install -m 0644 $(REMOTEACCESSMEDIATOR_SRC)/rapidjson/license.txt $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/rapidjson_license.txt

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
#   # save package contents for BSP
	@cd $(REMOTEACCESSMEDIATOR_IN_PKGDIR) && tar cvzf $(REMOTEACCESSMEDIATOR_PLATFORMCONFIGPACKAGEDIR)/$(REMOTEACCESSMEDIATOR_PACKAGE_NAME).tgz *
endif

endif

	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/remoteaccessmediator.targetinstall:
	@$(call targetinfo)

	@$(call install_init, remoteaccessmediator)
	@$(call install_fixup, remoteaccessmediator, PRIORITY, optional)
	@$(call install_fixup, remoteaccessmediator, SECTION, base)
	@$(call install_fixup, remoteaccessmediator, AUTHOR, "M\&M Software GmbH")
	@$(call install_fixup, remoteaccessmediator, DESCRIPTION, Mediator for remote access service)

	@$(call install_copy, remoteaccessmediator, 0, 0, 0755, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/remoteaccessmediator, /usr/bin/ramediator)
	@$(call install_copy, remoteaccessmediator, 0, 0, 0644, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/logging.config, /etc/remoteaccessmediator/logging.config)
	@$(call install_copy, remoteaccessmediator, 0, 0, 0644, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/config.json, /etc/remoteaccessmediator/config.json)
	@$(call install_copy, remoteaccessmediator, 0, 0, 0755, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/ramediator, /etc/init.d/ramediator)
	@$(call install_copy, remoteaccessmediator, 0, 0, 0644, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/websocketpp_license.txt, /usr/share/licenses/oss/license.websocketpp_0.8.1.txt)
	@$(call install_copy, remoteaccessmediator, 0, 0, 0644, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/easylogging_license.txt, /usr/share/licenses/oss/license.easyloggingpp_9.96.7.txt)
	@$(call install_copy, remoteaccessmediator, 0, 0, 0644, $(REMOTEACCESSMEDIATOR_IN_PKGDIR)/rapidjson_license.txt, /usr/share/licenses/oss/license.rapidjson_1.1.0.txt)

	@$(call install_finish, remoteaccessmediator)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

#$(STATEDIR)/remoteaccessmediator.clean:
#	@$(call targetinfo)
#	@$(call clean_pkg, remoteaccessmediator)

# vim: syntax=make
