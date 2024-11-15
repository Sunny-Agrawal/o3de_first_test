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

#include "usdex/core/LayerAlgo.h"

#include "usdex/pybind/UsdBindings.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace usdex::core;
using namespace pybind11;

namespace usdex::core::bindings
{

void bindLayerAlgo(module& m)
{
    m.def(
        "hasLayerAuthoringMetadata",
        &hasLayerAuthoringMetadata,
        arg("layer"),
        R"(
            Check if the ``Sdf.Layer`` has metadata indicating the provenance of the data.

            Note:

                This metadata is strictly informational, it is not advisable to drive runtime behavior from this metadata.
                In the future, the "creator" key may change, or a more formal specification for data provenance may emerge.

            Checks the CustomLayerData for a "creator" key.

            Args:
                layer: The layer to check

            Returns:
                A bool indicating if the metadata exists
        )"
    );

    m.def(
        "setLayerAuthoringMetadata",
        &setLayerAuthoringMetadata,
        arg("layer"),
        arg("value"),
        R"(
            Set metadata on the ``Sdf.Layer`` indicating the provenance of the data.

            It is desirable to capture data provenance information into the metadata of SdfLayers, in order to keep track of what tools & versions
            were used throughout content creation pipelines, and to capture notes from the content author. While OpenUSD does not currently have a
            formal specification for this authoring metadata, some conventions have emerged throughout the OpenUSD Ecosystem.

            The most common convention for tool tracking is to include a "creator" string in the ``Sdf.Layer.customLayerData``. Similarly, notes from
            the content author should be captured via ``Sdf.Layer.SetComment``. While these are trivial using ``Sdf.Layer`` public methods, they are
            also easy to forget, and difficult to discover.

            This function assists authoring applications in settings authoring metadata, so that each application can produce consistant provenance
            information. The metadata should only add information which can be used to track the data back to its origin. It should not be used to
            store sensitive information about the content, nor about the end user (i.e. do not use it to store Personal Identifier Information).

            Example:

                .. code-block:: python

                    layer = Sdf.Layer.CreateAnonymous()
                    authoring_metadata = "My Content Editor® 2024 SP 2, USD Exporter Plugin v1.1.23.11"
                    usdex.core.exportLayer(layer, file_name, authoring_metadata, user_comment);

            Note:

                This metadata is strictly informational, it is not advisable to drive runtime behavior from this metadata.
                In the future, the "creator" key may change, or a more formal specification for data provenance may emerge.

            Args:
                layer: The layer to modify
                value: The provenance information for this layer
        )"
    );

    m.def(
        "saveLayer",
        &saveLayer,
        arg("layer"),
        arg("authoringMetadata") = nullptr,
        arg("comment") = nullptr,
        R"(
            Save the given ``Sdf.Layer`` with an optional comment

            Note:

                This does not impact sublayers or any stages that this layer may be contributing to. See ``setLayerAuthoringMetadata`` for details.
                This is to preserve authoring metadata on referenced layers that came from other applications.

            Args:
                layer: The stage to be saved.
                authoringMetadata: The provenance information from the host application. See ``setLayerAuthoringMetadata`` for details.
                comment: The comment will be authored in the layer as the ``Sdf.Layer`` comment.

             Returns:
                A bool indicating if the save was successful.
        )"
    );

    m.def(
        "exportLayer",
        &exportLayer,
        arg("layer"),
        arg("identifier"),
        arg("authoringMetadata"),
        arg("comment") = nullptr,
        arg("fileFormatArgs") = pxr::SdfLayer::FileFormatArguments(),
        R"(
            Export the given ``Sdf.Layer`` to an identifier with an optional comment.

            Note:

                This does not impact sublayers or any stages that this layer may be contributing to. See ``setLayerAuthoringMetadata`` for details.
                This is to preserve authoring metadata on referenced layers that came from other applications.

            Args:
                layer: The layer to be exported.
                identifier: The identifier to be used for the new layer.
                authoringMetadata: The provenance information from the host application. See ``setLayerAuthoringMetadata`` for details.
                    If the "creator" key already exists, it will not be overwritten & this data will be ignored.
                comment: The comment will be authored in the layer as the ``Sdf.Layer`` comment.
                fileFormatArgs: Additional file format-specific arguments to be supplied during layer export.

            Returns:
                A bool indicating if the export was successful.
        )"
    );
}

} // namespace usdex::core::bindings
