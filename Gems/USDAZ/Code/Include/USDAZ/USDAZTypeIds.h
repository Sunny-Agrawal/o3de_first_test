
#pragma once

namespace USDAZ
{
    // System Component TypeIds
    inline constexpr const char* USDAZSystemComponentTypeId = "{B6B3C42D-FCE7-4EEA-8A40-B6DD6CEEBD77}";
    inline constexpr const char* USDAZEditorSystemComponentTypeId = "{35A937F8-CA06-4AEA-9DD0-4D75A19D4609}";

    // Module derived classes TypeIds
    inline constexpr const char* USDAZModuleInterfaceTypeId = "{1246DFFC-1A8B-40BE-A12C-96D5269B47DA}";
    inline constexpr const char* USDAZModuleTypeId = "{A9280D4D-97DC-4A0C-B35A-0713D99A6C8D}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* USDAZEditorModuleTypeId = USDAZModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* USDAZRequestsTypeId = "{2DB5B3D4-E542-4FC3-BFCE-26D667AB3E47}";
} // namespace USDAZ
