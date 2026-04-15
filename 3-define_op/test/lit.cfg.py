# -*- Python -*-
# lit configuration for ch-3 (define_op) tests.
#
# Run all tests:
#   llvm-lit <path-to-this-dir>
#
# Run a single file:
#   llvm-lit <path-to-this-dir>/filecheck_ops.mlir

import os
import lit.formats

# ---------------------------------------------------------------------------
# Basic lit config
# ---------------------------------------------------------------------------
config.name = "ch-3-siren"
config.test_format = lit.formats.ShTest(execute_external=True)

# Only treat .mlir files as tests
config.suffixes = [".mlir"]

# Where the test source files live (this directory)
config.test_source_root = os.path.dirname(__file__)

# ---------------------------------------------------------------------------
# Tool paths
# ---------------------------------------------------------------------------
# Walk up from this file to the repo root, then resolve tool directories.
_repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

# Where lit writes Output/ (script files, logs). Keeps build artifacts out of
# the source tree. Mirrors the standard LLVM pattern of using a build dir.
config.test_exec_root = os.environ.get(
    "LIT_EXEC_ROOT",
    os.path.join(_repo_root, "build", "test", "ch-3"),
)

LLVM_BIN = os.environ.get(
    "LLVM_BIN",
    os.path.join(_repo_root, "externals", "llvm-project", "build", "bin"),
)
INSTALL_BIN = os.environ.get(
    "INSTALL_BIN",
    os.path.join(_repo_root, "install", "bin"),
)

# Define substitutions so RUN lines can use tool names without full paths.
# lit replaces these tokens in RUN: lines before executing the shell command.
config.substitutions.append(("ch-3-opt", os.path.join(INSTALL_BIN, "ch-3-opt")))
config.substitutions.append(("FileCheck", os.path.join(LLVM_BIN, "FileCheck")))
config.substitutions.append(("split-file", os.path.join(LLVM_BIN, "split-file")))

# Exclude legacy test files that have no RUN: lines yet.
config.excludes = ["siren.mlir", "test_liveness.mlir"]
