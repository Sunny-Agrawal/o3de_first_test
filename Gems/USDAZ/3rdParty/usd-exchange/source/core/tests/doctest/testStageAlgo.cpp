// SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
// SPDX-License-Identifier: LicenseRef-NvidiaProprietary
//
// NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
// property and proprietary rights in and to this material, related
// documentation and any modifications thereto. Any use, reproduction,
// disclosure or distribution of this material and related documentation
// without an express license agreement from NVIDIA CORPORATION or
// its affiliates is strictly prohibited.

#include <usdex/test/FilesystemUtils.h>
#include <usdex/test/ScopedDiagnosticChecker.h>

#include <usdex/core/Core.h>
#include <usdex/core/Feature.h>
#include <usdex/core/LayerAlgo.h>
#include <usdex/core/StageAlgo.h>
#include <usdex/core/Version.h>

#include <pxr/base/vt/dictionary.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/usdFileFormat.h>
#include <pxr/usd/usd/usdaFileFormat.h>
#include <pxr/usd/usd/usdcFileFormat.h>
#include <pxr/usd/usdGeom/metrics.h>
#include <pxr/usd/usdGeom/tokens.h>

#include <doctest/doctest.h>

using namespace usdex::test;
using namespace pxr;

namespace
{

std::string getAuthoringMetadata()
{
    return TfStringPrintf("usdex cpp tests: %s, usd_ver: %d, with_python: %d", usdex::core::version(), PXR_VERSION, usdex::core::withPython());
}

// FUTURE: this is included in both python and c++ tests. Is it useful at runtime? Maybe it belongs in usdex::core instead
TfToken getUsdEncoding(const SdfLayer& layer)
{
    SdfFileFormatConstPtr fileFormat = layer.GetFileFormat();

    // If the encoding is explicit usda return that extension
    if (fileFormat == SdfFileFormat::FindById(UsdUsdaFileFormatTokens->Id))
    {
        return UsdUsdaFileFormatTokens->Id;
    }

    // If the encoding is explicit usdc return that extension
    if (fileFormat == SdfFileFormat::FindById(UsdUsdcFileFormatTokens->Id))
    {
        return UsdUsdcFileFormatTokens->Id;
    }

    if (fileFormat == SdfFileFormat::FindById(UsdUsdFileFormatTokens->Id))
    {
        return UsdUsdFileFormat::GetUnderlyingFormatForLayer(layer);
    }

    return {};
}

} // namespace

TEST_CASE("createStage identifier")
{
    const std::string defaultPrimName = "Root";
    const TfToken& upAxis = UsdGeomTokens->y;
    const double linearUnits = UsdGeomLinearUnits::meters;
    const std::string authoringMetadata = ::getAuthoringMetadata();

    UsdStageRefPtr stage = nullptr;

    // empty identifier
    std::string identifier = "";
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid identifier" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // unsupported identifier
    usdex::test::ScopedTmpDir tmpDir;
    identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.foo");
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid identifier" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // valid usda
    identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usda");
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    REQUIRE(stage != nullptr);
    CHECK(usdex::test::compareIdentifiers(stage->GetRootLayer()->GetIdentifier(), identifier));
    CHECK(getUsdEncoding(*stage->GetRootLayer()) == UsdUsdaFileFormatTokens->Id);
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));

    // valid usdc
    identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usdc");
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    REQUIRE(stage != nullptr);
    CHECK(usdex::test::compareIdentifiers(stage->GetRootLayer()->GetIdentifier(), identifier));
    CHECK(getUsdEncoding(*stage->GetRootLayer()) == UsdUsdcFileFormatTokens->Id);
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));

    // valid usd results in usdc
    identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usd");
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    REQUIRE(stage != nullptr);
    CHECK(usdex::test::compareIdentifiers(stage->GetRootLayer()->GetIdentifier(), identifier));
    CHECK(getUsdEncoding(*stage->GetRootLayer()) == UsdUsdcFileFormatTokens->Id);
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));
    stage = nullptr;
}

TEST_CASE("createStage defaultPrim")
{
    usdex::test::ScopedTmpDir tmpDir;
    const std::string identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usda");
    const TfToken& upAxis = UsdGeomTokens->y;
    const double linearUnits = UsdGeomLinearUnits::meters;
    const std::string authoringMetadata = ::getAuthoringMetadata();

    UsdStageRefPtr stage = nullptr;

    // invalid default prim name
    std::string defaultPrimName = "";
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid default prim name.*" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // valid default prim name
    defaultPrimName = "root";
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    CHECK(stage->GetDefaultPrim().GetName() == defaultPrimName);

    // It is valid to reuse an identifier
    // The new prim will be defined on the stage and be accessible as the default prim
    defaultPrimName = "Root";
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    CHECK(stage->GetDefaultPrim().GetName() == defaultPrimName);
}

TEST_CASE("createStage upAxis")
{
    usdex::test::ScopedTmpDir tmpDir;
    const std::string identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usda");
    const std::string defaultPrimName = "Root";
    const double linearUnits = UsdGeomLinearUnits::meters;
    const std::string authoringMetadata = ::getAuthoringMetadata();

    UsdStageRefPtr stage = nullptr;

    // empty axis
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics: Unsupported up axis value \"\"" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, {}, linearUnits, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // a non-axis token is not valid
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics: Unsupported up axis value.*" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, UsdGeomTokens->none, linearUnits, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // x is an invalid axis
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics: Unsupported up axis value.*" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, UsdGeomTokens->x, linearUnits, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // z is a valid axis
    stage = usdex::core::createStage(identifier, defaultPrimName, UsdGeomTokens->z, linearUnits, authoringMetadata);
    CHECK(UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->z);

    TfToken Z("Z");
    stage = usdex::core::createStage(identifier, defaultPrimName, Z, linearUnits, authoringMetadata);
    CHECK(UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->z);

    TfToken z("z");
    stage = usdex::core::createStage(identifier, defaultPrimName, z, linearUnits, authoringMetadata);
    CHECK(UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->z);

    // y is a valid axis
    stage = usdex::core::createStage(identifier, defaultPrimName, UsdGeomTokens->y, linearUnits, authoringMetadata);
    CHECK(UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->y);

    TfToken Y("Y");
    stage = usdex::core::createStage(identifier, defaultPrimName, Y, linearUnits, authoringMetadata);
    CHECK(UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->y);

    TfToken y("y");
    stage = usdex::core::createStage(identifier, defaultPrimName, y, linearUnits, authoringMetadata);
    CHECK(UsdGeomGetStageUpAxis(stage) == UsdGeomTokens->y);
    stage = nullptr;
}

TEST_CASE("createStage linearUnits")
{
    usdex::test::ScopedTmpDir tmpDir;
    const std::string identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usda");
    const std::string defaultPrimName = "Root";
    const TfToken& upAxis = UsdGeomTokens->y;
    const std::string authoringMetadata = ::getAuthoringMetadata();

    UsdStageRefPtr stage = nullptr;

    // invalid units
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics: Linear units value.*" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, 0.0, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // negative units are invalid
    {
        ScopedDiagnosticChecker check({ { TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics: Linear units value.*" } });
        stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, -1.0, authoringMetadata);
    }
    CHECK(stage == nullptr);
    CHECK(SdfLayer::Find(identifier) == nullptr);

    // valid units
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, UsdGeomLinearUnits::nanometers, authoringMetadata);
    CHECK(UsdGeomGetStageMetersPerUnit(stage) == UsdGeomLinearUnits::nanometers);
    CHECK(UsdGeomStageHasAuthoredMetersPerUnit(stage));

    // default units are authored
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, UsdGeomLinearUnits::meters, authoringMetadata);
    CHECK(UsdGeomGetStageMetersPerUnit(stage) == UsdGeomLinearUnits::meters);
    CHECK(UsdGeomStageHasAuthoredMetersPerUnit(stage));
    stage = nullptr;
}

TEST_CASE("createStage authoringMetadata")
{
    usdex::test::ScopedTmpDir tmpDir;
    const std::string identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usd");
    const std::string defaultPrimName = "Root";
    const TfToken& upAxis = UsdGeomTokens->y;
    const double linearUnits = UsdGeomLinearUnits::meters;
    const std::string authoringMetadata = ::getAuthoringMetadata();

    VtDictionary expectedData;
    expectedData["creator"] = authoringMetadata;

    // The authoring metadata is required
    UsdStageRefPtr stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata);
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));
    CHECK(stage->GetRootLayer()->GetCustomLayerData() == expectedData);

    // The value is arbitrary
    expectedData["creator"] = "foo";
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, "foo");
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));
    CHECK(stage->GetRootLayer()->GetCustomLayerData() == expectedData);
    stage = nullptr;
}

TEST_CASE("createStage fileFormatArgs")
{
    usdex::test::ScopedTmpDir tmpDir;
    const std::string identifier = TfStringPrintf("%s/%s", tmpDir.getPath(), "test.usd");
    const std::string defaultPrimName = "Root";
    const TfToken& upAxis = UsdGeomTokens->y;
    const double linearUnits = UsdGeomLinearUnits::meters;
    const std::string authoringMetadata = ::getAuthoringMetadata();

    // respects FileFormatArgs
    SdfLayer::FileFormatArguments args;
    args["format"] = "usda";
    UsdStageRefPtr stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata, args);
    REQUIRE(stage != nullptr);
    CHECK(usdex::test::compareIdentifiers(stage->GetRootLayer()->GetIdentifier(), identifier));
    CHECK(getUsdEncoding(*stage->GetRootLayer()) == UsdUsdaFileFormatTokens->Id);
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));

    // respects FileFormatArgs
    args["format"] = "usdc";
    stage = usdex::core::createStage(identifier, defaultPrimName, upAxis, linearUnits, authoringMetadata, args);
    REQUIRE(stage != nullptr);
    CHECK(usdex::test::compareIdentifiers(stage->GetRootLayer()->GetIdentifier(), identifier));
    CHECK(getUsdEncoding(*stage->GetRootLayer()) == UsdUsdcFileFormatTokens->Id);
    CHECK(usdex::core::hasLayerAuthoringMetadata(stage->GetRootLayer()));
    stage = nullptr;
}
