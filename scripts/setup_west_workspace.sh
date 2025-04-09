#!/bin/bash

set -e

source "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../finch-flight-software-env.sh"
source "$(dirname "${FINCH_FLIGHT_SOFTWARE_ROOT}")/.venv/bin/activate"
cd "${FINCH_FLIGHT_SOFTWARE_ROOT}"
west init --local --mf west.yml && west update
west zephyr-export
pip install -r "$(dirname "${FINCH_FLIGHT_SOFTWARE_ROOT}")/zephyr/scripts/requirements.txt"
west sdk install --install-base $(dirname "${FINCH_FLIGHT_SOFTWARE_ROOT}")
echo "FINCH Flight Software Development Environment setup completed."
