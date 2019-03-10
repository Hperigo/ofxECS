//
// Created by Henrique on 5/23/18.
//

#ifndef LEKSAPP_ENTITY_H
#define LEKSAPP_ENTITY_H


#include <bitset>
#include <array>
#include <vector>
#include <functional>
#include <assert.h>  
#include "Component.h"


#include <iostream>

namespace ecs{

    class Manager;
    class System;

    using EntityRef = std::shared_ptr<Entity>;

    namespace internal{
        struct EntityInfoBase{
            virtual void copyInto(const Entity* source, EntityRef& target);
        };
    }
    
    class Entity : public std::enable_shared_from_this<Entity> {

    public:

        Entity() {
            mNumOfEntities += 1;
        }

        virtual ~Entity(){
            if(isAlive()){
                markRefresh();
            }
        }

        EntityRef duplicate();
        
        bool isAlive() const { return mIsAlive; }
        virtual  void destroy() {
            mIsAlive = false;
            markRefresh();
        };

        virtual void setup() { }
        virtual void drawUi() { };

        
        std::string getName(){ return mName; }
        void setName(const std::string& name ){ mName = name; }
        
        unsigned int getId() {
            return mEntityId;
        }
        
        template < typename T>
        bool hasComponent() const{
            return mComponentBitset[ getComponentTypeID<T>() ];
        }

        
        template <class T,
        typename std::enable_if< !std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent() {
            assert(!hasComponent<T>());
            std::shared_ptr<WrapperComponent<T>> rawComponent( new WrapperComponent<T>( T() ) );
            
            auto cId = getComponentTypeID<WrapperComponent<T>>();
            
            auto rawHelper = std::make_shared< ComponentFactoryTemplate< WrapperComponent<T> > >();
            
            rawHelper->owner = rawComponent.get();
            rawComponent->mFactory = rawHelper;
            
            addComponentToManager(cId, rawComponent);
            return getComponent< T >();
            
        }
        
        template <class T,
        typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent() {
            
            //entity already has component!
            assert(!hasComponent<T>());
            
            std::shared_ptr<T> rawComponent( new T() );
            
            auto cId = getComponentTypeID<T>();
            
            auto rawHelper = std::make_shared< ComponentFactoryTemplate<T> >();
            rawHelper->owner = rawComponent.get();
            rawComponent->mFactory = rawHelper;
            
            addComponentToManager(cId, rawComponent);
            
            return  rawComponent.get();
            
        }
        
        
        template <class T, typename... TArgs,
        typename std::enable_if< ! std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent(TArgs&&... _Args) {
            
            std::shared_ptr<WrapperComponent<T>> rawComponent( new WrapperComponent<T>( std::forward<TArgs>(_Args)... ) );
            auto cId = getComponentTypeID<WrapperComponent<T>>();
            auto rawHelper = std::make_shared< ComponentFactoryTemplate< WrapperComponent<T> > >();
            rawHelper->owner = rawComponent.get();
            rawComponent->mFactory = rawHelper;
            
            addComponentToManager(cId, rawComponent);
            
            return  ( T* )rawComponent.get();
        }
        
        template <class T, typename... TArgs,
        typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent(TArgs&&... _Args) {
            
            std::shared_ptr<T> rawComponent( new T(std::forward<TArgs>(_Args)... ));
            
            auto cId = getComponentTypeID<T>();
            
            auto rawHelper = std::make_shared< ComponentFactoryTemplate<T> >();
            rawHelper->owner = rawComponent.get();
            rawComponent->mFactory = rawHelper;
            
            addComponentToManager(cId, rawComponent);
            
            return  rawComponent.get();
            
        }
     
        void addComponent( ComponentRef& rawComponent ){
            addComponentToManager(rawComponent->getFactory()->_id, rawComponent);
        }
        
        template<typename T>
        void removeComponent(){

            ComponentID componentTypeID;
            
            // we dont need a specialized function for wrapper components because getComponentTypeID already does that
            componentTypeID = getComponentTypeID<T>();
            mComponentBitset.set(componentTypeID, 0);
            
            markRefresh();
        }

        
        template <class T,
        typename std::enable_if< ! std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* getComponent(){
            
            assert(hasComponent< WrapperComponent<T> >());
            Component* comp =   getComponentFromManager( getComponentTypeID< WrapperComponent<T> >() );
            WrapperComponent<T>* wrapper = static_cast< WrapperComponent<T>* >(  comp );
            
            return & (wrapper->object);
        }
        
        
        template <class T,
        typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* getComponent(){

            if( hasComponent<T>() )
                return (T*)getComponentFromManager( getComponentTypeID<T>() );
            else
                return nullptr;
            
        }
        
        inline std::bitset<MaxComponents> getComponentBitset(){ return mComponentBitset; }
        
        
        //TODO: use bitset
        inline std::vector< Component* > getComponents(){
            std::vector< Component* > components;
            
            for( int i = 0; i < internal::lastID + 1; i++ ){
                
                if( mComponentBitset[i] == true ){
                    auto c = getComponentFromManager( i );
                    assert(c != nullptr); // this components should not be null, if so, why is the bitset true?
                    components.push_back( c );
                }

            }
            
            return components;
        }
        
        Manager* getManager() { return mManager; }
        
        std::shared_ptr<internal::EntityInfoBase> getFactory() const { return mFactory; };

        void setActive( bool active = true ){
            mIsActive = active;
        }
        
        bool isActive() const { return mIsActive; }
        
        std::function<void()> onDestroy;
        
    protected:
        
        ecs::Component* getComponentFromManager(ComponentID cId) const;
        void addComponentToManager( ComponentID cId,  const ComponentRef& component );
        void markRefresh();
        
        std::shared_ptr<internal::EntityInfoBase> mFactory;

        Manager* mManager;
        bool mIsAlive{ true };
        bool mIsActive{ true };
        
        std::bitset<MaxComponents> mComponentBitset;
        
        static unsigned int mNumOfEntities;
        unsigned int mEntityId;
        
        std::string mName;
        
        friend class Manager;
        
      //use this to initialize components in the entity constructor
        std::function<void()> onLateSetup;
    };
    
    template<class T>
    struct EntityHelper :  public internal::EntityInfoBase{
        
        void copyInto( const Entity* source, EntityRef& target) override{
            auto t = (T*)( source );
            std::shared_ptr<T> newOne = std::make_shared<T>( *t );
            target = newOne;
        }
    
    };
    
}

#endif //LEKSAPP_ENTITY_H
