
#pragma once

#include <USDAZ/USDAZTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace USDAZ
{
    class USDAZRequests
    {
    public:
        AZ_RTTI(USDAZRequests, USDAZRequestsTypeId);
        virtual ~USDAZRequests() = default;
        // Put your public methods here
    };

    class USDAZBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using USDAZRequestBus = AZ::EBus<USDAZRequests, USDAZBusTraits>;
    using USDAZInterface = AZ::Interface<USDAZRequests>;

} // namespace USDAZ
