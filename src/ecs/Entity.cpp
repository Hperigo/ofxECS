//
// Created by Henrique on 5/24/18.
//

#include "Entity.h"
#include "Manager.h"

#include <iostream>

using namespace ecs;



unsigned int Entity::mNumOfEntities = 0;

void Entity::addComponentToManager( ComponentID cId, const ComponentRef& component){

    mManager->addComponent( cId, component );

    mComponentArray[cId] = component.get();
    mComponentBitset[cId] = true;

    component->mEntity = this;
    component->mManager = mManager;
    component->mComponentId = cId;
    component->setup();

}

void Entity::markRefresh(){
    mManager->needsRefresh = true;
}




void internal::EntityInfoBase::copyInto(const EntityRef& source, EntityRef& target){
    
    target = std::make_shared<Entity>( *source );
}

