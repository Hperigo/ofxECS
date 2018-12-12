//
//  Drawables.cpp
//  SuffolkSlides
//
//  Created by Henrique on 12/4/18.
//

#include "Drawables.hpp"

using namespace std;
using namespace ecs;
ecs::DrawSystem* ecs::DrawSystem::mInstance = nullptr;
uint32_t DrawTarget::id_count = 0;

DrawSystem* ecs::DrawSystem::getInstance() {
    if (mInstance == nullptr) {
        static DrawSystem mS;
        mInstance = &mS;
    }
    
    return mInstance;
}


IDrawable::IDrawable() {
    
    setDrawTarget(DrawSystem::getInstance()->getDefaultDrawTarget());
}

IDrawable::IDrawable( DrawTarget* iDrawTarget ){

    iDrawTarget->addDrawable( this );
    
}

void IDrawable::setDrawTarget( std::shared_ptr<ecs::DrawTarget> iDrawTarget){
    
    if( iDrawTarget ){
        
        if( drawTargetOwner )
            drawTargetOwner->removeDrawable( this );
        
        iDrawTarget->addDrawable(this);
        
    }else{ //if draw target is null, remove it from owner
        
        if( hasDrawTarget() == false ){
            return;
        }
        
        // is draw target is null, remove from current target
        drawTargetOwner->removeDrawable(this);
        drawTargetOwner = nullptr;
    }
    
}


IDrawable::~IDrawable(){
    
    if( hasDrawTarget() ){
        drawTargetOwner->removeDrawable( this );
    }
}

