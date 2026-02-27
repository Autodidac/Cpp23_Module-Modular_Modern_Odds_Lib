#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT}/build/linux"
CONFIG="${1:-Debug}"

TOOLCHAIN_ARG=()
if [[ -n "${VCPKG_ROOT:-}" ]]; then
  TOOLCHAIN="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
  if [[ -f "${TOOLCHAIN}" ]]; then
    TOOLCHAIN_ARG=(-DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN}")
  else
    echo "WARNING: VCPKG_ROOT is set but toolchain not found: ${TOOLCHAIN}" >&2
  fi
fi

cmake -S "${ROOT}" -B "${BUILD_DIR}" -G Ninja \
  -DCMAKE_BUILD_TYPE="${CONFIG}" \
  "${TOOLCHAIN_ARG[@]}"

cmake --build "${BUILD_DIR}" --config "${CONFIG}"

exec "${BUILD_DIR}/ayejay_odds_example"
