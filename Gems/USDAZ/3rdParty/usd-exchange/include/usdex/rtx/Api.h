// SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
// SPDX-License-Identifier: LicenseRef-NvidiaProprietary
//
// NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
// property and proprietary rights in and to this material, related
// documentation and any modifications thereto. Any use, reproduction,
// disclosure or distribution of this material and related documentation
// without an express license agreement from NVIDIA CORPORATION or
// its affiliates is strictly prohibited.

#pragma once

//! @file usdex/rtx/Api.h
//! @brief Symbol macros for this library

#include "usdex/core/Api.h"

//! Define USDEX_RTX_API macro based on whether or not we are compiling usdex_rtx,
//! or including headers for linking to it. Functions that wish to be exported from a .dll/.so
//! should be decorated with USDEX_RTX_API.
#ifdef usdex_rtx_EXPORTS
#define USDEX_RTX_API USDEX_EXPORT
#else
#define USDEX_RTX_API USDEX_IMPORT
#endif
