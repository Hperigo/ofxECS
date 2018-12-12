//
// Created by Henrique on 5/23/18.
//

#ifndef LEKSAPP_ENTITY_H
#define LEKSAPP_ENTITY_H


#include <bitset>
#include <array>
#include <vector>
#include "Component.h"


#include <iostream>

namespace ecs{

    class Manager;
    class System;

    using EntityRef = std::shared_ptr<Entity>;

    namespace internal{
        struct EntityInfoBase{
            virtual void copyInto(const EntityRef& source, EntityRef& target);
        };
    }
    
    class Entity : public std::enable_shared_from_this<Entity> {

    public:

        Entity( ) {
            mEntityId = mNumOfEntities;
            mNumOfEntities += 1;
            mComponentArray.fill(nullptr);
        }

        virtual ~Entity(){
            markRefresh();
        }

        bool isAlive() const { return mIsAlive; }
        virtual  void destroy() {
            mIsAlive = false;
            markRefresh();
        };
        
        unsigned int getId() {
            return mEntityId;
        }
    
        virtual void setup() { };
        
        virtual void drawUi() { };

        
        template < typename T>
        bool hasComponent() const{
            return mComponentBitset[ getComponentTypeID<T>() ];
        }

        
        template <class T,
        typename std::enable_if< !std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent() {
           
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

            componentTypeID = getComponentTypeID<T>();  // we dont need a specialized function for wrapper components because getComponentTypeID already does that
    
            mComponentBitset.set(componentTypeID, 0);
            mComponentArray [ componentTypeID ]->mEntity = nullptr;/**/
            mComponentArray [ componentTypeID ] = nullptr;

            markRefresh();
        }

        
        template <class T,
        typename std::enable_if< !std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* getComponent(){
            
            assert(hasComponent< WrapperComponent<T> >());
            Component* comp = mComponentArray[getComponentTypeID< WrapperComponent<T> >()];
            WrapperComponent<T>* wrapper = static_cast< WrapperComponent<T>* >(  comp );
            
            return & (wrapper->object);
        }
        
        
        template <class T,
        typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* getComponent(){
            
            assert(hasComponent<T>());
            return (T*)mComponentArray[getComponentTypeID<T>()];
            
        }
        
        inline std::bitset<MaxComponents> getComponentBitset(){ return mComponentBitset; }
        
        
        //TODO: use bitset
        inline std::vector< Component* > getComponents(){
            std::vector< Component* > components;
            
            for( int i = 0; i < internal::lastID; i++ ){
                if( mComponentArray[i] != nullptr){
                    components.push_back( mComponentArray[i] );
                }
            }
            
            return components;
        }
        
        Manager* getManager() { return mManager; }
        
        std::shared_ptr<internal::EntityInfoBase> getFactory(){ return mFactory; };
        
        
        
        void setActive( bool active = true ){
            mIsActive = active;
        }
        
        bool isActive() const { return mIsActive; }
        
    protected:
        
        void addComponentToManager( ComponentID cId,  const ComponentRef& component );
        void markRefresh();
        
        std::shared_ptr<internal::EntityInfoBase> mFactory;

        Manager* mManager;
        bool mIsAlive{ true };
        bool mIsActive{ true };
        
        std::bitset<MaxComponents> mComponentBitset;
        std::array< Component* , MaxComponents> mComponentArray;
        
        static unsigned int mNumOfEntities;
        unsigned int mEntityId;
        
        friend class Manager;
        
      //use this to initialize components in the entity constructor
        std::function<void()> onSetup;
    };
    
    template<class T>
    struct EntityHelper :  public internal::EntityInfoBase{
        
        void copyInto( const EntityRef& source, EntityRef& target) override{
            target = std::make_shared<T>(  *std::static_pointer_cast<T>( source ) );
        }
    };
    
}

#endif //LEKSAPP_ENTITY_H
