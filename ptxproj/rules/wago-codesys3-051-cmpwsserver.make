# -*-makefile-*-
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
PACKAGES-$(PTXCONF_CDS3_CMPWSSERVER) += cds3-cmpwsserver

CDS3_CMPWSSERVER_VERSION   := 2.0.19
CDS3_CMPWSSERVER           := CmpWSServer
CDS3_CMPWSSERVER_DIR       := $(BUILDDIR)/$(CDS3_CMPWSSERVER)
CDS3_CMPWSSERVER_URL       := file://$(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPWSSERVER)
CDS3_CMPWSSERVER_SRC_DIR   := $(PTXDIST_WORKSPACE)/wago_intern/codesys3-Component/$(CDS3_CMPWSSERVER)
CDS3_CMPWSSERVER_SRC       := $(CDS3_CMPWSSERVER_SRC_DIR)/$(CDS3_CMPWSSERVER)
CDS3_CMPWSSERVER_BIN       := lib$(CDS3_CMPWSSERVER).so.$(CDS3_CMPWSSERVER_VERSION)

CDS3_CMPWSSERVER_PACKAGE_NAME := cds3-cmpwsserver_$(CDS3_CMPWSSERVER_VERSION)_$(PTXDIST_IPKG_ARCH_STRING)
CDS3_CMPWSSERVER_PLATFORMCONFIGPACKAGEDIR := $(PTXDIST_PLATFORMCONFIGDIR)/packages
CDS3_CMPWSSERVER_PACKAGE_DIR := $(PTXDIST_TEMPDIR)/package/$(CDS3_CMPWSSERVER_PACKAGE_NAME)
CDS3_CMPWSSERVER_WS_DIR		:=/var/www/ws

# ----------------------------------------------------------------------------
# Get
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpwsserver.get:
	@$(call targetinfo)
	@$(call touch)

# ----------------------------------------------------------------------------
# Extract
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpwsserver.extract:
	@$(call targetinfo)
	mkdir -p $(CDS3_CMPWSSERVER_DIR)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
 	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

#normally this is needed to use but the old schroot does not have unzip so we have to go a workaround via tar.gz
	rsync -a --exclude=.svn/ \
		--exclude=".*" \
		--exclude="*.d" \
		--exclude="*.o" \
		--exclude="*.pdf"  \
		--exclude="*tar.bz2" \
		$(CDS3_CMPWSSERVER_SRC_DIR)/ $(CDS3_CMPWSSERVER_DIR)/

endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-cmpwsserver.prepare:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) syserror
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) cmpelrestopcuaclient
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) parameterserver
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) cmpparameterserver
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) syspara
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) itf
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) lib
	$(MAKE) -C $(CDS3_CMPWSSERVER_DIR) SYSROOT=$(PTXCONF_SYSROOT_TARGET) dep
endif
	@$(call touch)


# ----------------------------------------------------------------------------
# Compile
# ----------------------------------------------------------------------------
CDS3_CMPWSSERVER_PATH      := PATH=$(CROSS_PATH)
CDS3_CMPWSSERVER_MAKE_ENV  := $(CROSS_ENV)
CDS3_CMPWSSERVER_MAKE_OPT  := CC=$(CROSS_CC)

$(STATEDIR)/cds3-cmpwsserver.compile:
	@$(call targetinfo)
ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	$(call world/compile, CDS3_CMPWSSERVER)
	$(HOSTPYTHON3) -m jsmin $(CDS3_CMPWSSERVER_DIR)/eWS_full.js > $(CDS3_CMPWSSERVER_DIR)/eWS.js
endif
	@$(call touch)

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

CMPWSSERVER_SYSROOT_INCLUDES= $(PTXCONF_SYSROOT_TARGET)/usr/include/CmpWSServer
$(STATEDIR)/cds3-cmpwsserver.install:
	@$(call targetinfo)

	@mkdir -p $(CMPWSSERVER_SYSROOT_INCLUDES)

ifndef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cp $(CDS3_CMPWSSERVER_DIR)/*.h $(CMPWSSERVER_SYSROOT_INCLUDES)/
	@cp $(CDS3_CMPWSSERVER_DIR)/*Itf.h $(PTXCONF_SYSROOT_TARGET)/usr/include/codesys3/
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	@cd $(CDS3_CMPWSSERVER_DIR) && tar cvzf $(CDS3_CMPWSSERVER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPWSSERVER_PACKAGE_NAME).tgz *.h
endif
else
	#PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES - Install header from archive
	@tar -xzvf $(CDS3_CMPWSSERVER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPWSSERVER_PACKAGE_NAME).tgz -C $(CMPWSSERVER_SYSROOT_INCLUDES)
endif
	@$(call touch)
	

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/cds3-cmpwsserver.targetinstall:
	@$(call targetinfo)
	@$(call install_init, cds3-cmpwsserver)

	@$(call install_fixup,cds3-cmpwsserver,PRIORITY,optional)
	@$(call install_fixup,cds3-cmpwsserver,SECTION,base)
	@$(call install_fixup,cds3-cmpwsserver,AUTHOR,"WAGO")
	@$(call install_fixup,cds3-cmpwsserver,DESCRIPTION,missing)

ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_BINARIES

	# Extract precompiled binaries from archive
	mkdir -p $(CDS3_CMPWSSERVER_PACKAGE_DIR)
	cd $(CDS3_CMPWSSERVER_PACKAGE_DIR) && \
	ar -xov $(CDS3_CMPWSSERVER_PLATFORMCONFIGPACKAGEDIR)/$(CDS3_CMPWSSERVER_PACKAGE_NAME).ipk
	@$(call install_archive, cds3-cmpwsserver, 0, 0, $(CDS3_CMPWSSERVER_PACKAGE_DIR)/data.tar.gz, )
else
	# WAGO_TOOLS_BUILD_VERSION_TRUNK | WAGO_TOOLS_BUILD_VERSION_RELEASE

	@$(call install_copy, cds3-cmpwsserver, 0, 0, 0750, $(CDS3_CMPWSSERVER_DIR)/$(CDS3_CMPWSSERVER_BIN), /usr/lib/$(CDS3_CMPWSSERVER_BIN))
	
	@$(call install_link, cds3-cmpwsserver, ./$(CDS3_CMPWSSERVER_BIN), /usr/lib/lib$(CDS3_CMPWSSERVER).so);
	
	@$(call install_link, cds3-cmpwsserver, ../$(CDS3_CMPWSSERVER_BIN), /usr/lib/cds3-custom-components/lib$(CDS3_CMPWSSERVER).so);
	
	@$(call install_alternative, cds3-cmpwsserver , 0, 0, 0644, $(PTXCONF_CDS3_PLCCONFIGDIR)/$(CDS3_CMPWSSERVER).cfg)
	
	@$(call install_copy, cds3-cmpwsserver, 0, 0, 0755, $(CDS3_CMPWSSERVER_WS_DIR))
	@$(call install_copy, cds3-cmpwsserver, 0, 0, 0644, $(CDS3_CMPWSSERVER_DIR)/eWS.js,  $(CDS3_CMPWSSERVER_WS_DIR)/eWS.js)

endif
	
	@$(call install_finish,cds3-cmpwsserver)
	
ifdef PTXCONF_WAGO_TOOLS_BUILD_VERSION_RELEASE
	# Backup binaries in configs/@platform@/packages/
	cp $(PKGDIR)/$(CDS3_CMPWSSERVER_PACKAGE_NAME).ipk $(CDS3_CMPWSSERVER_PLATFORMCONFIGPACKAGEDIR)/
endif

	@$(call touch)


# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------


$(STATEDIR)/cds3-cmpwsserver.clean:
	rm -rf $(STATEDIR)/cds3-cmpwsserver.*
	rm -rf $(PKGDIR)/cds3-cmpwsserver_*
	rm -rf $(CDS3_CMPWSSERVER_DIR)
	rm -rf $(PTXDIST_PLATFORMDIR)/root$(CDS3_CMPWSSERVER_WS_DIR)
    
