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
    class Entity : public std::enable_shared_from_this<Entity> {

    public:

        Entity() {
            mNumOfEntities += 1;
        }

        virtual ~Entity(){ }

        virtual EntityRef clone();
        
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
            return  hasComponentBitset( getComponentTypeID<T>() ) ;
        }

        bool hasComponentBitset( const size_t i ) const {
             return  mComponentBitset[ i ];
        }
        
        template <class T,
        typename std::enable_if< !std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent() {
            
            //entity already has component!
            assert(!hasComponent<T>());
            std::shared_ptr<WrapperComponent<T>> rawComponent( new WrapperComponent<T>( T() ) );
            
            auto cId = getComponentTypeID<T>();

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
            addComponentToManager(cId, rawComponent);

            return  rawComponent.get();
            
        }
        
        //wrapperComponent
        template <class T, typename... TArgs,
        typename std::enable_if< ! std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent(TArgs&&... _Args) {
            
            
            std::shared_ptr<WrapperComponent<T>> rawComponent( new WrapperComponent<T>( std::forward<TArgs>(_Args)... ) );
            auto cId = getComponentTypeID<T>(); //@NOTE: getComponentTypeID<WrapperComponent<T>>();
            addComponentToManager(cId, rawComponent);
            
            return  ( T* )rawComponent.get();
        }
        
        template <class T, typename... TArgs,
        typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        T* addComponent(TArgs&&... _Args) {
            
            std::shared_ptr<T> rawComponent( new T(std::forward<TArgs>(_Args)... ));
            
            auto cId = getComponentTypeID<T>();
            addComponentToManager(cId, rawComponent);
            
            return  rawComponent.get();
            
        }
    
        
        
        
        // normal component
        template <class T,
        typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        void removeComponent(){

            auto componentTypeID = getComponentTypeID<T>();            
            removeComponentWithId(componentTypeID);

        }

        // wrapper component
        template <class T,
        typename std::enable_if< ! std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
        void removeComponent(){

            auto componentTypeID = getComponentTypeID< WrapperComponent<T>>();
            std::cout << "removed from wrapper: " << componentTypeID << std::endl;
            removeComponentWithId(componentTypeID);
        }

        // actually remove the component
        void removeComponentWithId( const ComponentID& componentTypeID ){
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
            assert( hasComponent<T>() );
            if( hasComponent<T>() )
                return (T*)getComponentFromManager( getComponentTypeID<T>() );
            else
                return nullptr;
        }
        
        inline std::bitset<MaxComponents> getComponentBitset(){ return mComponentBitset; }


        inline std::vector< Component* > getComponents(){
            std::vector< Component* > components;
            
            for( int i = 0; i < internal::getLastID() + 1; i++ ){
                if( mComponentBitset[i] == true ){
                    auto c = getComponentFromManager( i );
                    assert(c != nullptr); // this components should not be null, if so, why is the bitset true?
                    components.push_back( c );
                }

            }
            
            return components;
        }
        
        Manager* getManager() { return mManager; }

        std::function<void()> onDestroy;
        
    protected:
        
        ecs::Component* getComponentFromManager(ComponentID cId) const;
        void addComponentToManager( ComponentID cId,  const ComponentRef& component );
        void markRefresh();

        Manager* mManager;
        bool mIsAlive{ true };
        
        std::bitset<MaxComponents> mComponentBitset;
        
        static unsigned int mNumOfEntities;
        unsigned int mEntityId;
        
        std::string mName;
        
        friend class Manager;
        
        //use this to initialize components in the entity constructor
        std::function<void()> onLateSetup;
    };
    
    
    
    // Holds a EntityRef and destroys it when this object leaves it's scope
    template <typename T>
    class ScopedEntity {
    public:
        ScopedEntity( EntityRef e ) : entity( e )
        { }
        
        ~ScopedEntity(){
            entity->destroy();
        }
        
        T* operator ->(){
            return (T*)( entity.get() );
        }
        
        EntityRef getPtr(){
            return entity;
        }
        
    private:
        EntityRef entity;
    };
}

#endif //LEKSAPP_ENTITY_H
