# Use the official Ubuntu base image
FROM ubuntu:20.04

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
    llvm \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Install Bazel
RUN apt-get update && apt-get install -y apt-transport-https curl gnupg && \
    curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor >bazel-archive-keyring.gpg && \
    mv bazel-archive-keyring.gpg /usr/share/keyrings/bazel-archive-keyring.gpg && \
    echo "deb [signed-by=/usr/share/keyrings/bazel-archive-keyring.gpg] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list && \
    apt-get update && apt-get install -y bazel && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Upgrade Bazel
RUN apt-get update && apt-get full-upgrade -y bazel

# Create a non-root user
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME -s /bin/bash \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(ALL\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

RUN wget https://github.com/bazelbuild/buildtools/releases/download/v7.3.1/buildifier-linux-amd64 \
    && chmod +x buildifier-linux-amd64 \
    && sudo mv buildifier-linux-amd64 /usr/local/bin/buildifier

RUN wget https://cmake.org/files/v3.22/cmake-3.22.6-linux-x86_64.sh \
    && chmod +x cmake-3.22.6-linux-x86_64.sh \
    && sudo ./cmake-3.22.6-linux-x86_64.sh --prefix=/usr/local --skip-license

# Set the user to vscode
USER $USERNAME

# Set the workspace directory
WORKDIR /workspace
