# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

FROM ubuntu:24.04

SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /workspace
COPY scripts /workspace/scripts
COPY finch-flight-software-env.sh /workspace/finch-flight-software-env.sh
COPY west.yml /workspace/west.yml

RUN /workspace/scripts/install_dependencies.sh
RUN /workspace/scripts/setup_python_venv.sh
RUN /workspace/scripts/setup_west_workspace.sh

# If the version in west.yml is different from the installed Zephyr version,
# the west build command breaks. That's why we copy west.yml to /, and it is
# not changed if you switch the repo to a different branch, so the build doesn't break.
# These commands change the used west.yml to /west.yml.
RUN cp /workspace/west.yml /west.yml
RUN source /.venv/bin/activate && west config --local manifest.file "../west.yml"

# Change the build directory to speed up builds.
# The default build folder was mounted and the access was too slow on Windows/Mac.
RUN source /.venv/bin/activate && west config build.dir-fmt "/build"

RUN cat <<EOF > /env-setup.sh
	source /.venv/bin/activate && \
	source /zephyr/zephyr-env.sh && \
	source /workspace/finch-flight-software-env.sh
EOF

RUN chmod +x /env-setup.sh

RUN cat <<EOF > /entrypoint.sh
	source /env-setup.sh && \
	exec /bin/bash
EOF

RUN chmod +x /entrypoint.sh

ENTRYPOINT "/entrypoint.sh"
