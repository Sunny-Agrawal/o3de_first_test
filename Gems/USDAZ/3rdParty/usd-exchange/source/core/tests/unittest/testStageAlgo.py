# SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: LicenseRef-NvidiaProprietary
#
# NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
# property and proprietary rights in and to this material, related
# documentation and any modifications thereto. Any use, reproduction,
# disclosure or distribution of this material and related documentation
# without an express license agreement from NVIDIA CORPORATION or
# its affiliates is strictly prohibited.

import omni.asset_validator
import usdex.core
import usdex.test
from pxr import Gf, Sdf, Tf, Usd, UsdGeom


class CreateStageTestCase(usdex.test.TestCase):
    def assertLayerNotRegistered(self, identifier):
        """Assert that an Sdf.Layer with a given identifier has not been registered"""
        self.assertFalse(Sdf.Layer.Find(identifier))

    def testIdentifier(self):
        # The identifier is required and must be a valid value otherwise the stage will not be created

        # An invalid value will result in an unsuccessful stage creation
        identifier = ""
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid identifier")]):
            stage = usdex.core.createStage(
                identifier,
                self.defaultPrimName,
                self.defaultUpAxis,
                self.defaultLinearUnits,
                self.defaultAuthoringMetadata,
            )
        self.assertIsNone(stage)

        identifier = self.tmpFile("test", "foo")
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid identifier")]):
            stage = usdex.core.createStage(
                identifier,
                self.defaultPrimName,
                self.defaultUpAxis,
                self.defaultLinearUnits,
                self.defaultAuthoringMetadata,
            )
        self.assertIsNone(stage)

        # A valid "usda" value will result in a successful stage creation
        identifier = self.tmpFile("test", "usda")
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsInstance(stage, Usd.Stage)
        self.assertSdfLayerIdentifier(stage.GetRootLayer(), identifier)
        self.assertUsdLayerEncoding(stage.GetRootLayer(), "usda")
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertIsValidUsd(stage)

        # A valid "usdc" value will result in a successful stage creation
        identifier = self.tmpFile("test", "usdc")
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsInstance(stage, Usd.Stage)
        self.assertSdfLayerIdentifier(stage.GetRootLayer(), identifier)
        self.assertUsdLayerEncoding(stage.GetRootLayer(), "usdc")
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertIsValidUsd(stage)

        # A valid "usd" value will result in a successful stage creation
        # The default encoding of "usdc" will be used
        identifier = self.tmpFile("test", "usd")
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsInstance(stage, Usd.Stage)
        self.assertSdfLayerIdentifier(stage.GetRootLayer(), identifier)
        self.assertUsdLayerEncoding(stage.GetRootLayer(), "usdc")
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertIsValidUsd(stage)

    def testDefaultPrimName(self):
        # The default prim name is required and must be a valid name otherwise the stage will not be created
        identifier = self.tmpFile("test", "usda")

        # An invalid value will result in an unsuccessful stage creation
        value = ""
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid default prim")]):
            stage = usdex.core.createStage(identifier, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

        # A valid value will result in a successful stage creation
        # The prim will be defined on the stage and be accessible as the default prim
        value = "root"
        stage = usdex.core.createStage(identifier, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(stage.GetDefaultPrim().GetName(), value)
        self.assertIsValidUsd(stage)

        # It is valid to reuse an identifier.
        # The new prim will be defined on the stage and be accessible as the default prim
        value = "Root"
        stage = usdex.core.createStage(identifier, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(stage.GetDefaultPrim().GetName(), value)
        self.assertIsValidUsd(stage)

    def testUpAxis(self):
        # The up axis is required and must be one of the two valid values otherwise the stage will not be created
        identifier = self.tmpFile("test", "usda")

        # An invalid value will result in an unsuccessful stage creation
        value = ""
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

        value = UsdGeom.Tokens.none
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics: Unsupported up axis value.*")]):
            stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

        # A valid value will result in a successful stage creation
        # The up axis will be reflected on the stage
        value = UsdGeom.Tokens.z
        stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), value)
        self.assertIsValidUsd(stage)

        # It is valid to reuse an identifier.
        # The new up axis will be reflected on the stage
        value = UsdGeom.Tokens.y
        stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), value)
        self.assertIsValidUsd(stage)

        # From python a string that matches the token can be supplied and will result in a successful stage creation.
        value = "Z"
        stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.z)
        self.assertIsValidUsd(stage)

        value = "Y"
        stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.y)
        self.assertIsValidUsd(stage)

        # To avoid confusion for Python clients that will often be passing the upAxis as a string we support lower case tokens.
        # The chance of confusion is high because `UsdGeom.Tokens.y` and `UsdGeom.Tokens.z` are actually uppercase strings under the hood.
        value = "y"
        stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.y)
        self.assertIsValidUsd(stage)

        value = "z"
        stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.z)
        self.assertIsValidUsd(stage)

        # Build a new identifier so that we can assert failures
        identifier = self.tmpFile("test", "usda")

        # An "X" up axis is considered invalid and result in an unsuccessful stage creation
        value = "X"
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

        value = "x"
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            stage = usdex.core.createStage(identifier, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

    def testLinearUnits(self):
        # The linear units are required and must be a value greater than 0 otherwise the stage will not be created
        identifier = self.tmpFile("test", "usda")

        # An invalid value will result in an unsuccessful stage creation
        value = 0.0
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

        value = -5.0
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertIsNone(stage)
        self.assertLayerNotRegistered(identifier)

        # A valid value will result in a successful stage creation
        # The linear units will be reflected on the stage
        value = UsdGeom.LinearUnits.nanometers
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageMetersPerUnit(stage), value)
        self.assertTrue(UsdGeom.StageHasAuthoredMetersPerUnit(stage))
        self.assertIsValidUsd(stage)

        # It is valid to reuse an identifier.
        # The new linear units will be reflected on the stage
        value = UsdGeom.LinearUnits.meters
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertEqual(UsdGeom.GetStageMetersPerUnit(stage), value)
        self.assertTrue(UsdGeom.StageHasAuthoredMetersPerUnit(stage))
        self.assertIsValidUsd(stage)

    def testAuthoringMetadata(self):
        # The authoring metadata is required
        identifier = self.tmpFile("test", "usda")
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(stage)
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertEqual(stage.GetRootLayer().customLayerData, {"creator": self.defaultAuthoringMetadata})
        self.assertIsValidUsd(stage)

        # The value is arbitrary
        stage = usdex.core.createStage(identifier, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, "foo")
        self.assertTrue(stage)
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertEqual(stage.GetRootLayer().customLayerData, {"creator": "foo"})
        self.assertIsValidUsd(stage)

    def testFileFormatArgs(self):

        # A valid "usd" value will result in a successful stage creation
        # If explicitly set the format will be "usda"
        identifier = self.tmpFile("test", "usd")
        fileFormatArgs = {"format": "usda"}
        stage = usdex.core.createStage(
            identifier,
            self.defaultPrimName,
            self.defaultUpAxis,
            self.defaultLinearUnits,
            self.defaultAuthoringMetadata,
            fileFormatArgs,
        )
        self.assertIsInstance(stage, Usd.Stage)
        self.assertSdfLayerIdentifier(stage.GetRootLayer(), identifier)
        self.assertUsdLayerEncoding(stage.GetRootLayer(), "usda")
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertIsValidUsd(stage)

        # A valid "usd" value will result in a successful stage creation
        # If explicitly set the format will be "usdc"
        identifier = self.tmpFile("test", "usd")
        fileFormatArgs = {"format": "usdc"}
        stage = usdex.core.createStage(
            identifier,
            self.defaultPrimName,
            self.defaultUpAxis,
            self.defaultLinearUnits,
            self.defaultAuthoringMetadata,
            fileFormatArgs,
        )
        self.assertIsInstance(stage, Usd.Stage)
        self.assertSdfLayerIdentifier(stage.GetRootLayer(), identifier)
        self.assertUsdLayerEncoding(stage.GetRootLayer(), "usdc")
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertIsValidUsd(stage)


class ConfigureStageTestCase(usdex.test.TestCase):
    def testDefaultPrimName(self):
        # The default prim name is required and must be a valid name otherwise the stage will not be configured
        stage = Usd.Stage.CreateInMemory()

        # An invalid value will result in an unsuccessful stage configuration
        value = ""
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid default prim name")]):
            result = usdex.core.configureStage(stage, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(stage.HasDefaultPrim())

        # A valid value will result in a successful stage configuration
        # The prim will be defined on the stage and be accessible as the default prim
        value = "root"
        result = usdex.core.configureStage(stage, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(stage.GetDefaultPrim().GetName(), value)
        self.assertEqual(stage.GetDefaultPrim().GetSpecifier(), Sdf.SpecifierDef)
        self.assertEqual(stage.GetDefaultPrim().GetTypeName(), "Scope")
        self.assertIsValidUsd(stage)

        # If the stage is configured again with a different value the configuration will succeed
        # The new prim will be defined on the stage and be accessible as the default prim
        value = "Root"
        result = usdex.core.configureStage(stage, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(stage.GetDefaultPrim().GetName(), value)
        self.assertEqual(stage.GetDefaultPrim().GetSpecifier(), Sdf.SpecifierDef)
        self.assertEqual(stage.GetDefaultPrim().GetTypeName(), "Scope")
        self.assertIsValidUsd(
            stage,
            issuePredicates=[omni.asset_validator.IssuePredicates.ContainsMessage("The prim <root> (/root) is a sibling of the default prim <Root>")],
        )

        # If there is already a prim specified in the root layer with the given name then it will be untouched
        # This includes the specifier and type name
        stage = Usd.Stage.CreateInMemory()
        prim = stage.OverridePrim("/root")

        value = prim.GetName()
        result = usdex.core.configureStage(stage, value, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(stage.GetDefaultPrim().GetName(), value)
        self.assertEqual(stage.GetDefaultPrim().GetSpecifier(), Sdf.SpecifierOver)
        self.assertEqual(stage.GetDefaultPrim().GetTypeName(), "")
        self.assertIsInvalidUsd(
            stage,
            issuePredicates=[
                omni.asset_validator.IssuePredicates.ContainsMessage(
                    "dangling over and does not contain the target prim/property of a relationship or connection attribute."
                ),
                omni.asset_validator.IssuePredicates.ContainsMessage('The default prim <root> of type "" is not Xformable'),
            ],
        )

    def testUpAxis(self):
        # The up axis is required and must be one of the two valid values otherwise the stage will not be configured
        stage = Usd.Stage.CreateInMemory()

        # An invalid value will result in an unsuccessful stage configuration
        value = ""
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(UsdGeom.StageHasAuthoredMetersPerUnit(stage))

        value = UsdGeom.Tokens.none
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(UsdGeom.StageHasAuthoredMetersPerUnit(stage))

        # A valid value will result in a successful stage configuration
        # The up axis will be reflected on the stage
        value = UsdGeom.Tokens.z
        result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), value)
        self.assertIsValidUsd(stage)

        # If the stage is configured again with a different value the configuration will succeed
        # The new up axis will be reflected on the stage
        value = UsdGeom.Tokens.y
        result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), value)
        self.assertIsValidUsd(stage)

        # From python a string that matches the token can be supplied and will result in a successful stage configuration.
        value = "Z"
        result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.z)
        self.assertIsValidUsd(stage)

        value = "Y"
        result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.y)
        self.assertIsValidUsd(stage)

        # To avoid confusion for Python clients that will often be passing the upAxis as a string we support lower case tokens.
        # The chance of confusion is high because `UsdGeom.Tokens.y` and `UsdGeom.Tokens.z` are actually uppercase strings under the hood.
        stage = Usd.Stage.CreateInMemory()

        value = "y"
        result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.y)
        self.assertIsValidUsd(stage)

        value = "z"
        result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageUpAxis(stage), UsdGeom.Tokens.z)
        self.assertIsValidUsd(stage)

        # Build a new identifier so that we can assert failures
        stage = Usd.Stage.CreateInMemory()

        # An "X" up axis is considered invalid and result in an unsuccessful stage configuration
        value = "X"
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(UsdGeom.StageHasAuthoredMetersPerUnit(stage))

        value = "x"
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            result = usdex.core.configureStage(stage, self.defaultPrimName, value, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(UsdGeom.StageHasAuthoredMetersPerUnit(stage))

    def testLinearUnits(self):
        # The linear units are required and must be a value greater than 0 otherwise the stage will not be configured
        stage = Usd.Stage.CreateInMemory()

        # An invalid value will result in an unsuccessful stage configuration
        value = 0.0
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(UsdGeom.StageHasAuthoredMetersPerUnit(stage))

        value = -5.0
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_WARNING_TYPE, ".*invalid stage metrics")]):
            result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertFalse(result)
        self.assertFalse(UsdGeom.StageHasAuthoredMetersPerUnit(stage))

        # A valid value will result in a successful stage configuration
        # The linear units will be reflected on the stage
        value = UsdGeom.LinearUnits.nanometers
        result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageMetersPerUnit(stage), value)
        self.assertTrue(UsdGeom.StageHasAuthoredMetersPerUnit(stage))
        self.assertIsValidUsd(stage)

        # It is valid to reuse an identifier.
        # The new linear units will be reflected on the stage
        value = UsdGeom.LinearUnits.meters
        result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, value, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertEqual(UsdGeom.GetStageMetersPerUnit(stage), value)
        self.assertTrue(UsdGeom.StageHasAuthoredMetersPerUnit(stage))
        self.assertIsValidUsd(stage)

    def testAuthoringMetadata(self):
        # The authoring metadata will be written
        stage = Usd.Stage.CreateInMemory()
        result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        self.assertTrue(result)
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertEqual(stage.GetRootLayer().customLayerData, {"creator": self.defaultAuthoringMetadata})
        self.assertIsValidUsd(stage)

        # The value will not be overwritten if not supplied
        result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits)
        self.assertTrue(result)
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertEqual(stage.GetRootLayer().customLayerData, {"creator": self.defaultAuthoringMetadata})
        self.assertIsValidUsd(stage)

        # The value will not be overwritten if explicitly supplied as None
        result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, authoringMetadata=None)
        self.assertTrue(result)
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertEqual(stage.GetRootLayer().customLayerData, {"creator": self.defaultAuthoringMetadata})
        self.assertIsValidUsd(stage)

        # the value is arbitrary
        stage.GetRootLayer().ClearCustomLayerData()
        self.assertEqual(stage.GetRootLayer().customLayerData, {})
        result = usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, "foo")
        self.assertTrue(result)
        self.assertTrue(usdex.core.hasLayerAuthoringMetadata(stage.GetRootLayer()))
        self.assertEqual(stage.GetRootLayer().customLayerData, {"creator": "foo"})
        self.assertIsValidUsd(stage)


class SaveStageTestCase(usdex.test.TestCase):

    def __createRefLayer(self):
        stage = Usd.Stage.Open(self.tmpLayer("ref"))
        UsdGeom.Xform.Define(stage, f"/{self.defaultPrimName}")
        stage.GetRootLayer().Save()
        return stage.GetRootLayer().identifier

    def __createBaseLayer(self):
        stage = Usd.Stage.Open(self.tmpLayer("base"))
        refLayer = self.__createRefLayer()
        xform = UsdGeom.Xform.Define(stage, f"/{self.defaultPrimName}")
        xform.AddTranslateOp().Set(Gf.Vec3f(1, 1, 1))
        UsdGeom.Xform.Define(stage, f"/{self.defaultPrimName}/cubeRoot")
        cube = UsdGeom.Cube.Define(stage, f"/{self.defaultPrimName}/cubeRoot/cube")
        cube.GetSizeAttr().Set(10)
        cube.AddTranslateOp().Set(Gf.Vec3f(1, 2, 3))
        prim = stage.DefinePrim(f"/{self.defaultPrimName}/refXform")
        prim.GetReferences().AddReference(assetPath=refLayer, primPath="/Root")
        stage.GetRootLayer().Save()
        return stage.GetRootLayer().identifier

    def __createOverLayer(self):
        stage = Usd.Stage.Open(self.tmpLayer("over"))
        prim = stage.OverridePrim(f"/{self.defaultPrimName}")
        UsdGeom.Xform(prim).AddTranslateOp().Set(Gf.Vec3f(2, 2, 2))
        prim = stage.OverridePrim(f"/{self.defaultPrimName}/cubeRoot/cube")
        cube = UsdGeom.Cube(prim)
        cube.AddTranslateOp().Set(Gf.Vec3f(1, 2, 3))
        stage.GetRootLayer().Save()
        return stage.GetRootLayer().identifier

    def __composeStage(self):
        stage = Usd.Stage.Open(self.tmpLayer("composed"))
        stage.GetRootLayer().subLayerPaths.insert(0, self.__createBaseLayer())
        stage.GetRootLayer().subLayerPaths.insert(0, self.__createOverLayer())
        usdex.core.configureStage(stage, self.defaultPrimName, self.defaultUpAxis, self.defaultLinearUnits, self.defaultAuthoringMetadata)
        prim = stage.DefinePrim(f"/{self.defaultPrimName}/refWorld")
        prim.GetReferences().AddReference(
            assetPath=stage.GetRootLayer().subLayerPaths[-1],  # base
            primPath=f"/{self.defaultPrimName}",
        )
        return stage

    def testSaveStage(self):
        comment = "test save stage comment"
        stage = self.__composeStage()
        rootLayer = stage.GetRootLayer()
        baseLayer = stage.GetLayerStack()[-1]
        overLayer = stage.GetLayerStack()[-2]
        root = stage.GetDefaultPrim()

        stage.SetEditTarget(Usd.EditTarget(rootLayer))  # Root Stage Layer
        stage.DefinePrim(f"{root.GetPath()}/another")
        stage.SetEditTarget(Usd.EditTarget(baseLayer))  # base
        stage.DefinePrim(f"{root.GetPath()}/another1")
        stage.SetEditTarget(Usd.EditTarget(overLayer))  # over
        stage.DefinePrim(f"{root.GetPath()}/another2")
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_STATUS_TYPE, "Saving.*")]):
            usdex.core.saveStage(stage, authoringMetadata=self.defaultAuthoringMetadata, comment=comment)
        for layer in stage.GetLayerStack():
            if not layer.anonymous:
                self.assertTrue(usdex.core.hasLayerAuthoringMetadata(layer))
                self.assertEqual(layer.customLayerData, {"creator": self.defaultAuthoringMetadata}, f"{layer.identifier} did not match")
                self.assertEqual(layer.comment, comment)

        # Test native stage save - make sure the comment has been cleared
        for layer in stage.GetLayerStack():
            layer.ClearCustomLayerData()
        stage.SetEditTarget(Usd.EditTarget(rootLayer))  # Root Stage Layer
        stage.DefinePrim(f"{root.GetPath()}/another3")
        stage.SetEditTarget(Usd.EditTarget(baseLayer))  # base
        stage.DefinePrim(f"{root.GetPath()}/another4")
        stage.SetEditTarget(Usd.EditTarget(overLayer))  # over
        stage.DefinePrim(f"{root.GetPath()}/another5")
        stage.Save()
        for layer in stage.GetLayerStack():
            self.assertFalse(layer.HasCustomLayerData())
            self.assertFalse(usdex.core.hasLayerAuthoringMetadata(layer))

        # Test Save without comments - make comments first
        stage.SetEditTarget(Usd.EditTarget(rootLayer))  # Root Stage Layer
        stage.DefinePrim(f"{root.GetPath()}/another6")
        stage.SetEditTarget(Usd.EditTarget(baseLayer))  # base
        stage.DefinePrim(f"{root.GetPath()}/another7")
        stage.SetEditTarget(Usd.EditTarget(overLayer))  # over
        stage.DefinePrim(f"{root.GetPath()}/another8")
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_STATUS_TYPE, "Saving.*")]):
            usdex.core.saveStage(stage, authoringMetadata=self.defaultAuthoringMetadata, comment=comment)
        for layer in stage.GetLayerStack():
            if not layer.anonymous:
                self.assertTrue(usdex.core.hasLayerAuthoringMetadata(layer))
                self.assertEqual(layer.customLayerData, {"creator": self.defaultAuthoringMetadata})
                self.assertEqual(layer.comment, comment)

        # Test no comment argument
        stage.SetEditTarget(Usd.EditTarget(rootLayer))  # Root Stage Layer
        stage.DefinePrim(f"{root.GetPath()}/another9")
        stage.SetEditTarget(Usd.EditTarget(baseLayer))  # base
        stage.DefinePrim(f"{root.GetPath()}/another10")
        stage.SetEditTarget(Usd.EditTarget(overLayer))  # over
        stage.DefinePrim(f"{root.GetPath()}/another11")
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_STATUS_TYPE, "Saving.*")]):
            usdex.core.saveStage(stage, authoringMetadata=self.defaultAuthoringMetadata)
        for layer in stage.GetLayerStack():
            if not layer.anonymous:
                self.assertTrue(usdex.core.hasLayerAuthoringMetadata(layer))
                self.assertEqual(layer.customLayerData, {"creator": self.defaultAuthoringMetadata})
                self.assertEqual(layer.comment, comment)  # retained from previous save

        # Test no authoringMetadata argument
        stage.SetEditTarget(Usd.EditTarget(rootLayer))  # Root Stage Layer
        stage.DefinePrim(f"{root.GetPath()}/another9")
        stage.SetEditTarget(Usd.EditTarget(baseLayer))  # base
        stage.DefinePrim(f"{root.GetPath()}/another10")
        stage.SetEditTarget(Usd.EditTarget(overLayer))  # over
        stage.DefinePrim(f"{root.GetPath()}/another11")
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_STATUS_TYPE, "Saving.*")]):
            usdex.core.saveStage(stage)
        for layer in stage.GetLayerStack():
            if not layer.anonymous:
                # retained from previous save
                self.assertTrue(usdex.core.hasLayerAuthoringMetadata(layer))
                self.assertEqual(layer.customLayerData, {"creator": self.defaultAuthoringMetadata})
                self.assertEqual(layer.comment, comment)

        # setting custom metadata using the same keys blocks the automated authoring metadata
        for layer in stage.GetLayerStack():
            layer.ClearCustomLayerData()
            layer.customLayerData = {"creator": "foo bar"}
        stage.SetEditTarget(Usd.EditTarget(rootLayer))  # Root Stage Layer
        stage.DefinePrim(f"{root.GetPath()}/another12")
        stage.SetEditTarget(Usd.EditTarget(baseLayer))  # base
        stage.DefinePrim(f"{root.GetPath()}/another13")
        stage.SetEditTarget(Usd.EditTarget(overLayer))  # over
        stage.DefinePrim(f"{root.GetPath()}/another14")
        with usdex.test.ScopedDiagnosticChecker(self, [(Tf.TF_DIAGNOSTIC_STATUS_TYPE, "Saving.*")]):
            usdex.core.saveStage(stage, authoringMetadata=self.defaultAuthoringMetadata)
        for layer in stage.GetLayerStack():
            if not layer.anonymous:
                self.assertTrue(usdex.core.hasLayerAuthoringMetadata(layer))
                self.assertEqual(layer.customLayerData, {"creator": "foo bar"})
                self.assertEqual(layer.comment, comment)

        # validate
        self.assertIsValidUsd(
            stage,
            issuePredicates=[
                # we defined many typeless prims in this test for convenience
                omni.asset_validator.IssuePredicates.ContainsMessage("Missing type for Prim"),
            ],
        )


class LocationEditableTestCase(usdex.test.TestCase):
    def testIsEditableLocationFromStagePath(self):
        stage = Usd.Stage.CreateInMemory()
        result, reason = usdex.core.isEditablePrimLocation(stage, "/Valid")
        self.assertTrue(result)
        self.assertEqual(reason, "")

        # paths must be absolute
        stage = Usd.Stage.CreateInMemory()
        path = "../relative"
        result, reason = usdex.core.isEditablePrimLocation(stage, path)
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*is not a valid absolute prim path")

        # paths must be prim paths
        path = "/absolute.property"
        result, reason = usdex.core.isEditablePrimLocation(stage, path)
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*is not a valid absolute prim path")

        # if the prim exists it cannot be an Instance Proxy
        stage.CreateClassPrim("/Prototypes")
        UsdGeom.Xform.Define(stage, "/Prototypes/Prototype")
        UsdGeom.Xform.Define(stage, "/Prototypes/Prototype/InstanceProxyChild")
        xformPrim = UsdGeom.Xform.Define(stage, "/World/Instance").GetPrim()
        xformPrim.GetReferences().AddInternalReference(Sdf.Path("/Prototypes/Prototype"))
        xformPrim.SetInstanceable(True)
        result, reason = usdex.core.isEditablePrimLocation(stage, f"{xformPrim.GetPath()}/InstanceProxyChild")
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*is an instance proxy, authoring is not allowed")

    def testIsEditableLocationFromPrimName(self):
        stage = Usd.Stage.CreateInMemory()
        validPrim = UsdGeom.Xform.Define(stage, "/Valid").GetPrim()
        result, reason = usdex.core.isEditablePrimLocation(validPrim, "child")
        self.assertTrue(result)
        self.assertEqual(reason, "")

        # an invalid prim fails
        invalidPrim = Usd.Prim()
        result, reason = usdex.core.isEditablePrimLocation(invalidPrim, "child")
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*Invalid UsdPrim")

        # the name must be a valid identifier
        result, reason = usdex.core.isEditablePrimLocation(validPrim, "1 2 3 !!!")
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*is not a valid prim name")

        # if the child exists it cannot be an Instance Proxy
        stage.CreateClassPrim("/Prototypes")
        UsdGeom.Xform.Define(stage, "/Prototypes/Prototype")
        UsdGeom.Xform.Define(stage, "/Prototypes/Prototype/InstanceProxyChild")
        xformPrim = UsdGeom.Xform.Define(stage, "/World/Instance").GetPrim()
        xformPrim.GetReferences().AddInternalReference(Sdf.Path("/Prototypes/Prototype"))
        xformPrim.SetInstanceable(True)
        result, reason = usdex.core.isEditablePrimLocation(xformPrim, "InstanceProxyChild")
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*is an instance proxy, authoring is not allowed")

        # the parent cannot be an Instance Proxy either
        instanceProxyChild = stage.GetPrimAtPath(f"{xformPrim.GetPath()}/InstanceProxyChild")
        result, reason = usdex.core.isEditablePrimLocation(instanceProxyChild, "grandchild")
        self.assertFalse(result)
        self.assertRegexpMatches(reason, ".*is an instance proxy, authoring is not allowed")
