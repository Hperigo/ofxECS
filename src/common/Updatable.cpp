//
//  Updatable.cpp
//  SuffolkSlides
//
//  Created by Henrique on 12/5/18.
//

#include "Updatable.hpp"

using namespace std;
using namespace ecs;
ecs::UpdateSystem* ecs::UpdateSystem::mInstance = nullptr;
uint32_t UpdateTarget::id_count = 0;

UpdateSystem* ecs::UpdateSystem::getInstance() {
    if (mInstance == nullptr) {
        static UpdateSystem mS;
        mInstance = &mS;
    }
    
    return mInstance;
}


IUpdatable::IUpdatable() {
    auto d = UpdateSystem::getInstance()->getDefaultUpdateTarget();
    
    setUpdateTarget(d);
}

IUpdatable::IUpdatable( UpdateTarget* iUpdateTarget ){
    
    iUpdateTarget->addUpdatable( this );
    
}

void IUpdatable::setUpdateTarget( std::shared_ptr<ecs::UpdateTarget>& iUpdateTarget){
    
    if( iUpdateTarget ){
        
        if( UpdateTargetOwner )
            UpdateTargetOwner->removeUpdatable( this );
        
        iUpdateTarget->addUpdatable(this);
        
    }else{ //if draw target is null, remove it from owner
        
        if( hasUpdateTarget() == false ){
            return;
        }
        
        // is draw target is null, remove from current target
        UpdateTargetOwner->removeUpdatable(this);
        UpdateTargetOwner = nullptr;
    }
    
}


IUpdatable::~IUpdatable(){
    
    if( hasUpdateTarget() ){
        UpdateTargetOwner->removeUpdatable( this );
    }
}
