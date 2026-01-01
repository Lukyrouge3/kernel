FROM debian:bookworm

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    bison flex \
    libgmp3-dev libmpc-dev libmpfr-dev \
    make \
    texinfo \
    wget xz-utils \
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

# Where the cross toolchain will be installed
ENV PREFIX=/opt/cross
ENV TARGET=i686-elf
ENV PATH="${PREFIX}/bin:${PATH}"

WORKDIR /src
COPY build-toolchain.sh /src/build-toolchain.sh
RUN chmod +x /src/build-toolchain.sh

RUN /src/build-toolchain.sh

RUN useradd -m -s /bin/bash dev
WORKDIR /work
USER dev