
#pragma once

#include <AzToolsFramework/API/ToolsApplicationAPI.h>

#include <Clients/USDAZSystemComponent.h>

namespace USDAZ
{
    /// System component for USDAZ editor
    class USDAZEditorSystemComponent
        : public USDAZSystemComponent
        , protected AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = USDAZSystemComponent;
    public:
        AZ_COMPONENT_DECL(USDAZEditorSystemComponent);

        static void Reflect(AZ::ReflectContext* context);

        USDAZEditorSystemComponent();
        ~USDAZEditorSystemComponent();

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    };
} // namespace USDAZ
