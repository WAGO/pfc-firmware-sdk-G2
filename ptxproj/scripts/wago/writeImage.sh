#!/bin/bash

# Set error on use of undefined shell variables
set -o nounset

WRITE_BLOCK_SIZE="4M"

function print_usage
{
    echo
    echo "* Command line interface tool to write binary images to block devices *"
    echo
    echo "Usage:"
    echo "  $0 <options>"
    echo
    echo "Options:"
    echo "  -h, --help                        Show this help text"
    echo "  -q, --quiet                       Do not print info or error messages to stdout/stderr"
    echo "  -u, --unverified                  Do not verify written image"
    echo "  -d, --device <param>              Device to write to"
    echo "  -f, --input-file <param>          Image file to write"
}


function print
{
    if [[ $QUIET -eq 0 ]]; then
        echo "$1"
    fi
}


function prepare_device
{
    ls "$DEVICE"?* >/dev/null 2>&1
    if [[ $? -ne 0  ]]; then
        print "No device to unmount"
    else
        for PART in $(ls "$DEVICE"?*); do
            local MOUNTSTRING=$(mount | grep "$PART")
            if [[ -n "$MOUNTSTRING" ]]; then
                print "Unmount device \"$PART\"..."
                umount "$PART";
                STATUS=$?
                if [[ $STATUS -ne 0 ]]; then
                    print "ERROR: Failed to unmount device \"$PART\" (status $STATUS)"
                    return $STATUS
                fi
                print "done"
            fi
        done
    fi
}


function wipe_target
{
    local DEV_BLOCK_SIZE=$(blockdev --getss "$DEVICE")
    STATUS=$?
    if [[ $STATUS -ne 0 ]]; then
        DEV_BLOCK_SIZE=512
        print "WARNING: Failed to get logical block size from device, using assumed default of $DEV_BLOCK_SIZE bytes"
    fi
    print "Wiping first block of $DEV_BLOCK_SIZE bytes (area for MBR)..."
    flock "$DEVICE" dd bs=$DEV_BLOCK_SIZE if="/dev/zero" of="$DEVICE" count=1
    STATUS=$?
    if [[ $STATUS -ne 0 ]]; then
        print "ERROR: Failed to wipe first block on device \"$DEVICE\" (status $STATUS)"
        return $STATUS
    fi
    print "done"
    local BLOCK_COUNT=34
    local BYTES=$(($BLOCK_COUNT*$DEV_BLOCK_SIZE))
    print "Wiping last $BLOCK_COUNT blocks of $DEV_BLOCK_SIZE bytes (-> $BYTES bytes, area for GPT)..."
    flock "$DEVICE" dd bs=$DEV_BLOCK_SIZE if="/dev/zero" of="$DEVICE" count=$BLOCK_COUNT seek=$(($(blockdev --getsz "$DEVICE") - $BLOCK_COUNT))
    STATUS=$?
    if [[ $STATUS -ne 0 ]]; then
        print "ERROR: Failed to wipe last $BLOCK_COUNT blocks on device \"$DEVICE\" (status $STATUS)"
        return $STATUS      
    fi
    print "done"
}


function write_image
{
    print "Write image \"$IMAGE\" to \"$DEVICE\" (block size $WRITE_BLOCK_SIZE)..."
    flock "$DEVICE" dd if="$IMAGE" of="$DEVICE" bs=$WRITE_BLOCK_SIZE oflag=direct
    STATUS=$?
    if [[ $STATUS -ne 0 ]]; then 
        print "ERROR: Failed to write image (status $STATUS)"
        return $STATUS
    fi
    print "done"
    print "Synchronize devices..."
    sync
    STATUS=$?
    if [[ $STATUS -ne 0 ]]; then
        print "ERROR: Failed to synchronize devices (status $STATUS)"
        return $STATUS
    fi
    print "done"

    return $STATUS
}


function verify_image
{
    local IMAGE_SIZE=$(stat --printf="%s" "$IMAGE")
    print "Verify image \"$IMAGE\" on \"$DEVICE\" ($IMAGE_SIZE bytes)..."
    head -c $IMAGE_SIZE "$DEVICE" | diff - "$IMAGE"
    STATUS=$?
    if [[ $STATUS -ne 0 ]]; then 
        print "ERROR: Image verification failed (status $STATUS)"
        return $STATUS
    fi
    print "done: OK"

    return $STATUS
}


function parse_cmd_line
{
    while [[ $# -ne 0 && $STATUS == "0" ]]; do
        case "$1" in
            -h)
                print_usage
                exit 0
                ;;
            --help)
                print_usage
                exit 0
                ;;
            -q)
                QUIET=1
                ;;
            --quiet)
                QUIET=1
                ;;
            -u)
                VERIFY=0
                ;;
            --unverified)
                VERIFY=0
                ;;
            -v)
                WRITE=0
                VERIFY=1
                ;;
            --verify-only)
                WRITE=0
                VERIFY=1
                ;;
            -w)
                WIPE=1
                ;;
            --wipe)
                WIPE=1
                ;;
            -d)
                shift 1
                DEVICE="$1"
                ;;
            --device)
                shift 1
                DEVICE="$1"
                ;;
            -f)
                shift 1
                IMAGE="$1"
                ;;
            --input-file)
                shift 1
                IMAGE="$1"
                ;;
        esac
        shift 1
    done
    
return $STATUS
}



STATUS=0
QUIET=0
WRITE=1
WIPE=0
VERIFY=1
DEVICE=""
IMAGE=""

if [[ $# -lt 1 ]]; then
    print_usage
    exit 0
fi

parse_cmd_line "$@"

if [[ $STATUS -ne 0  ]]; then
    print "ERROR: Argument parsing failed"
    exit 1
fi

if [[ -z "$IMAGE" || ! -e "$IMAGE" ]]; then
    print "ERROR: No valid image file given: \"$IMAGE\""
    exit 1
fi

if [[ -z "$DEVICE" || ! -e "$DEVICE" ]]; then
    print "ERROR: No valid device given: \"$DEVICE\""
    exit 1
fi

if [[ ! -b "$DEVICE" ]]; then
    print "ERROR: \"$DEVICE\" is no valid block device"
    exit 1
fi

if [[ $STATUS -eq 0 ]]; then
    prepare_device
fi

if [[ $STATUS -eq 0 && $WIPE -ne 0 ]]; then
    wipe_target
fi

if [[ $STATUS -eq 0 && $WRITE -ne 0 ]]; then
    udevadm settle
    SETTLE_STATUS=$?
    if [[ $SETTLE_STATUS -ne 0 ]]; then
        print "WARNING: Failed to settle UDEV ($SETTLE_STATUS)"
    fi
    write_image
fi

if [[ $STATUS -eq 0 && $VERIFY -ne 0 ]]; then
    udevadm settle
    SETTLE_STATUS=$?
    if [[ $SETTLE_STATUS -ne 0 ]]; then
        print "WARNING: Failed to settle UDEV ($SETTLE_STATUS)"
    fi
    verify_image
fi

exit $STATUS

