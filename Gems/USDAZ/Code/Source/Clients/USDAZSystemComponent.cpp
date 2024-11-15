
#include "USDAZSystemComponent.h"

#include <USDAZ/USDAZTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace USDAZ
{
    AZ_COMPONENT_IMPL(USDAZSystemComponent, "USDAZSystemComponent",
        USDAZSystemComponentTypeId);

    void USDAZSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<USDAZSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void USDAZSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("USDAZService"));
    }

    void USDAZSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("USDAZService"));
    }

    void USDAZSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void USDAZSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    USDAZSystemComponent::USDAZSystemComponent()
    {
        if (USDAZInterface::Get() == nullptr)
        {
            USDAZInterface::Register(this);
        }
    }

    USDAZSystemComponent::~USDAZSystemComponent()
    {
        if (USDAZInterface::Get() == this)
        {
            USDAZInterface::Unregister(this);
        }
    }

    void USDAZSystemComponent::Init()
    {
    }

    void USDAZSystemComponent::Activate()
    {
        USDAZRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void USDAZSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        USDAZRequestBus::Handler::BusDisconnect();
    }

    void USDAZSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace USDAZ
