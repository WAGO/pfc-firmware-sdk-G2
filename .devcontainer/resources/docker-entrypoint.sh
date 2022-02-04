#!/bin/bash
git lfs pull
exec /usr/local/share/docker-init.sh "$@"