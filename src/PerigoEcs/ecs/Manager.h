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
        Entity* e = new ecs::Entity();
        auto sharedEntity = std::shared_ptr<ecs::Entity >( e );
        setupEntity(sharedEntity);
        return sharedEntity;
    }
    
    
    ScopedEntity<Entity> createScopedEntity() {
        return ScopedEntity<Entity>(createEntity());
    }

    template<typename T, typename... Args>
    std::shared_ptr<T> createEntity(Args&&... args){
        
        T* e = new T(std::forward<Args>(args)... );
        std::shared_ptr<T> sharedEntity = std::shared_ptr<T>( e );
        
        setupEntity(sharedEntity);
        return sharedEntity;
    }

    template<typename T, typename... Args>
    ScopedEntity<T> createScopedEntity(Args&&... args){
        return ScopedEntity<T>( createEntity<T>( std::forward<Args>( args ) ...  ) );
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
        
        auto _id = getComponentTypeID< WrapperComponent<T> >();
        return  (std::vector< WrapperComponent<T>* >&) mComponentsByType[_id];
    }
    
    
    template <class ...Args>
    std::vector<std::shared_ptr<Entity>> getEntitiesWithComponents() const {
        
        std::bitset<MaxComponents> bitsetMask;
        setBitset( &bitsetMask, getComponentTypeID<Args>()... );
        std::vector<std::shared_ptr<Entity>> entities;
        for( auto &e : mEntityPool.mEntities ){
            
            if(auto shared = e ){
                bool b = ( shared->getComponentBitset() | bitsetMask  ) == shared->getComponentBitset(); // check if entity has all the bits in the bitset mask
                if( b ){
                    entities.push_back( shared );
                }
            }
         }
        
        return entities;
    };

    
    // EntityRef copyEntity( const EntityRef& iEntity ){
    //     return copyEntity(iEntity.get());
    // }
    
    
    std::vector<EntityRef> getEntities() {
        
        std::vector<EntityRef> output;
        for( auto &e : mEntityPool.mEntities ){
            
            if( auto shared = e ){
                output.push_back(shared);
            }
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
        
        void addEntityToPool( const EntityRef& e ) {
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
                if(e){
                    num++;
                }
            }
            return num;
        }
        
        
        void cleanup(){

            uint64_t index;
            
            for( auto e = mEntities.begin(); e != mEntities.end(); ){
                // if( *e == nullptr ){
                //     mEntities.erase(e);
                    
                // }else{
                //     ++e;
                //     ++index;
                // }
            }
        }


        std::vector< EntityRef > mEntities;
        std::array< std::vector<ComponentRef>, MaxComponents> mComponents;
        
        std::queue<uint64_t> idPool;
        bool fetchId(uint64_t* outputID);
        const int resizePool = 512;
    };

    EntityPool mEntityPool;
    
protected:
    
    //  ---- general manager vars -------
    std::array< std::vector<Component*>, MaxComponents> mComponentsByType;
    std::vector<SystemRef> mSystems;
    
    bool needsRefresh{false};
    bool isManagerInitialized = false;

    // setup entity after it's creation
    void setupEntity(const EntityRef& e ){

        e->mManager = this;
        
        mEntityPool.addEntityToPool( e ); // add a weak_ptr to pool
        mEntityPool.resizeComponentVector();
        
        e->setup();
        
        if( e->onLateSetup ){
            e->onLateSetup();
        }
        
    }
    static void entityDeleter( ecs::EntityRef e ){

        if( e->onDestroy ){
            e->onDestroy();
        }

        e->markRefresh();
        auto& pool = e->getManager()->mEntityPool;
        auto id = e->getId();
        pool.idPool.push(id);

        pool.mEntities[id] = nullptr;
        
        e.reset();
    }

    friend class Entity;
};
}
#endif //LEKSAPP_MANAGER_H
