//
//  Drawables.cpp
//  SuffolkSlides
//
//  Created by Henrique on 12/4/18.
//

#include "Drawables.hpp"
#include "Transform.hpp"
#include "Entity.h"

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


void IDrawable::setTree( Transform* t, bool d ){
    
    
    auto fn = [&, d](Transform& t) -> bool {
        
        
        auto e = t.getEntity();
//        cout << "setting drawable for: " << e->getId() << " to: " << d << endl;
        auto drawable = dynamic_cast<ecs::IDrawable*>(e);
        // if cast fails, transform entity does not have a IDrawable interface
        if (drawable) {
            drawable->setDrawable(d);
        }
        
        return true;
    };
    
    // do on the root;
    fn( *t );
    t->descendTree(fn);
    
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

void IDrawable::drawAtTop(){
    
    if( drawTargetOwner ){
        drawTargetOwner->removeDrawable(this);
        drawTargetOwner->addDrawable(this);
    }
}

IDrawable::~IDrawable(){

    if( hasDrawTarget() ){
        drawTargetOwner->removeDrawable( this );
    }
}



void DrawTarget::swapDrawablesOrder(IDrawable* a, IDrawable* b){
    
    auto aIt = std::find(mDrawables.begin(), mDrawables.end(), a);
    auto bIt = std::find(mDrawables.begin(), mDrawables.end(), b);
    
    std::swap(aIt, bIt);
    
}
