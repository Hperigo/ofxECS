//
//  Drawables.cpp
//  SuffolkSlides
//
//  Created by Henrique on 12/4/18.
//

#include "Drawables.hpp"

#include "Transform.hpp"
#include "Manager.h"

using namespace std;
using namespace ecs;

void UpdateSystem::update() {
    
    if( doUpdate == false ){
        return;
    }
    numOfUpdateCalls = 0;
    auto updatables = getManager()->getComponentsArray<Updatable>();
    for( auto& u : updatables  ){
        
        if( u->mFn && u->doUpdate ){
            u->mFn();
            numOfUpdateCalls += 1;
        }
    }
}

void UpdateSystem::setTreeUpdatable( Transform* t, bool d ){
    
    auto fn = [&, d](Transform& t) -> bool {
        auto e = t.getEntity();
        if ( e->hasComponent<Updatable>() ) {
            e->getComponent<Updatable>()->doUpdate = d;
        }
        
        return true;
    };
    
    // do on the root;
    fn( *t );
    t->descendTree(fn);
}


void IDrawable::setTreeVisible( Transform* t, bool d ){
    
    
    auto fn = [&, d](Transform& t) -> bool {
        
        auto e = t.getEntity();
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


void DrawSystem2D::update() {

    roots.clear();
    auto entities = mManager->getEntitiesWithComponents<Transform>();
    for( auto& e : entities ){
        
        if( e->getComponent<Transform>()->isRoot() ){
            roots.push_back(e);
        }
    }
}

void DrawSystem2D::draw() {
    numOfDrawCalls = 0;
    for(auto& e :  roots  ){
        drawEntity( e.get(), 0 );
    }
}


void DrawSystem2D::drawEntity(Entity* e, uint32_t depth ){

    auto d = dynamic_cast<ecs::IDrawable*>( e );

    if(d != nullptr ) {
        bool shouldDraw = d->isDrawable(); // && e->hasComponent<DrawSystem2D::DefaultDrawLayer>();
        if( shouldDraw ){
            
            d->preDraw();
            d->draw();
            
            numOfDrawCalls++;
        }
    }

    if( e->hasComponent<Transform>() ){
        for( auto c : e->getComponent<Transform>()->getChildren() ){
            drawEntity(c->getEntity(), depth + 1);
        }
    }

    if( d != nullptr && d->isDrawable() ){
        d->postDraw();
    }

}
