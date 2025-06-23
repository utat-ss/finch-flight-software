FROM ubuntu:24.04

# Set default shell during Docker image build to bash
SHELL ["/bin/bash", "-c"]

# Set non-interactive frontend for apt-get to skip any user confirmations
ENV DEBIAN_FRONTEND=noninteractive

RUN mkdir -p /workspace/
COPY scripts /workspace/scripts
COPY finch-flight-software-env.sh /workspace/finch-flight-software-env.sh
COPY west.yml /workspace/west.yml

RUN apt update
RUN apt --no-install-recommends -y install sudo

RUN /workspace/scripts/install_dependencies.sh
RUN /workspace/scripts/setup_python_venv.sh
RUN /workspace/scripts/setup_west_workspace.sh

# install debuggers
RUN sudo apt install --no-install-recommends -y \
    openocd minicom

RUN source /.venv/bin/activate && \
    pip install pyocd

RUN echo "source /.venv/bin/activate \
         && source /zephyr/zephyr-env.sh \
         && source /workspace/finch-flight-software-env.sh \
         && cd /workspace" > /root/.bashrc

ENTRYPOINT /bin/bash
