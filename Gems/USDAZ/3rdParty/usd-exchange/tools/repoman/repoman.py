# SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: LicenseRef-NvidiaProprietary
#
# NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
# property and proprietary rights in and to this material, related
# documentation and any modifications thereto. Any use, reproduction,
# disclosure or distribution of this material and related documentation
# without an express license agreement from NVIDIA CORPORATION or
# its affiliates is strictly prohibited.
import contextlib
import io
import os
import sys

import packmanapi

REPO_ROOT = os.path.join(os.path.dirname(os.path.realpath(__file__)), "../..")
REPO_DEPS_FILE = os.path.join(REPO_ROOT, "deps/repo-deps.packman.xml")
REPO_DEPS_OPTIONAL_FILE = os.path.join(REPO_ROOT, "deps/repo-deps-nv.packman.xml")


def bootstrap():
    """
    Bootstrap all omni.repo modules.

    Pull with packman from repo.packman.xml and add them all to python sys.path to enable importing.
    """
    with contextlib.redirect_stdout(io.StringIO()):
        deps = packmanapi.pull(REPO_DEPS_FILE)
        with contextlib.suppress(packmanapi.PackmanErrorFileNotFound):
            deps.update(packmanapi.pull(REPO_DEPS_OPTIONAL_FILE, remotes=["cloudfront"]))

    for dep_path in deps.values():
        if dep_path not in sys.path:
            sys.path.append(dep_path)


if __name__ == "__main__":
    bootstrap()
    import omni.repo.man

    omni.repo.man.main(REPO_ROOT)
