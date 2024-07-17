#!/bin/bash

# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2023 WAGO GmbH & Co. KG

current_processes=$(ps -T -A  -o spid,comm)
known_processes="/etc/specific/known_processes.lst"
echo "Known processes:"
while read -r line; do
	{ [[ "$line" =~ ^#.*$ ]] || [[ -z "$line" ]]; } && continue
	IFS=';'
	# Split known process entry
	read -ra ARRAY <<< "$line"
	regex=${ARRAY[0]}
	sched=${ARRAY[1]}
	prio=${ARRAY[2]}
	# Match known process regex in process list
	match=$(grep -E "$regex" <<< "$current_processes")
	if [[ -n "$match" ]]; then
		IFS=$'\n'
		readarray -t ARRAY <<< "$match"
		# The regex may result in multiple matches
		# Refine the regex if that's unwanted
		for value in "${ARRAY[@]}"; do
			IFS=' '
			# Now split to get the pid
			read -ra ARRAY2 <<< "$value"
			pid=${ARRAY2[0]}
			# Finally set the prio
			chrt -p -"$sched" "$prio" "$pid"
			echo "process: ${ARRAY2[1]}, spid: $pid, prio: $prio, scheduler: $sched"
		done
	fi
done < "$known_processes"
