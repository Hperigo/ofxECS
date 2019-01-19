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

    mComponentBitset[cId] = true;

    component->mEntity = this;
    component->mManager = mManager;
    component->mComponentId = cId;
    component->setup();

}

ecs::Component* Entity::getComponentFromManager(ComponentID cId) const {
    
    auto& pool = mManager->mEntityPool;
    auto component = pool.mComponents[cId][mEntityId].get();
    return component;
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


