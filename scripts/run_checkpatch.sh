#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

set -e

source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../finch-flight-software-env.sh"

files=$(find "${FINCH_FLIGHT_SOFTWARE_ROOT}" -type f \
    \( -name "*.[ch]" -o -name "*.dts" -o -name "*.dtsi" -o -name "*.overlay" \) ! -path "*/build/*")

echo "Found the following files to be checked:"
for i in $files; do
    echo "$i"
done


set +e

exit_code=0

for i in $files; do
    echo "Running checkpatch on $i"
    chmod +x "${FINCH_FLIGHT_SOFTWARE_ROOT}/scripts/checkpatch.pl"
    perl "${FINCH_FLIGHT_SOFTWARE_ROOT}/scripts/checkpatch.pl" --mailback --no-tree -f --emacs --summary-file --show-types \
         --ignore BRACES,PRINTK_WITHOUT_KERN_LEVEL,SPLIT_STRING,SPDX_LICENSE_TAG --max-line-length=100 "$i"
    ret=$?
    if [ $ret -ne 0 ]; then
         exit_code=1
    fi
    echo
done

set -e

if [ $exit_code -ne 0 ]; then
    echo "One or more checkpatch runs failed."
    exit 1
fi
