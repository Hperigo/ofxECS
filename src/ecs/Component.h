//
// Created by Henrique on 5/23/18.
//

#include <memory>
#include <bitset>

#ifndef ECSSAPP_COMPONENT_H
#define ECSSAPP_COMPONENT_H

namespace ecs{

    // forward decls...
    class Component;

    class Entity;
    using EntityRef = std::shared_ptr<Entity>;
    using EntityHandle = std::weak_ptr<Entity>;
    class Manager;

    using ComponentID = std::size_t;
    using ComponentRef = std::shared_ptr<Component>;

    constexpr std::size_t MaxComponents{100};
    using ComponentBitset = std::bitset<MaxComponents>;
    
    namespace internal{

        //@TODO: maybe move this to std:: typeinfo?
        static ComponentID lastID{0};
        inline ComponentID getUniqueComponentID() noexcept {
            return lastID++;
        }
    
        struct ComponentFactoryInterface : public std::enable_shared_from_this<ComponentFactoryInterface> {
            virtual void copyInto(const Component* source, ComponentRef& target){};
            virtual void load(void* archiver){};
            virtual void save(void* archiver){};
            virtual ComponentRef create() = 0;
            
            ComponentID _id;
        };
        
        template <typename T>
        inline ComponentID getComponentTypeID() noexcept {
            
            static ComponentID typeID { internal::getUniqueComponentID() };
            return typeID;
            
        }
    }

    struct Component {

    public:
        virtual ~Component() { }
        virtual void setup() { };
        virtual void drawUi() { };
        virtual void onDestroy(){ };


        Entity* getEntity(){ return mEntity; }
        Entity* getEntity() const { return mEntity; }
        Manager* getManager(){ return mManager; }


        std::shared_ptr<internal::ComponentFactoryInterface> getFactory() const {
            return mFactory;
        }
        void setFactory(const std::shared_ptr<internal::ComponentFactoryInterface>& iFactory ){ mFactory = iFactory; }
    protected:
    
        std::shared_ptr<internal::ComponentFactoryInterface> mFactory;
        
        Entity* mEntity;
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
    
    
    // we need to store T as a class child of ComponentFactoryInterface, so it can be stored into the component
    template<class T>
    struct ComponentFactory :  public internal::ComponentFactoryInterface{
        
            ComponentFactory() {
                owner = &object;
                _id = getComponentTypeID<T>();
            }
        
        
        void copyInto(const Component* source, ComponentRef& target) override{
            T* sourceObj = (T*)source;
            
            target.reset();
            target = std::make_shared<T>( *((T*)source) );
            target->setFactory( shared_from_this() ) ;
        }
            
        
        ComponentRef create() override;
            
            
        void save(void* archiver) override{ }
            
        void load(void* archiver) override { }
        
        T* owner;
        static T object;
    };
    
    template<typename T>
    T ecs::ComponentFactory<T>::object = T();

    
    template<class T>
    struct ComponentFactoryTemplate : public ComponentFactory<T>{

    };
    
    template<class T>
    ComponentRef ComponentFactory<T>::create(){
        auto t = std::make_shared<T>();
        
        auto helper = std::make_shared<ComponentFactoryTemplate<T> >();
        helper->owner = t.get();
        t->setFactory(helper);
        
        return t;
    }
    
}//end of namespace



#endif //ECSSAPP_COMPONENT_H
