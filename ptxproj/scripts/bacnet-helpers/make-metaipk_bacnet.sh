#!/usr/bin/env bash

# Build repo for installing BACnet on pfc firmware
#
# WAGO GmbH & Co. KG

# Arguments:
while [[ $# -gt 0 ]]; do
  case $1 in
    -v|--version)
      BACNET_VERSION="$2"
      ;;
    -r|--revision)
      BACNET_REV="$2"
      ;;
    -u|--url)
      PTXCONF_OPKG_OPKG_CONF_URL="$2"
      ;;
    --libwebsockets)
      WITH_LIBWEBSOCKETS="$2"
      ;;
    --libbacnetstack)
      WITH_BACNETSTACK="$2"
      ;;
    --libbacnet)
      WITH_LIBBACNET="$2"
      ;;
    --libbacnetconfig)
      WITH_BACNETCONFIG="$2"
      ;;
    --cds3tsciobacnet)
      WITH_TSCIOBACNET="$2"
      ;;
    --cds3iodrvbacnet)
      WITH_IODRVBACNET="$2"
      ;;
    --wbmbacnet)
      WITH_WBMBACNET="$2"
      ;;
    --bacnetnative)
      WITH_BACNETNATIVE="$2"
      ;;
    --ppbacnet)
      WITH_PPBACNET="$2"
      ;;
    -*|--*)
      echo "!!! Unknown option $1"
      exit 1
      ;;
  esac
  shift # past argument
  shift # past value
done

if [ -z "$WITH_LIBWEBSOCKETS" ] || \
   [ -z "$WITH_BACNETSTACK" ] || \
   [ -z "$WITH_LIBBACNET" ] || \
   [ -z "$WITH_BACNETCONFIG" ] || \
   [ -z "$WITH_TSCIOBACNET" ] || \
   [ -z "$WITH_IODRVBACNET" ] || \
   [ -z "$WITH_WBMBACNET" ] || \
   [ -z "$WITH_BACNETNATIVE" ] || \
   [ -z "$WITH_PPBACNET" ]; then
  echo "!!! Missing value for one or more packages"
  exit 1
fi

# Prepare URL for sed ()
PTXCONF_OPKG_OPKG_CONF_URL_SED=${PTXCONF_OPKG_OPKG_CONF_URL//$'/'/$'\/'}

# PTXdist defines:
PTXPROJ=$(pwd)
PTX_WORKSPACE="$PTXDIST_WORKSPACE"
PTX_PLATFORMDIR="$PTXDIST_PLATFORMDIR"

# Local defines:
BUILD_DIR=$PTXDIST_PLATFORMDIR/packages/
FW_VERSION=$(cat  $PTXDIST_WORKSPACE/projectroot/etc/REVISIONS | grep "FIRMWARE=" | cut -d= -f2 | sed -e 's/(/_/g;s/)//g')

if [ "$WITH_LIBWEBSOCKETS" = "y" ]; then
  LIB_WEBSOCKETS_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep libwebsockets_ | cut -f2)
  if [ "$LIB_WEBSOCKETS_IPK" = "" ]; then
    echo !!! LIB_WEBSOCKETS_IPK not found
    exit 1
  fi
fi

if [ "$WITH_BACNETSTACK" = "y" ]; then
  LIB_BACNET_STACK_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep libbacnetstack_ | cut -f2)
  if [ "$LIB_BACNET_STACK_IPK" = "" ]; then
    echo !!! LIB_BACNET_STACK_IPK not found
    exit 1
  fi
fi

if [ "$WITH_LIBBACNET" = "y" ]; then
  LIB_BACNET_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep libbacnet_ | cut -f2)
  if [ "$LIB_BACNET_IPK" = "" ]; then
    echo !!! LIB_BACNET_IPK not found
    exit 1
  fi
fi

if [ "$WITH_BACNETCONFIG" = "y" ]; then
  LIB_BACNET_CONFIG_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep libbacnetconfig_ | cut -f2 )
  if [ "$LIB_BACNET_CONFIG_IPK" = "" ]; then
    echo !!! LIB_BACNET_CONFIG_IPK not found
    exit 1
  fi
fi

if [ "$WITH_TSCIOBACNET" = "y" ]; then
  CDS3_TSCIO_BACNET_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep cds3-tsciobacnet_ | cut -f2 )
  if [ "$CDS3_TSCIO_BACNET_IPK" = "" ]; then
    echo !!! CDS3_TSCIO_BACNET_IPK not found
    exit 1
  fi
fi

if [ "$WITH_IODRVBACNET" = "y" ]; then
  CDS3_IODRV_BACNET_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep cds3-iodrvbacnet_ | cut -f2 )
  if [ "$CDS3_IODRV_BACNET_IPK" = "" ]; then
    echo !!! CDS3_IODRV_BACNET_IPK not found
    exit 1
  fi
fi

if [ "$WITH_WBMBACNET" = "y" ]; then
  WAGO_NG_PLUGIN_BACNET_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep wbm-ng-plugin-bacnet_ | cut -f2 )
  if [ "$WAGO_NG_PLUGIN_BACNET_IPK" = "" ]; then
    echo !!! WAGO_NG_PLUGIN_BACNET_IPK not found
    exit 1
  fi
fi

if [ "$WITH_BACNETNATIVE" = "y" ]; then
  BACNET_NATIVE_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep bacnetnative_ | cut -f2 )
  if [ "$BACNET_NATIVE_IPK" = "" ]; then
    echo !!! BACNET_NATIVE_IPK not found
    exit 1
  fi
fi

if [ "$WITH_PPBACNET" = "y" ]; then
  PP_BACNET_IPK=$(ls $PTXDIST_PLATFORMDIR/packages | grep pp-bacnet_ | cut -f2 )
  if [ "$PP_BACNET_IPK" = "" ]; then
    echo !!! PP_BACNET_IPK not found
    exit 1
  fi
fi

PACKAGES="$LIB_BACNET_IPK $LIB_BACNET_CONFIG_IPK $CDS3_TSCIO_BACNET_IPK $CDS3_IODRV_BACNET_IPK $WAGO_NG_PLUGIN_BACNET_IPK $LIB_BACNET_STACK_IPK $BACNET_NATIVE_IPK $PP_BACNET_IPK $LIB_WEBSOCKETS_IPK"
VERSION="${BACNET_VERSION}"
REPO_NAME="bacnet_rev${BACNET_REV}_${VERSION}_FW${FW_VERSION}.repo"

#--------------------------------------
# Print variables
#--------------------------------------
echo PTXPROJ=$PTXPROJ
echo PTX_WORKSPACE=$PTX_WORKSPACE
echo PTX_PLATFORMDIR=$PTX_PLATFORMDIR

echo BACNET_VERSION=$BACNET_VERSION
echo BACNET_REV=$BACNET_REV

echo PTXCONF_OPKG_OPKG_CONF_URL=$PTXCONF_OPKG_OPKG_CONF_URL
echo PTXCONF_OPKG_OPKG_CONF_URL_SED=$PTXCONF_OPKG_OPKG_CONF_URL_SED

echo BUILD_DIR=$BUILD_DIR
echo FW_VERSION=$FW_VERSION
echo PACKAGES=$PACKAGES
echo VERSION=$VERSION
echo REPO_NAME=$REPO_NAME

#--------------------------------------
# Cleanup function
#--------------------------------------
function cleanup
{
  rm -rf ${BUILD_DIR}bacnet-repo-src ${BUILD_DIR}bacnet-repo
}

#--------------------------------------
# Create folder
#--------------------------------------
mkdir -m 700 -p ${BUILD_DIR}bacnet-repo-src/data/root
mkdir -p        ${BUILD_DIR}bacnet-repo-src/data/root/packages
mkdir -p        ${BUILD_DIR}bacnet-repo-src/control

#---------------------------------------------------------
#get ipk files stored in this meta packet
#---------------------------------------------------------
for PACKAGE in $PACKAGES; do
  if [ ! -f "$PTXDIST_PLATFORMDIR/packages/$PACKAGE" ]; then
    echo "ERROR Could not find package $PTXDIST_PLATFORMDIR/packages/$PACKAGE"
    cleanup
    exit 1
  fi
  #echo "INFO  get package $PACKAGE"
  cp "$PTXDIST_PLATFORMDIR/packages/$PACKAGE" ${BUILD_DIR}bacnet-repo-src/data/root/packages
done

#---------------------------------------------------------
#create repository file
#---------------------------------------------------------
echo '#!/usr/bin/env bash'                         > ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo 'set -e'                                     >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ''                                           >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo 'pkg_dir=$1'                                 >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' '                                          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' if [ -z $pkg_dir ] || [ ! -d $pkg_dir ]; then'             >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   echo "Usage: ipkg-make-index <package_directory>" >&2'   >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   exit 1'                                  >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' fi'                                        >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' '                                          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' which md5sum >/dev/null 2>&1 || alias md5sum=md5'          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' empty=1'                                   >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' '                                          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' for pkg in `find $pkg_dir -name '\''*.ipk'\'' | sort`; do' >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   empty='                                  >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   name="${pkg##*/}"'                       >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   name="${name%%_*}"'                      >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' # [[ "$name" = "kernel" ]] && continue'    >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' # [[ "$name" = "libc" ]] && continue'      >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   echo "Generating index for package $pkg" >&2'            >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   file_size=$(stat -c "%s" $pkg)'                          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   md5sum=$(md5sum $pkg | awk '\''{print $1}'\'')'          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   sha256sumvar=$(sha256sum $pkg | awk '\''{print $1}'\'')' >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   # Take pains to make variable value sed-safe'            >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   sed_safe_pkg=`echo $pkg | sed -e '\''s/^\.\///g'\'' -e '\''s/\\//\\\\\\//g'\''`' >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   ar x $pkg ./control.tar.gz '             >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   tar xzf ./control.tar.gz ./control'      >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   cat ./control | sed -e "s/^Description:/Filename: $sed_safe_pkg\\' >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo 'Size: $file_size\\'                         >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo 'MD5Sum: $md5sum\\'                          >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo 'SHA256sum: $sha256sumvar\\'                 >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo 'Description:/"'                             >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   rm -f ./control.tar.gz'                  >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   rm -f ./control'                         >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo '   echo ""'                                 >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' done'                                      >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' [ -n "$empty" ] && echo'                   >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh
echo ' exit 0'                                    >> ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh

chmod +x ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh

cd ${BUILD_DIR}bacnet-repo-src/data/root/packages
./ipkg-make-index.sh ./ > Packages
gzip Packages

cd $PTXPROJ
rm ${BUILD_DIR}bacnet-repo-src/data/root/packages/ipkg-make-index.sh

#---------------------------------------------------------
#create control file
#---------------------------------------------------------
echo "Package: bacnet_repo"                                      > ${BUILD_DIR}bacnet-repo-src/control/control
echo "Priority: optional"                                       >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Version:  $VERSION"                                       >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Section:    base"                                         >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Architecture: armhf"                                      >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Maintainer: \"WAGO GmbH & Co. KG (SEp)\" " >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Depends: "                                                >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Source: "                                                 >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "Description:  Installation for WAGO BACnet. This packet is a meta packet including all parts needed for BACnet." >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "              This packet is built for Firmware Version $FW_VERSION" >> ${BUILD_DIR}bacnet-repo-src/control/control
echo "              The following packets are included."        >> ${BUILD_DIR}bacnet-repo-src/control/control
for PACKAGE in $PACKAGES; do
echo "              $PACKAGE"                                   >> ${BUILD_DIR}bacnet-repo-src/control/control
done
echo "              The packet installs a local repo only. Use opkg update and opkg install to install BACnet." >> ${BUILD_DIR}bacnet-repo-src/control/control

#---------------------------------------------------------
#create postinst file
#---------------------------------------------------------
echo "#!/bin/bash"                                               > ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "#"                                                        >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "# WAGO GmbH & Co. KG"                      >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo ""                                                         >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "sed -i -e \"s/$PTXCONF_OPKG_OPKG_CONF_URL_SED/src\/gz local file:\/\/\/root\/packages/g\" /etc/opkg/opkg.conf" >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "source /etc/profile &>/dev/null"                          >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "function start_update"                                    >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "{"                                                        >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    local PID_OF_IPKG=\$1"                                >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    while ps -p \$PID_OF_IPKG > /dev/null"                >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    do"                                                   >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "        usleep 100000"                                    >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    done"                                                 >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    export PREPARE_MODE=@PREPARE_MODE@"                   >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    /root/install_bacnet.sh 2>&1 |  tee -a /var/log/bacnet_install.log" >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    sync"                                                 >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "}"                                                        >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo ""                                                         >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "function update_firewall"                                 >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "{"                                                        >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    local CONTENT=\"        <rule state=\\\"on\\\" proto=\\\"tcp\\\" dst_port=\\\"47808\\\"\/>\"" >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    local FILE=\"/etc/firewall/services/bacnet.xml\""     >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    if ! grep -q \"tcp\" \$FILE; then"                    >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "        sed -i \"/<\/rules>/ s/.*/\$CONTENT\n&/\" \$FILE" >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    fi"                                                   >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "    sync"                                                 >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "}"                                                        >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo ""                                                         >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "PID_OF_IPKG=\$(pidof opkg)"                               >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "start_update \$PID_OF_IPKG &"                             >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "update_firewall"                                          >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo ""                                                         >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo "exit 0"                                                   >> ${BUILD_DIR}bacnet-repo-src/control/postinst
echo ""                                                         >> ${BUILD_DIR}bacnet-repo-src/control/postinst

chmod +x ${BUILD_DIR}bacnet-repo-src/control/postinst

#---------------------------------------------------------
#create install_bacnet.sh
#---------------------------------------------------------
echo "#!/bin/sh"                                         > ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
echo ""                                                 >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
echo "/etc/init.d/runtime stop"                         >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
echo "opkg update"                                      >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
# remove packages
for PACKAGE in $PACKAGES; do
echo "opkg remove --force-depends $(echo $PACKAGE | cut -f1 -d"_")" >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
done
echo "opkg clean"                                       >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
echo "opkg update"                                      >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
# install packages
for PACKAGE in $PACKAGES; do
echo "opkg install --force-depends $(echo $PACKAGE | cut -f1 -d"_")" >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
done
echo "opkg remove bacnet_repo"                          >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
echo "/etc/init.d/runtime start"                        >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
if [ "$WITH_PPBACNET" = "y" ]; then
echo "sed -i -e 's/\"BACnet\"/\"BACnet_V2\"/g' /usr/share/wdd/*" >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
echo "/etc/init.d/paramd restart"                       >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh
fi
echo ""                                                 >> ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh

chmod +x ${BUILD_DIR}bacnet-repo-src/data/root/install_bacnet.sh

#---------------------------------------------------------
#create files to store in ipk
#---------------------------------------------------------
mkdir -p ${BUILD_DIR}bacnet-repo
tar --owner=root --group=root -C ${BUILD_DIR}bacnet-repo-src/control -czf ${BUILD_DIR}bacnet-repo/control.tar.gz .
tar --owner=root --group=root -C ${BUILD_DIR}bacnet-repo-src/data -czf ${BUILD_DIR}bacnet-repo/data.tar.gz .
echo "2.0" > ${BUILD_DIR}bacnet-repo/debian-binary

#---------------------------------------------------------
#create ipk file
#---------------------------------------------------------
rm -f $REPO_NAME
cd ${BUILD_DIR}bacnet-repo
echo "OPKG  create package $REPO_NAME"
ar r ../$REPO_NAME debian-binary data.tar.gz control.tar.gz 2> /dev/null
cd $PTXPROJ

# cleanup BUILD_DIR
cleanup

exit 0
