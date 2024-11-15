
#include <AzCore/Serialization/SerializeContext.h>
#include "USDAZEditorSystemComponent.h"

#include <USDAZ/USDAZTypeIds.h>

namespace USDAZ
{
    AZ_COMPONENT_IMPL(USDAZEditorSystemComponent, "USDAZEditorSystemComponent",
        USDAZEditorSystemComponentTypeId, BaseSystemComponent);

    void USDAZEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<USDAZEditorSystemComponent, USDAZSystemComponent>()
                ->Version(0);
        }
    }

    USDAZEditorSystemComponent::USDAZEditorSystemComponent() = default;

    USDAZEditorSystemComponent::~USDAZEditorSystemComponent() = default;

    void USDAZEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("USDAZEditorService"));
    }

    void USDAZEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("USDAZEditorService"));
    }

    void USDAZEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void USDAZEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void USDAZEditorSystemComponent::Activate()
    {
        USDAZSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void USDAZEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        USDAZSystemComponent::Deactivate();
    }

} // namespace USDAZ
