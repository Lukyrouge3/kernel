#!/usr/bin/env bash
set -euo pipefail

: "${PREFIX:=/opt/cross}"
: "${TARGET:=i686-elf}"
: "${NPROC:=1}"

# Pick stable, known-good versions
BINUTILS_VER=2.42
GCC_VER=14.2.0

mkdir -p /tmp/build
cd /tmp/build

echo "==> Downloading sources..."
wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VER}.tar.xz
wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VER}/gcc-${GCC_VER}.tar.xz

echo "==> Extracting..."
tar -xf binutils-${BINUTILS_VER}.tar.xz
tar -xf gcc-${GCC_VER}.tar.xz

echo "==> Building binutils..."
mkdir -p build-binutils
cd build-binutils
../binutils-${BINUTILS_VER}/configure \
  --target="${TARGET}" \
  --prefix="${PREFIX}" \
  --with-sysroot \
  --disable-nls \
  --disable-werror
make -j"$(nproc)"
make install
cd ..

echo "==> Preparing gcc prerequisites..."
cd gcc-${GCC_VER}
# GCC downloads prerequisites (gmp/mpfr/mpc/isl) if needed, but we installed libs already.
# Still fine to run this; it will just skip/reuse.
./contrib/download_prerequisites || true
cd ..

echo "==> Building gcc (C only, freestanding-friendly)..."
mkdir -p build-gcc
cd build-gcc
../gcc-${GCC_VER}/configure \
  --target="${TARGET}" \
  --prefix="${PREFIX}" \
  --disable-nls \
  --enable-languages=c \
  --without-headers \
  --disable-shared \
  --disable-threads \
  --disable-libssp \
  --disable-libmudflap \
  --disable-libgomp \
  --disable-libquadmath \
  --disable-libatomic \
  --disable-libstdcxx \
  --disable-multilib
make -j"$(nproc)" all-gcc
make -j"$(nproc)" all-target-libgcc
make install-gcc
make install-target-libgcc

echo "==> Toolchain installed to ${PREFIX}"
"${TARGET}-gcc" --version
