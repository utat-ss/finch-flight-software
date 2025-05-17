# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

if [ -n "$BASH_SOURCE" ]; then
    _script_path="${BASH_SOURCE[0]}"
elif [ -n "$ZSH_VERSION" ]; then
    _script_path="${(%):-%x}"
else
    echo "This script is designed for bash or zsh when sourced."
    return 1
fi

export FINCH_FLIGHT_SOFTWARE_ROOT="$(cd "$(dirname "$_script_path")" && pwd)"
echo "FINCH_FLIGHT_SOFTWARE_ROOT set to $FINCH_FLIGHT_SOFTWARE_ROOT"
