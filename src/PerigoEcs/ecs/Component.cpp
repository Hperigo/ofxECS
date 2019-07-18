#include "Component.h"

namespace ecs {
    
    static ComponentID lastID{0};
    
    namespace internal{
        
        ComponentID getLastID(){
            return lastID;
        }
        
        ComponentID getUniqueComponentID(){
                return lastID++;
        }
        
    }
}
