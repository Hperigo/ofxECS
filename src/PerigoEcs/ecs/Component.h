//
// Created by Henrique on 5/23/18.
//

#include <memory>
#include <bitset>
#include <iostream>       // std::cout

#ifndef ECSSAPP_COMPONENT_H
#define ECSSAPP_COMPONENT_H

#ifndef ECS_MAX_COMPONENTS
    #define ECS_MAX_COMPONENTS 200
#endif

namespace ecs{

    // forward decls...
    class Component;

    class Entity;
    using EntityRef = std::shared_ptr<Entity>;
    class Manager;

    using ComponentID = std::size_t;
    using EntityID = std::size_t;
    
    using ComponentRef = std::shared_ptr<Component>;
    constexpr std::size_t MaxComponents{ECS_MAX_COMPONENTS};
    using ComponentBitset = std::bitset<MaxComponents>;
    
    namespace internal{

        //@TODO: maybe move this to std:: typeinfo?
        
        ComponentID getLastID();
        ComponentID getUniqueComponentID();
            
        template <typename T>
        ComponentID getComponentTypeID() noexcept {
            
            static ComponentID typeID { internal::getUniqueComponentID() };
//            std::cout << typeid(T).name() << "  " << typeID << std::endl;
            return typeID;
            
        }
    }

    struct Component {

    public:
        virtual ~Component() { }
        virtual void setup() { };
        virtual void drawUi() { };
        virtual void onDestroy(){ };
        virtual Component clone(){ };

        Entity* getEntity(){ return mEntity; }
        Entity* getEntity() const { return mEntity; }
        Manager* getManager(){ return mManager; }

    protected:
            
        Entity* mEntity;
        uint64_t mEntityId;
        Manager* mManager;
    
        std::size_t mComponentId;

        friend class Entity;
        friend class Manager;
    };
    

    // object used to wrap other classes outside ecs ( vec2, color, etc.. )
    template<typename T>
    struct WrapperComponent : public Component{
        
        WrapperComponent(){
        }
        WrapperComponent(const T& input ) : object(input) {
            
        }

        T& getRef() { return object; }

        //TODO: make private
        T object;
    };
    
    
    template <class T,
    typename std::enable_if< !std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
    inline ComponentID getComponentTypeID(){
        return internal::getComponentTypeID< WrapperComponent<T> >();
    }
    
    template <class T,
    typename std::enable_if< std::is_base_of<ecs::Component, T>::value, T>::type* = nullptr>
    inline ComponentID getComponentTypeID(){
            return internal::getComponentTypeID<T>();
    }

}//end of namespace



#endif //ECSSAPP_COMPONENT_H
