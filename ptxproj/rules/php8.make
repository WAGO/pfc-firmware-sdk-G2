# -*-makefile-*-
#
# Copyright (C) 2006-2008 by Robert Schwebel
#               2009, 2012 by Marc Kleine-Budde <mkl@pengutronix.de>
#               2015 by Bruno Thomsen <bth@kamstrup.com>
#               2019 by Adam Bagsik <adam.bagsik@wago.com>
#               2021 by Juergen Borleis <jbe@pengutronix.de>
#               2022 by Andreas Helmcke <ahelmcke@ela-soft.com>
#               2022-2023 by Patrick Enns <patrick.enns@wago.com>
#
# For further information about the PTXdist project and license conditions
# see the README file.
#

#
# We provide this package
#
PACKAGES-$(PTXCONF_PHP8) += php8

#
# Paths and names
#
PHP8_BASE_VERSION  := 8.3.8
PHP8_VERSION       := $(PHP8_BASE_VERSION)+wago1
PHP8_ARCHIVE_NAME  := php-$(PHP8_BASE_VERSION)
PHP8_MD5           := 3a731ebd24cd36c70148416703ba2f0e
PHP8               := php-$(PHP8_VERSION)
PHP8_SUFFIX        := tar.xz
PHP8_SOURCE        := $(SRCDIR)/$(PHP8_ARCHIVE_NAME).$(PHP8_SUFFIX)
PHP8_DIR           := $(BUILDDIR)/$(PHP8)
PHP8_LICENSE       := PHP-3.01
PHP8_LICENSE_FILES := file://LICENSE;md5=5ebd5be8e2a89f634486445bd164bef0

#
# Note: older releases are moved to the 'museum', but the 'de.php.net'
# response with a HTML file instead of the archive. So, try the 'museum'
# URL first
#
PHP8_URL := \
	https://www.php.net/distributions/$(PHP8_ARCHIVE_NAME).$(PHP8_SUFFIX) \
	http://museum.php.net/php8/$(PHP8_ARCHIVE_NAME).$(PHP8_SUFFIX) \
	http://de.php.net/distributions/$(PHP8_ARCHIVE_NAME).$(PHP8_SUFFIX) \
	http://de2.php.net/get/$(PHP8_ARCHIVE_NAME).$(PHP8_SUFFIX)/from/this/mirror

# ----------------------------------------------------------------------------
# Prepare
# ----------------------------------------------------------------------------

#
# autoconf
#
PHP8_AUTOCONF := \
	$(CROSS_AUTOCONF_USR) \
	--prefix=/usr/php8 \
	--disable-rpath \
	--disable-re2c-cgoto \
	--disable-gcc-global-regs \
	--$(call ptx/endis, PTXCONF_PHP8_SAPI_CLI)-cli \
	--without-pear \
	--disable-embed \
	--without-fpm-user \
	--without-fpm-group \
	--without-fpm-systemd \
	--without-fpm-acl \
	--without-fpm-apparmor \
	--without-fpm-selinux \
	--disable-fuzzer \
	--disable-litespeed \
	--disable-phpdbg \
	--disable-phpdbg-debug \
	--disable-phpdbg-readline \
	--$(call ptx/endis, PTXCONF_PHP8_SAPI_CGI)-cgi \
	--without-valgrind \
	--disable-gcov \
	--disable-debug \
	--disable-debug-assertions \
	--disable-zts \
	--disable-rtld-now \
	--without-layout \
	--with-config-file-path=/etc/php8 \
	--without-config-file-scan-dir \
	--disable-sigchild \
	--disable-libgcc \
	--disable-short-tags \
	--disable-dmalloc \
	--$(call ptx/endis, PTXCONF_GLOBAL_IPV6)-ipv6 \
	--disable-dtrace \
	--disable-fd-setsize \
	--disable-werror \
	--disable-memory-sanitizer \
	--disable-address-sanitizer \
	--disable-undefined-sanitizer \
	--disable-all \
	--without-kerberos \
	--without-system-ciphers \
	--without-external-pcre \
	--without-pcre-jit \
	--disable-bcmath \
	--without-bz2 \
	--disable-calendar \
	--disable-ctype \
	--disable-dba \
	--without-qdbm \
	--without-gdbm \
	--without-ndbm \
	--without-db4 \
	--without-db3 \
	--without-db2 \
	--without-db1 \
	--without-dbm \
	--without-tcadb \
	--without-lmdb \
	--without-cdb \
	--disable-inifile \
	--disable-flatfile \
	--disable-dl-test \
	--without-enchant \
	--disable-exif \
	--$(call ptx/endis, PTXCONF_PHP8_FILTER)-filter \
	--disable-ftp \
	--without-openssl-dir \
	--without-external-gd \
	--without-avif \
	--without-webp \
	--without-freetype \
	--disable-gd-jis-conv \
	--without-gettext \
	--without-gmp \
	--without-mhash \
	--without-iconv \
	--without-imap \
	--without-imap-ssl \
	--disable-intl \
	--without-ldap \
	--without-ldap-sasl \
	--disable-mbregex \
	--without-mysqli \
	--without-mysql-sock \
	--without-oci8 \
	--without-odbcver \
	--without-adabas \
	--without-sapdb \
	--without-solid \
	--without-ibm-db2 \
	--without-empress \
	--without-empress-bcs \
	--without-custom-odbc \
	--without-iodbc \
	--without-esoob \
	--without-unixODBC \
	--without-dbmaker \
	--disable-huge-code-pages \
	--disable-pcntl \
	--without-pdo-dblib \
	--without-pdo-firebird \
	--without-pdo-mysql \
	--without-zlib-dir \
	--without-pdo-oci \
	--without-pdo-odbc \
	--without-pdo-pgsql \
	--without-pgsql \
	--disable-phar \
	--disable-posix \
	--without-pspell \
	--without-libedit \
	--without-readline \
	--$(call ptx/endis, PTXCONF_PHP8_SESSION)-session \
	--without-mm \
	--without-snmp \
	--without-sodium \
	--without-external-libcrypt \
	--without-password-argon2 \
	--disable-sysvmsg \
	--disable-sysvshm \
	--without-tidy \
	--disable-tokenizer \
	--without-expat \
	--disable-zend-test \
	--disable-mysqlnd \
	--disable-mysqlnd-compression-support \
	--without-pear \
	--disable-fiber-asm \
	--disable-zend-signals

ifdef PTXCONF_PHP8_SAPI_FPM
PHP8_AUTOCONF += --enable-fpm
else
PHP8_AUTOCONF += --disable-fpm
endif

ifdef PTXCONF_PHP8_XML
PHP8_AUTOCONF += --enable-xml
else
PHP8_AUTOCONF += --disable-xml
endif

ifdef PTXCONF_PHP8_XML_LIBXML2
PHP8_AUTOCONF += --with-libxml
else
PHP8_AUTOCONF += --without-libxml
endif

ifdef PTXCONF_PHP8_XML_LIBXML2_READER
PHP8_AUTOCONF += --enable-xmlreader
else
PHP8_AUTOCONF += --disable-xmlreader
endif

ifdef PTXCONF_PHP8_XML_LIBXML2_WRITER
PHP8_AUTOCONF += --enable-xmlwriter
else
PHP8_AUTOCONF += --disable-xmlwriter
endif

ifdef PTXCONF_PHP8_XML_LIBXML2_DOM
PHP8_AUTOCONF += --enable-dom
else
PHP8_AUTOCONF += --disable-dom
endif

ifdef PTXCONF_PHP8_XML_LIBXML2_XSLT
PHP8_AUTOCONF += --with-xsl=/usr
else
PHP8_AUTOCONF += --without-xsl
endif

ifdef PTXCONF_PHP8_XML_LIBXML2_SIMPLEXML
PHP8_AUTOCONF += --enable-simplexml
else
PHP8_AUTOCONF += --disable-simplexml
endif

ifdef PTXCONF_PHP8_EXT_SOAP
PHP8_AUTOCONF += --enable-soap
else
PHP8_AUTOCONF += --disable-soap
endif

ifdef PTXCONF_PHP8_EXT_CURL
PHP8_AUTOCONF += --with-curl=/usr
else
PHP8_AUTOCONF += --without-curl
endif

ifdef PTXCONF_PHP8_EXT_SOCKETS
PHP8_AUTOCONF += --enable-sockets
else
PHP8_AUTOCONF += --disable-sockets
endif

ifdef PTXCONF_PHP8_EXT_PDO
PHP8_AUTOCONF += --enable-pdo
else
PHP8_AUTOCONF += --disable-pdo
endif

ifdef PTXCONF_PHP8_EXT_SQLITE3
PHP8_AUTOCONF += \
	--with-sqlite3=/usr \
	--with-pdo-sqlite=/usr
# broken config system: sqlite3 (local copy) uses it
# but it is only linked to if used by external dependencies
#PHP8_CONF_ENV += PHP_LDFLAGS="-ldl -lpthread"
else
PHP8_AUTOCONF += --without-sqlite3 --without-pdo-sqlite
endif

ifdef PTXCONF_PHP8_EXT_ZLIB
PHP8_AUTOCONF += --with-zlib=/usr
else
PHP8_AUTOCONF += --without-zlib
endif

ifdef PTXCONF_PHP8_EXT_ZIP
PHP8_AUTOCONF += --with-zip
else
PHP8_AUTOCONF += --without-zip
endif

ifdef PTXCONF_PHP8_EXT_OPENSSL
PHP8_AUTOCONF += --with-openssl=/usr
else
PHP8_AUTOCONF += --without-openssl
endif

ifdef PTXCONF_PHP8_EXT_SHMOP
PHP8_AUTOCONF += --enable-shmop --enable-sysvsem
else
PHP8_AUTOCONF += --disable-shmop --disable-sysvsem
endif

ifdef PTXCONF_PHP8_EXT_MBSTRING
PHP8_AUTOCONF += --enable-mbstring
else
PHP8_AUTOCONF += --disable-mbstring
endif

ifdef PTXCONF_PHP8_EXT_FILEINFO
PHP8_AUTOCONF += --enable-fileinfo
else
PHP8_AUTOCONF += --disable-fileinfo
endif

ifdef PTXCONF_PHP8_EXT_FFI
PHP8_AUTOCONF += --with-ffi
else
PHP8_AUTOCONF += --without-ffi
endif

ifdef PTXCONF_PHP8_EXT_GD
PHP8_AUTOCONF += --enable-gd
PHP8_AUTOCONF += --with-jpeg
PHP8_AUTOCONF += --with-xpm
endif

ifdef PTXCONF_PHP8_EXT_OPCACHE
PHP8_AUTOCONF += --enable-opcache
else
PHP8_AUTOCONF += --disable-opcache
endif

ifdef PTXCONF_PHP8_EXT_OPCACHE_JIT
PHP8_AUTOCONF += --enable-opcache-jit
else
PHP8_AUTOCONF += --disable-opcache-jit
endif

# ----------------------------------------------------------------------------
# Install
# ----------------------------------------------------------------------------

$(STATEDIR)/php8.install:
	@$(call targetinfo)
	@$(call world/install, PHP8)
	@install -vD -m644 $(PHP8_DIR)/php.ini-production $(PHP8_PKGDIR)/etc/php8/php.ini
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/include/php8 \
		&& ln -s $(PTXDIST_SYSROOT_TARGET)/usr/php8/include/php    $(PTXDIST_SYSROOT_TARGET)/usr/include/php8
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/php8-config \
		&& ln -s $(PTXDIST_SYSROOT_TARGET)/usr/php8/bin/php-config $(PTXDIST_SYSROOT_TARGET)/usr/bin/php8-config
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize8 \
		&& ln -s $(PTXDIST_SYSROOT_TARGET)/usr/php8/bin/phpize     $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize8
	@$(call touch)

# ----------------------------------------------------------------------------
# Target-Install
# ----------------------------------------------------------------------------

$(STATEDIR)/php8.targetinstall:
	@$(call targetinfo)

	@$(call install_init, php8)
	@$(call install_fixup, php8,PRIORITY,optional)
	@$(call install_fixup, php8,SECTION,base)
	@$(call install_fixup, php8,AUTHOR,"Patrick Enns <patrick.enns@wago.com>")
	@$(call install_fixup, php8,DESCRIPTION,missing)

ifdef PTXCONF_PHP8_SAPI_CLI
	@$(call install_copy, php8, 0, 0, 0755, $(PHP8_PKGDIR)/usr/php8/bin/php, /usr/bin/php8)
endif

ifdef PTXCONF_PHP8_SAPI_CGI
	@$(call install_copy, php8, 0, 0, 0755, $(PHP8_PKGDIR)/usr/php8/bin/php-cgi, /usr/bin/php8-cgi)
endif

ifdef PTXCONF_PHP8_INI
	@$(call install_alternative, php8, 0, 0, 0644, /etc/php8/php.ini)
endif

ifdef PTXCONF_PHP8_EXT_OPCACHE
	@$(call install_copy, php8, 0, 0, 0755, $(PHP8_DIR)/modules/opcache.so, /usr/lib/php8/opcache.so)
endif

ifdef PTXCONF_PHP8_SAPI_FPM
	@$(call install_copy, php8, 0, 0, 0755, $(PHP8_PKGDIR)/usr/php8/sbin/php-fpm, /usr/sbin/php8-fpm)

	@$(call install_alternative, php8, 0, 0, 0640, /etc/php8/php-fpm.conf)
	@$(call install_alternative, php8, 0, 0, 0640, /etc/php8/php-fpm.d/www.conf)
ifdef PTXCONF_INITMETHOD_BBINIT
ifdef PTXCONF_PHP8_SAPI_FPM_STARTSCRIPT
	@$(call install_alternative, php8, 0, 0, 0755, /etc/init.d/php8-fpm)

ifneq ($(call remove_quotes,$(PTXCONF_PHP8_SAPI_FPM_BBINIT_LINK)),)
	@$(call install_link, php8, \
		../init.d/php8-fpm, \
		/etc/rc.d/$(PTXCONF_PHP8_SAPI_FPM_BBINIT_LINK))
endif
endif
endif
endif

# license
	@$(call install_copy, php8, 0, 0, 0644, \
		$(PTXDIST_WORKSPACE)/projectroot/usr/share/licenses/oss/license.php8_$(PHP8_BASE_VERSION).txt, \
		/usr/share/licenses/oss/license.php8_$(PHP8_VERSION).txt)

	@$(call install_finish, php8)
	@$(call touch)

# ----------------------------------------------------------------------------
# Clean
# ----------------------------------------------------------------------------

$(STATEDIR)/php8.clean:
	@$(call targetinfo)
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/include/php8
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/php8-config
	@rm -f $(PTXDIST_SYSROOT_TARGET)/usr/bin/phpize8
	@$(call clean_pkg, PHP8)

# vim: syntax=make
