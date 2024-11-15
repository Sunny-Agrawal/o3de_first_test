
#include <USDAZ/USDAZTypeIds.h>
#include <USDAZModuleInterface.h>
#include "USDAZSystemComponent.h"

namespace USDAZ
{
    class USDAZModule
        : public USDAZModuleInterface
    {
    public:
        AZ_RTTI(USDAZModule, USDAZModuleTypeId, USDAZModuleInterface);
        AZ_CLASS_ALLOCATOR(USDAZModule, AZ::SystemAllocator);
    };
}// namespace USDAZ

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), USDAZ::USDAZModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_USDAZ, USDAZ::USDAZModule)
#endif
