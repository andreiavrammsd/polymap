# Use the official Ubuntu base image
FROM ubuntu:26.04

# Avoid interactive package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    curl \
    wget \
    unzip \
    python3 \
    python3-pip \
    gdb \
    lldb \
    clang \
    clang-format \
    lcov \
    llvm \
    software-properties-common \
    sudo \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Install Bazelisk as "bazel"
ARG BAZELISK_VERSION=1.29.0
RUN curl -fsSL \
    "https://github.com/bazelbuild/bazelisk/releases/download/v${BAZELISK_VERSION}/bazelisk-linux-amd64" \
    -o /usr/local/bin/bazel \
    && chmod +x /usr/local/bin/bazel

# Install Buildifier
ARG BUILDIFIER_VERSION=7.3.1
RUN curl -fsSL \
    "https://github.com/bazelbuild/buildtools/releases/download/v${BUILDIFIER_VERSION}/buildifier-linux-amd64" \
    -o /usr/local/bin/buildifier \
    && chmod +x /usr/local/bin/buildifier

# Install CMake
RUN wget https://cmake.org/files/v3.22/cmake-3.22.6-linux-x86_64.sh \
    && chmod +x cmake-3.22.6-linux-x86_64.sh \
    && sudo ./cmake-3.22.6-linux-x86_64.sh --prefix=/usr/local --skip-license

# Create a non-root user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN usermod --login ${USERNAME} --home /home/${USERNAME} --move-home $(getent passwd ${USER_UID} | cut -d: -f1) \
    && usermod --gid ${USER_GID} ${USERNAME} \
    && passwd -d ${USERNAME} \
    && printf '%s ALL=(ALL) NOPASSWD:ALL\n' "${USERNAME}" > "/etc/sudoers.d/${USERNAME}" \
    && chmod 0440 "/etc/sudoers.d/${USERNAME}" \
    && visudo -cf "/etc/sudoers.d/${USERNAME}"

# Set the user to vscode
USER $USERNAME

# Set the workspace directory
WORKDIR /workspace
