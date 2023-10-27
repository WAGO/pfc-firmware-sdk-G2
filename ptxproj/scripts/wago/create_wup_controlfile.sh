#!/bin/dash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

FILE="$1"

FIRMWARE_MAJOR_LONG="$2"
FIRMWARE_MINOR_LONG="$3"
FIRMWARE_BUGFIX_LONG="$4"
FIRMWARE_REVISION_LONG="${FIRMWARE_MAJOR_LONG}${FIRMWARE_MINOR_LONG}${FIRMWARE_BUGFIX_LONG}"
FIRMWARE_INDEX="$5"
FIRMWARE_MAJOR="${FIRMWARE_MAJOR_LONG#0*}"
FIRMWARE_MINOR="${FIRMWARE_MINOR_LONG#0*}"
FIRMWARE_BUGFIX="${FIRMWARE_BUGFIX_LONG#0*}"
FIRMWARE_BASE="${FIRMWARE_MAJOR}.${FIRMWARE_MINOR}"
FIRMWARE_REVISION="${FIRMWARE_BASE}.${FIRMWARE_BUGFIX}"
RAUC_UPDATEFILE="$(basename "$6")"
PLATFORM="$7"

#-------------------------------------------------------------------------------
# PFC G2
#-------------------------------------------------------------------------------

PFC_G2_REFID="PFC-Common"
PFC_G2_MIN_VERSION="3.0.0"
PFC_G2_ARTICLES=$(cat << EOF
  0750-8110
  0750-8111
  0750-8112
  0750-8112/0025-0000
  0750-8210
  0750-8210/0025-0000
  0750-8210/0040-0000
  0750-8210/0040-0001
  0750-8211
  0750-8211/0040-0000
  0750-8211/0040-0001
  0750-8212
  0750-8212/0000-0100
  0750-8212/0025-0000
  0750-8212/0025-0001
  0750-8212/0025-0002
  0750-8212/0040-0000
  0750-8212/0040-0001
  0750-8212/0040-0010
  0750-8213
  0750-8213/0040-0010
  0750-8214
  0750-8215
  0750-8215/K000-0001
  0750-8216
  0750-8216/0025-0000
  0750-8216/0025-0001
  0750-8216/0040-0000
  0750-8216/K000-0001
  0750-8217
  0750-8217/0025-0000
  0750-8217/K000-0002
  0750-8217/K000-0006
  0750-8217/0600-0000
  0750-8217/0625-0000
EOF
)

#-------------------------------------------------------------------------------
# PFC300
#-------------------------------------------------------------------------------

PFC300_REFID="PFC-Common"
PFC300_MIN_VERSION="3.0.0"
PFC300_ARTICLES=$(cat << EOF
  0750-8302
EOF
)

#-------------------------------------------------------------------------------
# TP
#-------------------------------------------------------------------------------

TP_REFID="TP-Common"
TP_MIN_VERSION="3.0.0"
TP_ARTICLES=$(cat << EOF
  0762-4101
  0762-4102
  0762-4103
  0762-4104
  0762-4201/8000-0001
  0762-4202/8000-0001
  0762-4203/8000-0001
  0762-4204/8000-0001
  0762-4205/8000-0001
  0762-4206/8000-0001
  0762-4301/8000-0002
  0762-4302/8000-0002
  0762-4303/8000-0002
  0762-4304/8000-0002
  0762-4305/8000-0002
  0762-4306/8000-0002
  0762-5203/8000-0001
  0762-5204/8000-0001
  0762-5205/8000-0001
  0762-5206/8000-0001
  0762-5303/8000-0002
  0762-5304/8000-0002
  0762-5305/8000-0002
  0762-5306/8000-0002
  0762-6201/8000-0001
  0762-6202/8000-0001
  0762-6203/8000-0001
  0762-6204/8000-0001
  0762-6301/8000-0002
  0762-6302/8000-0002
  0762-6303/8000-0002
  0762-6304/8000-0002
  0752-8303/8000-0002
EOF
)

#-------------------------------------------------------------------------------
# CC100
#-------------------------------------------------------------------------------

CC100_REFID="CTL-Common"
CC100_MIN_VERSION="3.0.0"
CC100_ARTICLES=$(cat << EOF
  0751-9301
  0751-9401
EOF
)

#===============================================================================

case "$PLATFORM" in
  PFC-G2-Linux|PFC-G2-Linux-hardened)
    REFID=$PFC_G2_REFID
    MIN_VERSION=$PFC_G2_MIN_VERSION
    ARTICLES=$PFC_G2_ARTICLES
    ;;
  PFC-300-Linux)
    REFID=$PFC300_REFID
    MIN_VERSION=$PFC300_MIN_VERSION
    ARTICLES=$PFC300_ARTICLES
    ;;
  TP-Linux)
    REFID=$TP_REFID
    MIN_VERSION=$TP_MIN_VERSION
    ARTICLES=$TP_ARTICLES
    ;;
  CC100-Linux)
    REFID=$CC100_REFID
    MIN_VERSION=$CC100_MIN_VERSION
    ARTICLES=$CC100_ARTICLES
    ;;
  *)
    echo "error: platfom not supported" >&2
    exit 1
    ;;
esac

#===============================================================================

print_articles() {
  for article in $ARTICLES
  do
        echo "    <Article OrderNo=\"${article}\" GroupRef=\"${REFID}\"/>"
  done
}

cat << EOF > "$FILE"
<?xml version="1.0" encoding="utf-8"?>
<!-- Caution! Elements and attributes in this file are case sensitive! -->
<FirmwareUpdateFile StructureVersion="1.0" Revision="1" System="PFC-Linux">
  <FirmwareDescription Revision="${FIRMWARE_REVISION}" ReleaseIndex="${FIRMWARE_INDEX}">
    <AssociatedFiles>
      <File RefID="RAUC-File" Type="rauc" Name="$RAUC_UPDATEFILE" TargetPath="/tmp/fwupdate/update_${FIRMWARE_INDEX}_${FIRMWARE_REVISION_LONG}.raucb"/>
    </AssociatedFiles>
  </FirmwareDescription>
  <ArticleList>
$(print_articles)
  </ArticleList>
  <GroupList>
    <Group RefID="${REFID}">
      <Upgrade>
        <VersionList>
          <VersionRange SoftwareRevision="${MIN_VERSION}-${FIRMWARE_BASE}.99"/>
        </VersionList>
      </Upgrade>
      <Downgrade>
        <VersionList>
          <VersionRange SoftwareRevision="${MIN_VERSION}-99.99.99"/>
        </VersionList>
      </Downgrade>
    </Group>
  </GroupList>
</FirmwareUpdateFile>
EOF

exit 0
