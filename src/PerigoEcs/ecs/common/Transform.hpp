//
//  Transform.hpp
//  EcsTest
//
//  Created by Henrique on 11/30/18.
//

#ifndef Transform_hpp
#define Transform_hpp

#include "ecs/Component.h"
#include "ecs/System.h"

#include <functional>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x2.hpp>
#include <glm/gtc/quaternion.hpp>


class Transform : public ecs::Component, public std::enable_shared_from_this<Transform>{
    
public:
    Transform();
    Transform( const glm::vec3& pos_ );
    Transform( const Transform& other );
    ~Transform();
    
    void onDestroy() override;
    
    void setTransform(const glm::mat4& transform );
    
    glm::mat4 getLocalTransform() const { return mCTransform; }
    glm::mat4 getWorldTransform() {
        
        if( needsUpdate() ){
            updateMatrices();
        }
        return mWorldTransform;
    }
    
    //--- sets the lcation,rotation, anchor and scale based on another transform ( whithout setting it's parent )
    
    void set( Transform& other );
    
    // Position ------------------------------
    glm::vec3 getWorldPos();
    void setWorldPos(const glm::vec3& pos);
    
    glm::vec3 getPos();
    glm::vec3* getPosPtr(){ return &localPos; }
    void setPos(const glm::vec3& pos){ localPos = pos; mNeedsUpdate = true; }

    // anchor point -----
    
    void setAnchorPoint(const glm::vec3& p ){  anchorPoint = p; mNeedsUpdate = true; }
    glm::vec3 getAnchorPoint() const { return anchorPoint; }
    glm::vec3* getAnchorPointPtr() { return &anchorPoint; }
    
    // Scale --------------------------------
    
    void setWorldScale(const glm::vec3& scale );
    glm::vec3 getWorldScale();
    
    void setScale(const glm::vec3& scale ){ localScale = scale; mNeedsUpdate = true; }
    void setScaleUniform( float s ) { setScale( glm::vec3(s,s,s) ); mNeedsUpdate = true; }
    
    glm::vec3* getScalePtr(){  return &localScale; }
    glm::vec3 getScale();


    // Rotation --------------------------------
    
    void setWorld2dRotation( float radians );
    void setWorldRotation(const glm::quat& q );
    
    
    void set2dRotation( float radians ){
        mRotation = glm::angleAxis( radians, glm::vec3( 0, 0, 1 ) );
        mNeedsUpdate = true;
    }
    
    void setRotation(const glm::quat& rotation ){ mRotation = rotation;  mNeedsUpdate = true; }
    
    
    glm::quat getRotation() {
        
        if(needsUpdate()){
            updateMatrices();
        }
        
        return mRotation;
        
    }
    float get2dRotation()  {
        
        if(needsUpdate()){
            updateMatrices();
        }
        
        return glm::eulerAngles(mRotation).z;
    }
    glm::quat getWorldRotation();
    glm::quat* getRotationPtr(){  return &mRotation; }
    
    float getWorldRotationRadians()  { return glm::eulerAngles( getWorldRotation() ).z; }
    
    // Parenting ----
    void setParent( Transform* _parent, bool keepWordCTransform = false );
    void removeParent(bool keepWorldCTransform = true, bool removeFromList = true);
    
    Transform* getParent() const { return parent; }
    
    std::vector<Transform*> getChildren() const { return children; }
    
    
    bool needsUpdate() {
        bool needs = mNeedsUpdate;//  || mAlwaysUpdate);
        return needs;
    }
    
//    bool addChild( Transform* transform );
//    bool removeChild( Transform* transform );
    
    // todo: rename to containChild
    bool hasChild(const Transform* child,  bool recursive = true );
    bool hasParent() const { return (parent != nullptr); }
    
    Transform* getRoot();
    
    bool isRoot()const { return ( !hasParent() ) ? true : false; }
    bool isLeaf()const { return (children.size() == 0) ? true : false; }
    
    bool removeChildFromList(Transform* child);
    bool addChildToList(Transform* child);
    
    Transform* findChild(const Transform* child );
    
    /// Visit all of this components' descendents depth-first. TODO: separate visitor patterns from data.
    void descendTree(const std::function<bool (Transform& t)> &fn);
    
    
    size_t getId() const { return mId; }
    void setId(uint32_t i){ mId = i; }
    
    void updateMatrices(bool updateChildren = true);
    
    void setAlwaysUpdate( bool v ){  mAlwaysUpdate = v; }
    bool getAlwaysUpdate(){ return mAlwaysUpdate; }
    
protected:
    
    bool mNeedsUpdate = true;
    bool mAlwaysUpdate = true; /* buggy */

    
    glm::vec3 localPos;
    glm::vec3 anchorPoint;
    
    glm::vec3 rotation;
    glm::vec3 localScale{1.0f, 1.0f, 1.0f};
    
    glm::quat mRotation;
    
    glm::mat4 mCTransform;
    glm::mat4 mWorldTransform;
    
    std::vector<Transform*> children;
    Transform* parent = nullptr;
    
    
    static uint32_t transformId;
    uint32_t mId = 0;
};



/*
template <>
struct ecs::ComponentFactoryTemplate<Transform> : public ecs::ComponentFactory<Transform> {
    
    ComponentFactoryTemplate(){
        //        ComponentFactory();
    }
    
    
    void load(void* archiver) override {
        
        glm::JsonTree& tree = *static_cast<glm::JsonTree*>( archiver );
        
        glm::vec3 pos;
        pos.x = tree.getChild("pos").getChild(0).getValue<float>();
        pos.y = tree.getChild("pos").getChild(1).getValue<float>();
        pos.z = tree.getChild("pos").getChild(2).getValue<float>();
        
        
        glm::vec3 anchorPoint;
        anchorPoint.x = tree.getChild("anchor").getChild(0).getValue<float>();
        anchorPoint.y = tree.getChild("anchor").getChild(1).getValue<float>();
        anchorPoint.z = tree.getChild("anchor").getChild(2).getValue<float>();
        
        glm::vec3 scale;
        scale.x = tree.getChild("scale").getChild(0).getValue<float>();
        scale.y = tree.getChild("scale").getChild(1).getValue<float>();
        scale.z = tree.getChild("scale").getChild(2).getValue<float>();
        
        
        float r;
        r = tree.getChild("rotation").getChild(0).getValue<float>();
        owner->setPos( pos );
        owner->setAnchorPoint(anchorPoint);
        owner->setScale( scale );
        owner->setRotation( r );
    }
    
    void save(void* archiver) override {
        
        glm::JsonTree* tree = static_cast<glm::JsonTree*>( archiver );
        
        auto tJson = glm::JsonTree::makeArray( std::to_string(_id) );
        
        auto pJson = glm::JsonTree::makeArray("pos");
        pJson.addChild( glm::JsonTree("",  owner->getPos().x) );
        pJson.addChild( glm::JsonTree("",  owner->getPos().y) );
        pJson.addChild( glm::JsonTree("",  owner->getPos().z) );
        
        auto aJson = glm::JsonTree::makeArray("anchor");
        aJson.addChild( glm::JsonTree("",  owner->getAnchorPoint().x) );
        aJson.addChild( glm::JsonTree("",  owner->getAnchorPoint().y) );
        aJson.addChild( glm::JsonTree("",  owner->getAnchorPoint().z) );
        
        auto sJson = glm::JsonTree::makeArray("scale");
        sJson.addChild( glm::JsonTree("",  owner->getScale().x) );
        sJson.addChild( glm::JsonTree("",  owner->getScale().y) );
        sJson.addChild( glm::JsonTree("",  owner->getScale().z) );
        
        auto rJson = glm::JsonTree::makeArray("rotation");
        rJson.addChild( glm::JsonTree("", owner->getRotationRadians()) );
        
        
        tJson.addChild( pJson );
        tJson.addChild( aJson );
        tJson.addChild( rJson );
        tJson.addChild( sJson );
        
        tree->addChild( tJson );
    }
};

//template<>
//inline Transform ecs::ComponentFactory<Transform>::object = Transform();
namespace ImGui{
    
    inline void DrawTransform2D( Transform* t ){
        
        
        ui::PushID( "t" );
        ui::DragFloat3( " position",    &(*t->getPosPtr())[0]  );
        ui::DragFloat3( " scale",       &(*t->getScalePtr())[0], 0.01f  );
        ui::DragFloat3( " anchor pont", &(*t->getAnchorPointPtr())[0], 0.01f  );
        
        auto radians = t->getRotationRadians();
        if(  ui::DragFloat( "rotation radians", &radians, 0.01f ) ){
            t->setRotation( radians );
        }
        
        t->updateMatrices();
        
        ui::PopID();
    }
    
    ecs::EntityRef DrawTree(const Transform* root );
    
}*/

#endif /* Transform_hpp */
