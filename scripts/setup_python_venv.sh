#!/bin/bash

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

set -e

source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../finch-flight-software-env.sh"
python3 -m venv "$(dirname "${FINCH_FLIGHT_SOFTWARE_ROOT}")/.venv"
source "$(dirname "${FINCH_FLIGHT_SOFTWARE_ROOT}")/.venv/bin/activate"
pip install west
