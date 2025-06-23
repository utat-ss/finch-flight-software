FROM ubuntu:24.04

ARG PASSWORD="zephyr"
ARG ZEPHYR_SDK_VERSION=0.17.0

# Set default shell during Docker image build to bash
SHELL ["/bin/bash", "-c"]

# Set non-interactive frontend for apt-get to skip any user confirmations
ENV DEBIAN_FRONTEND=noninteractive

# =======================
# 1. Install dependencies
# =======================
RUN set -e && apt update && apt install --no-install-recommends -y \
    git \
    cmake \
    ninja-build \
    gperf \
    ccache \
    dfu-util \
    device-tree-compiler \
    wget \
    python3-dev \
    python3-pip \
    python3-setuptools \
    python3-tk \
    python3-wheel \
    python3-venv \
    xz-utils \
    file \
    make \
    gcc \
    gcc-multilib \
    g++-multilib \
    libsdl2-dev \
    libmagic1 \
    openssh-server

# ===================================================================================
# 2. Set root password, set up directories, ssh, allow root login, expose ssh port 22
# ===================================================================================
RUN echo "root:${PASSWORD}" | chpasswd

RUN mkdir -p /workspace/

RUN mkdir -p /var/run/sshd && \
    chmod 0755 /var/run/sshd
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

EXPOSE 22

# ==================================
# 3. Setup Python venv, install west
# ==================================
RUN python3 -m venv /venv
ENV PATH="/venv/bin:$PATH"
RUN python3 -m pip install --no-cache-dir west

# Override the west manifest to only install necessary modules
COPY west.yml /workspace/west.yml

# =======================
# 4. Setup west workspace
# =======================
RUN cd /workspace && \
    west init --local --mf west.yml && \
    west update && \
    west zephyr-export

RUN pip install -r "/zephyr/scripts/requirements.txt"

RUN west sdk install --version ${ZEPHYR_SDK_VERSION} --install-base /

ENTRYPOINT /usr/sbin/sshd && /bin/bash