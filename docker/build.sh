#!/bin/bash

set -e

TOOLCHAIN_PATH="/opt/gcc-Toolchain-2022.02/LINARO.Toolchain-2022.02/arm-linux-gnueabihf/bin/"

print_usage() {
    cat << EOF
build, (c) 2023 WAGO GmbH & Co. KG
Usage:
    build init | build | images | help
Commands:
    init    initialize project
    build   build all packages
    images  build images
    wup     create WUP file
EOF
}

case "$1" in
    -h | --help | help)
        print_usage
        ;;
    init)
        echo "Setup PTXdist project"
        ptxdist select configs/wago-pfcXXX/ptxconfig_pfc_g2
        ptxdist platform configs/wago-pfcXXX/platformconfig        
        ptxdist toolchain $TOOLCHAIN_PATH
        ptxdist clean -q
        ;;
    build | "")
        ptxdist go -q
        ;;
    get)
        ptxdist select configs/wago-pfcXXX/ptxconfig_pfc_g2
        ptxdist platform configs/wago-pfcXXX/platformconfig        
        ptxdist toolchain $TOOLCHAIN_PATH
        ptxdist get -q
        ;;
    images)
        ptxdist images -q
        ;;
    wup)
        make wup
        ;;
    *)
        echo "error: unknown command" 1>&2
        exit 1
        ;;
esac
