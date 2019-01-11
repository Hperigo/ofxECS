//
// Created by Henrique on 5/24/18.
//

#include "Entity.h"
#include "Manager.h"

#include <iostream>

using namespace ecs;



unsigned int Entity::mNumOfEntities = 0;

void Entity::addComponentToManager( ComponentID cId, const ComponentRef& component){

    mManager->addComponent(getId(), cId, component );

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

EntityRef Entity::duplicate(){
    return mManager->copyEntity(this);
}

void internal::EntityInfoBase::copyInto(const Entity* source, EntityRef& target){
    target = std::make_shared<Entity>( *source );
}


