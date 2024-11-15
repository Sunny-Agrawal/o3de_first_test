// SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
// SPDX-License-Identifier: LicenseRef-NvidiaProprietary
//
// NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
// property and proprietary rights in and to this material, related
// documentation and any modifications thereto. Any use, reproduction,
// disclosure or distribution of this material and related documentation
// without an express license agreement from NVIDIA CORPORATION or
// its affiliates is strictly prohibited.

#pragma once

//! @file usdex/core/LayerAlgo.h
//! @brief Utility functions to provide consistant authoring of `SdfLayers`

#include "Api.h"

#include <pxr/usd/sdf/layer.h>

#include <optional>
#include <string_view>

namespace usdex::core
{

//! @defgroup layers SdfLayer Authoring
//!
//! Utility functions to provide consistant authoring of `SdfLayers`
//!
//! It is desirable to capture data provenance information into the metadata of SdfLayers, in order to keep track of what tools & versions
//! were used throughout content creation pipelines, and to capture notes from the content author. While OpenUSD does not currently have a
//! formal specification for this authoring metadata, some conventions have emerged throughout the OpenUSD Ecosystem.
//!
//! The most common convention for tool tracking is to include a "creator" string in the `SdfLayer::CustomLayerData`. Similarly, notes from
//! the content author should be captured via `SdfLayer::SetComment`. While these are trivial using `SdfLayer` public methods, they are
//! also easy to forget, and difficult to discover.
//!
//! These functions assist authoring applications in setting authoring metadata, so that each application can produce consistant provenance
//! information. The metadata should only add information which can be used to track the data back to its origin. It should not be used to
//! store sensitive information about the content, nor about the end user (i.e. do not use it to store Personal Identifier Information).
//!
//! Example:
//!
//!     auto layer = SdfLayer::CreateAnonymous();
//!     static constexpr const char* s_authoringMetadata = "My Content Editor® 2024 SP 2, USD Exporter Plugin v1.1.23.11";
//!     usdex::core::exportLayer(layer, fileName, s_authoringMetadata, userComment);
//!
//! @note This metadata is strictly informational, it is not advisable to drive runtime behavior from this metadata.
//! In the future, the "creator" key may change, or a more formal specification for data provenance may emerge.
//!
//! @{

//! Check if the `SdfLayer` has metadata indicating the provenance of the data.
//!
//! @note This metadata is strictly informational, it is not advisable to drive runtime behavior from this metadata.
//! In the future, the "creator" key may change, or a more formal specification for data provenance may emerge.
//!
//! Checks the CustomLayerData for a "creator" key.
//!
//! @param layer The layer to check
USDEX_API bool hasLayerAuthoringMetadata(pxr::SdfLayerHandle layer);

//! Set metadata on the `SdfLayer` indicating the provenance of the data.
//!
//! @note This metadata is strictly informational, it is not advisable to drive runtime behavior from this metadata.
//! In the future, the "creator" key may change, or a more formal specification for data provenance may emerge.
//!
//! This function stores the provided provenance information into a "creator" key of the CustomLayerData.
//! There is no formal specification for the format of this data, but it should only to add information which can be used to track
//! the data back to its product of origin. It should not be used to store information about the content itself, nor about the end user (PII).
//!
//! @param layer The layer to modify
//! @param value The provenance information for this layer
USDEX_API void setLayerAuthoringMetadata(pxr::SdfLayerHandle layer, const std::string& value);

//! Save the given `SdfLayer` with an optional comment.
//!
//! @note This does not impact sublayers or any stages that this layer may be contributing to. This is to
//! preserve authoring metadata on referenced layers that came from other applications. See @ref layers
//! for more details on `setLayerAuthoringMetadata`.
//!
//! @param layer The layer to be saved.
//! @param authoringMetadata The provenance information from the host application. See @ref layers for details.
//! @param comment The comment will be authored in the layer as the `SdfLayer` comment.
//! @returns A bool indicating if the save was successful.
USDEX_API bool saveLayer(
    pxr::SdfLayerHandle layer,
    std::optional<std::string_view> authoringMetadata = std::nullopt,
    std::optional<std::string_view> comment = std::nullopt
);

//! Export the given `SdfLayer` to an identifier with an optional comment.
//!
//! @note This does not impact sublayers or any stages that this layer may be contributing to. This is to
//! preserve authoring metadata on referenced layers that came from other applications. See @ref layers
//! for more details on `setLayerAuthoringMetadata`.
//!
//! @param layer The layer to be exported.
//! @param identifier The identifier to be used for the new layer.
//! @param authoringMetadata The provenance information from the host application. See @ref layers for details.
//! @param comment The comment will be authored in the layer as the `SdfLayer` comment.
//! @param fileFormatArgs Additional file format-specific arguments to be supplied during layer export.
//! @returns A bool indicating if the export was successful.
USDEX_API bool exportLayer(
    pxr::SdfLayerHandle layer,
    const std::string& identifier,
    const std::string& authoringMetadata,
    std::optional<std::string_view> comment = std::nullopt,
    const pxr::SdfLayer::FileFormatArguments& fileFormatArgs = pxr::SdfLayer::FileFormatArguments()
);

//! @}

} // namespace usdex::core
