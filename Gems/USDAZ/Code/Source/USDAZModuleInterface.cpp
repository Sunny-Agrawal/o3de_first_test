
#include "USDAZModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <USDAZ/USDAZTypeIds.h>

#include <Clients/USDAZSystemComponent.h>

namespace USDAZ
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(USDAZModuleInterface,
        "USDAZModuleInterface", USDAZModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(USDAZModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(USDAZModuleInterface, AZ::SystemAllocator);

    USDAZModuleInterface::USDAZModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            USDAZSystemComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList USDAZModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<USDAZSystemComponent>(),
        };
    }
} // namespace USDAZ
