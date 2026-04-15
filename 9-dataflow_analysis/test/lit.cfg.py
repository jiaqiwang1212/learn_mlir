# -*- Python -*-
# lit configuration for ch-9 (dataflow_analysis) tests.
#
# Run all tests:
#   llvm-lit <path-to-this-dir>
#
# Run a single file:
#   llvm-lit <path-to-this-dir>/filecheck_const_prop.mlir

import os
import lit.formats

config.name = "ch-9-dataflow"
config.test_format = lit.formats.ShTest(execute_external=True)

config.suffixes = [".mlir"]

config.test_source_root = os.path.dirname(__file__)

_repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))

config.test_exec_root = os.environ.get(
    "LIT_EXEC_ROOT",
    os.path.join(_repo_root, "build", "test", "ch-9"),
)

LLVM_BIN = os.environ.get(
    "LLVM_BIN",
    os.path.join(_repo_root, "externals", "llvm-project", "build", "bin"),
)
INSTALL_BIN = os.environ.get(
    "INSTALL_BIN",
    os.path.join(_repo_root, "install", "bin"),
)

config.substitutions.append(("ch-9-opt", os.path.join(INSTALL_BIN, "ch-9-opt")))
config.substitutions.append(("FileCheck", os.path.join(LLVM_BIN, "FileCheck")))
config.substitutions.append(("split-file", os.path.join(LLVM_BIN, "split-file")))
