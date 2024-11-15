
#include <USDAZ/USDAZTypeIds.h>
#include <USDAZModuleInterface.h>
#include "USDAZEditorSystemComponent.h"

namespace USDAZ
{
    class USDAZEditorModule
        : public USDAZModuleInterface
    {
    public:
        AZ_RTTI(USDAZEditorModule, USDAZEditorModuleTypeId, USDAZModuleInterface);
        AZ_CLASS_ALLOCATOR(USDAZEditorModule, AZ::SystemAllocator);

        USDAZEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                USDAZEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<USDAZEditorSystemComponent>(),
            };
        }
    };
}// namespace USDAZ

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), USDAZ::USDAZEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_USDAZ_Editor, USDAZ::USDAZEditorModule)
#endif
