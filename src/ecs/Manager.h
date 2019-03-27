//
// Created by Henrique on 5/24/18.
//

#ifndef LEKSAPP_MANAGER_H
#define LEKSAPP_MANAGER_H


#include "Entity.h"
#include "System.h"

#include <vector>
#include <queue>
#include <array>


#include <iostream>
using namespace std;

namespace  ecs{
    using ManagerRef = std::shared_ptr<class Manager>;

class Manager {


public:
    Manager(){
    
    }
    
    ~Manager(){ }

    template<typename... Args>
    static ManagerRef create(Args&&... args){
        return std::make_shared<Manager>( std::forward<Args>(args)...  );
    }

    EntityRef createEntity(){
        
        EntityRef e = std::make_shared<Entity>();
        e->mManager = this;
        e->mFactory = std::make_shared< internal::EntityInfoBase >();
        
        mEntityPool.addEntityToPool(e);
        mEntityPool.resizeComponentVector();

        e->setup();
        
        if( e->onLateSetup ){
            e->onLateSetup();
        }
        
        return e;
    }
    
    template<typename T, typename... Args>
    std::shared_ptr<T> createEntity(Args&&... args){

        std::shared_ptr<T> e = std::make_shared<T>( std::forward<Args>(args)...  );
        e->mManager = this;
        e->mFactory = std::make_shared< EntityHelper<T> >();
        
        mEntityPool.addEntityToPool(e);

        e->setup();
        
        if( e->onLateSetup ){
            e->onLateSetup();
        }
        
        return e;
    }

    template<typename T, typename... TArgs>
    std::shared_ptr<T> createSystem(TArgs&&... _Args) {

        std::shared_ptr<T> rawSystem( new T(std::forward<TArgs>(_Args)... ));

        SystemRef systemPtr{ rawSystem };
        rawSystem->mManager = this;
        
        mSystems.push_back( rawSystem );
        
        if( isManagerInitialized == true ){
            rawSystem->setup();
        }
        
        return  rawSystem;
    }

    void removeSystem(SystemRef iSystem){
        
        auto sys = std::find( mSystems.begin(), mSystems.end(), iSystem );

        if( sys != mSystems.end() ){
            mSystems.erase(sys);
        }
    }

    void setup();
    void update();
    void draw();
    
    void refresh();

    void addComponent(uint64_t entityId, ComponentID id, const ComponentRef component){

        auto& componentVector = mEntityPool.mComponents[id];
        auto& componentVectorByType = mComponentsByType[id];
        
        if( entityId >= componentVector.size() ){
            
            mEntityPool.resizeComponentVector();
            
            
            assert( componentVector.size() );
            componentVector[entityId] = component;
            componentVectorByType.push_back( component.get() );
            
        }else{
            
            componentVector[entityId] = component;
            componentVectorByType.push_back( component.get() );

        }

    }

    template<class T>
    void setBitset(std::bitset<MaxComponents>* bitset, T head)const {
        bitset->set( head, 1 );
    }


    template <class T, class ...Args>
    void  setBitset(std::bitset<MaxComponents>* bitset, T head ,  Args ... args) const {
        setBitset( bitset, head );
        setBitset( bitset, args ... );
    }
    
    template <class T,
    typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
    const std::vector<T*>& getComponentsArray() {
        
        if( needsRefresh ){
            refresh();
        }
        
        auto _id = getComponentTypeID<T>();
        return  (std::vector<T*>&) mComponentsByType[_id];
    }
    
    template <class T,
    typename std::enable_if< ! std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
    const std::vector< WrapperComponent<T>* >& getComponentsArray() {
        
        if( needsRefresh ){
            refresh();
        }
        
        auto _id = getComponentTypeID<T>();
        return  (std::vector< WrapperComponent<T>* >&) mComponentsByType[_id];
    }
    
    
    template <class ...Args>
    std::vector<std::shared_ptr<Entity>> getEntitiesWithComponents() const {
        
        std::bitset<MaxComponents> bitsetMask;
        setBitset( &bitsetMask, getComponentTypeID<Args>()... );
        std::vector<std::shared_ptr<Entity>> entities;
        for( auto &e : mEntityPool.mEntities ){
            
            if( e == nullptr ){
                continue;
            }
            
            bool b = ( e->getComponentBitset() | bitsetMask  ) == e->getComponentBitset(); // check if entity has all the bits in the bitset mask
            if( b ){
                 entities.push_back( e );
             }
            
         }
        
        return entities;
    };
    
    EntityRef copyEntity( const Entity* iEntity ){
        EntityRef e;
        iEntity->getFactory()->copyInto( iEntity, e );
        
        mEntityPool.addEntityToPool(e);
        
        for(size_t i = 0; i < e->mComponentBitset.size(); ++i){
            
            if(  e->mComponentBitset[i] == true ){
                
                auto sourceComponent = iEntity->getComponentFromManager( i );
                assert( sourceComponent != nullptr );
                
                ComponentRef targetComponent;
                
                sourceComponent->getFactory()->copyInto( sourceComponent, targetComponent );
                targetComponent->mEntity = e.get();
                
                addComponent( e->getId(), i, targetComponent );

            }
        }
        
       // printCheck();
        
        return e;
    }
    
    EntityRef copyEntity( const EntityRef& iEntity ){
        return copyEntity(iEntity.get());
    }
    
    
    std::vector<EntityRef> getEntities() {
        
        std::vector<EntityRef> output;
        for( auto &e : mEntityPool.mEntities ){
            
            if( e == nullptr ){
                continue;
            }
            output.push_back(e);
        }

        return output;
    }
    std::vector<SystemRef>& getSystems() { return mSystems; }
    
    
    void printCheck();
    
    struct EntityPool {

        EntityPool(){
            
        }
        EntityPool duplicate();
        void setPool(const EntityPool& otherPool );
        
        void addEntityToPool( EntityRef e ) {
            // grabs an empty ( available id) from pool, if that's not available create a new space
            uint64_t eId = -1;
            if( fetchId( &eId ) ){
                
                e->mEntityId = eId;
                mEntities[eId] = e;

            }else{
                
                e->mEntityId = mEntities.size();
                mEntities.emplace_back(e);
            }
        }
        
        void resizeEntityBuffer(){
            
            // resize entity vector and later the components vector
            mEntities.resize(resizePool);
            
            // add empty id's to pool
            for(int i = 0; i < resizePool; i++){
                idPool.push(i);
            }
            
            resizeComponentVector();
        }
        
        void resizeComponentVector(){
            
            for(int i = 0; i < MaxComponents; i++){
                mComponents[i].resize( mEntities.size() );
            }
        }
        
        uint32_t getNumOfActiveEntities(){
            uint32_t num = 0;
            for(auto e : mEntities){
                if(e != nullptr){
                    num++;
                }
            }
            return num;
        }
        
        
        void cleanup(){
            
            
            
            uint64_t index;
            
            for( auto e = mEntities.begin(); e != mEntities.end(); ){
                if( *e == nullptr ){
                    mEntities.erase(e);
                    
                }else{
                    ++e;
                    ++index;
                }
            }
        }
        
        std::vector<EntityRef> mEntities;
        std::array< std::vector<ComponentRef>, MaxComponents> mComponents;
        
        std::queue<uint64_t> idPool;
        bool fetchId(uint64_t* outputID);
        const int resizePool = 512;
    };

    EntityPool mEntityPool;
    
protected:

    // ---- entity QUEUE functions -----
    //@TODO: maybe add an specilized pool object?
    
    
    //  ---- general manager vars -------
    
    std::array< std::vector<Component*>, MaxComponents> mComponentsByType;
    std::vector<SystemRef> mSystems;
    
    bool needsRefresh{false};
    bool isManagerInitialized = false;

    //we use this to cast a whole vector at once, only possible with a raw pointer
    // TODO: make this the main array, not a copy, by using `new` and `delete`

    friend class Entity;
};
}
#endif //LEKSAPP_MANAGER_H
