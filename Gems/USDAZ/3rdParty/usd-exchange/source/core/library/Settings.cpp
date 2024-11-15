// SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
// SPDX-License-Identifier: LicenseRef-NvidiaProprietary
//
// NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
// property and proprietary rights in and to this material, related
// documentation and any modifications thereto. Any use, reproduction,
// disclosure or distribution of this material and related documentation
// without an express license agreement from NVIDIA CORPORATION or
// its affiliates is strictly prohibited.

#include "usdex/core/Settings.h"

using namespace pxr;

namespace usdex::core
{

TF_DEFINE_ENV_SETTING(
    USDEX_ENABLE_OMNI_TRANSCODING,
    true,
    "Use the omni::transcoding bootstring implementation when validating Prim and Property names"
);

} // namespace usdex::core

namespace
{

// This is a workaround for an optimization issue on Windows. If the TfEnvSettings are unused in the compilation unit in which
// they are defined, they may be compiled out of the resulting object file. We did not exhaustively test all optimization settings,
// but this issue did not occur on a Windows Debug build with optimization disabled. It also has not been seen on Linux, neither in
// release nor debug builds, though only a limited compiler set was tested (gcc 7 and gcc 11). In any case, we can force the symbols
// to remain if we use them directly. Each setting above must be used explicitly in the LoadSettings constructor below.
struct LoadSettings
{
    LoadSettings()
    {
        TfGetEnvSetting(usdex::core::USDEX_ENABLE_OMNI_TRANSCODING);
    }
};

static LoadSettings g_settings;

} // namespace
