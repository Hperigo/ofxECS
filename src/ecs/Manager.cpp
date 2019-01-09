//
//  Manager.cpp
//  EcsCereal
//
//  Created by Henrique on 6/21/18.
//

#include <map>
#include "Manager.h"

using namespace ecs;
//std::map< std::string , std::shared_ptr<ecs::internal::ComponentFactoryInterface>> Manager::typeFactory = std::map< std::string, std::shared_ptr<ecs::internal::ComponentFactoryInterface > >();


bool Manager::getId(uint64_t* outputID){
    
    if( idPool.size() ){
        *outputID = idPool.top();
        idPool.pop();
        
        return true;
    }
    
    return false;
}
