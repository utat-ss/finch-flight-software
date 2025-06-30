FROM ubuntu:24.04

ARG ZEPHYR_SDK_VERSION=0.17.0
ARG PASSWORD="zephyr"

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
    openocd openssh-server minicom

RUN source /.venv/bin/activate && \
    pip install pyocd

RUN echo "root:${PASSWORD}" | chpasswd

RUN mkdir -p /var/run/sshd && \
    chmod 0755 /var/run/sshd

RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

EXPOSE 22

# add scripts to run in .bashrc so that a ssh shell can have the same setup as docker shell
RUN echo "source /.venv/bin/activate \
         && source /zephyr/zephyr-env.sh \
         && source /workspace/finch-flight-software-env.sh \
         && cd /workspace" > /root/.bashrc

ENTRYPOINT service ssh start && \
           /bin/bash