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
        *outputID = idPool.front();
        idPool.pop();
        return true;
    }
    
    return false;
}


void Manager::setup(){
    
    if( needsRefresh == true ){
        refresh();
    }
    
    for(auto& sys  : mSystems){
        sys->setup();
    }
    
    update();
    
}

void Manager::update(){
    refresh();
    
    for(auto& sys  : mSystems){
        
        if( sys->updatable ){
            sys->update();
        }
    }
}


void Manager::refresh(){
    
    if( !needsRefresh ){
        return;
    }
    
    for( std::size_t i = 0; i < mComponents.size(); ++i ){
        
        auto& componentVector(mComponents[i]);
        
        // erase components
        int j = 0;
        for( auto cIt = componentVector.begin(); cIt != componentVector.end(); ++cIt) {
            
            if( (*cIt) == nullptr ){
                continue;
            }
            
            auto e = (*cIt)->getEntity();
            assert( e != nullptr );
            
            if( !e->isAlive() ){
                (*cIt)->onDestroy();
                //cIt = componentVector.erase(cIt);
                (*cIt).reset();
            }
            
            j++;
        }
        
        mComponentsByType[i].clear();
        for(auto cp :  componentVector){
            if(cp != nullptr){
                mComponentsByType[i].push_back( cp.get() );
            }
        }
    }
    
    for( auto eIt = mEntities.begin(); eIt != mEntities.end(); ++eIt){
        
        if( *eIt == nullptr ){
            continue;
        }
        
        if( ! (*eIt)->isAlive() )
        {
            idPool.push((*eIt)->getId());
            (*eIt).reset();
            //                eIt = mEntities.erase( eIt );
        }
    }
    needsRefresh = false;
    
}

void Manager::draw(){
    
    
    for(auto& sys  : mSystems){
        if( sys->drawable ){
            sys->draw();
        }
    }
}
