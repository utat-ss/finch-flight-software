# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

FROM ubuntu:24.04

SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /workspace
COPY scripts /workspace/scripts
COPY finch-flight-software-env.sh /workspace/finch-flight-software-env.sh
COPY west.yml /workspace/west.yml

RUN apt update
RUN apt --no-install-recommends -y install sudo

RUN /workspace/scripts/install_dependencies.sh
RUN /workspace/scripts/setup_python_venv.sh
RUN /workspace/scripts/setup_west_workspace.sh

RUN sudo apt install --no-install-recommends -y \
	openocd minicom

RUN source /.venv/bin/activate && \
	pip install pyocd && \
	pyocd pack update && \
	pyocd pack install stm32h743zi stm32g431rbtx

RUN cat <<EOF > /entrypoint.sh
	source /.venv/bin/activate && \
	source /zephyr/zephyr-env.sh && \
	source /workspace/finch-flight-software-env.sh && \
	/bin/bash
EOF

RUN chmod +x /entrypoint.sh

ENTRYPOINT "/entrypoint.sh"