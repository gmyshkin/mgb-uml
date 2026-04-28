#!/usr/bin/env python3
"""Package the MGB-UML plugin SDK.

The SDK intentionally includes only the plugin-facing headers and examples
needed by third-party developers. It preserves the repository-style `src/...`
include layout under `include/mgb-uml` so existing plugin examples can include
headers such as `src/data/mgbPluginInterface.h`.
"""

from __future__ import annotations

import argparse
import os
import shutil
import tarfile
import tempfile
import zipfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

SDK_SOURCE_DIRS = [
    ROOT / "sdk" / "docs",
    ROOT / "sdk" / "templates",
]

SDK_SOURCE_FILES = [
    ROOT / "sdk" / "README.md",
    ROOT / "COPYING",
    ROOT / "VERSION.txt",
]

PLUGIN_HEADER_FILES = [
    ROOT / "src" / "mgb_api.h",
    ROOT / "src" / "util.h",
    ROOT / "src" / "data" / "mgbPluginInterface.h",
    ROOT / "src" / "data" / "mgbPluginManager.h",
    ROOT / "src" / "data" / "node.h",
    ROOT / "src" / "data" / "style.h",
    ROOT / "src" / "data" / "graphelementdata.h",
    ROOT / "src" / "data" / "graphelementproperty.h",
    ROOT / "src" / "gui" / "nodeitem.h",
]


def copy_file(src: Path, dst: Path) -> None:
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)


def build_sdk_tree(stage: Path, platform_name: str) -> Path:
    sdk_root = stage / f"mgb-uml-plugin-sdk-{platform_name}"
    sdk_root.mkdir(parents=True)

    for src in SDK_SOURCE_FILES:
        if src.exists():
            copy_file(src, sdk_root / src.name)

    for src_dir in SDK_SOURCE_DIRS:
        if src_dir.exists():
            shutil.copytree(src_dir, sdk_root / src_dir.name)

    for header in PLUGIN_HEADER_FILES:
        if not header.exists():
            raise FileNotFoundError(f"Required SDK header is missing: {header}")
        rel = header.relative_to(ROOT)
        copy_file(header, sdk_root / "include" / "mgb-uml" / rel)

    manifest = sdk_root / "SDK_MANIFEST.txt"
    manifest.write_text(
        "\n".join(
            [
                "MGB-UML Plugin SDK",
                f"Platform package: {platform_name}",
                "",
                "Contents:",
                "- include/mgb-uml: plugin-facing MGB-UML headers",
                "- docs: plugin development guide",
                "- templates/basic-node-plugin: working compiled plugin template",
                "",
                "Set MGB_UML_SDK to this directory before building the template.",
                "",
            ]
        ),
        encoding="utf-8",
    )

    return sdk_root


def make_zip(src_dir: Path, output: Path) -> None:
    with zipfile.ZipFile(output, "w", zipfile.ZIP_DEFLATED) as archive:
        for path in src_dir.rglob("*"):
            archive.write(path, path.relative_to(src_dir.parent))


def make_tar_gz(src_dir: Path, output: Path) -> None:
    with tarfile.open(output, "w:gz") as archive:
        archive.add(src_dir, arcname=src_dir.name)


def main() -> int:
    parser = argparse.ArgumentParser(description="Package the MGB-UML plugin SDK")
    parser.add_argument("--platform", required=True, help="Platform label, e.g. linux, windows, macos")
    parser.add_argument("--output", required=True, type=Path, help="Output .zip or .tar.gz path")
    args = parser.parse_args()

    output = args.output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)

    with tempfile.TemporaryDirectory() as tmp:
        sdk_root = build_sdk_tree(Path(tmp), args.platform)
        if output.name.endswith(".tar.gz") or output.name.endswith(".tgz"):
            make_tar_gz(sdk_root, output)
        elif output.suffix == ".zip":
            make_zip(sdk_root, output)
        else:
            raise ValueError("Output must end with .zip, .tar.gz, or .tgz")

    print(f"Created {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
