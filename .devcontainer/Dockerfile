FROM debian:bookworm

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    nasm \
    binutils \
    qemu-system-x86 \
    qemu-utils \
    gdb \
    ca-certificates \
    git \
    curl \
    file \
    qemu-system-gui \
    ssh \
 && rm -rf /var/lib/apt/lists/*

# Nice-to-have: keep build artifacts out of root-owned dirs when using bind mounts
RUN useradd -m -s /bin/bash dev
USER dev
WORKDIR /work
