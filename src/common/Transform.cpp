//
//  Transform.cpp
//  Created by Henrique on 10/10/17.
//

#include "Transform.hpp"
#include "ecs/Manager.h"

#include  <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

uint32_t Transform::transformId = 0;

Transform::Transform(){
    mId = transformId;
    transformId++;
}

Transform::Transform( const vec3& pos_ ) : localPos(pos_){
    
    mId = transformId;
    transformId++;
    
}

Transform::Transform( const Transform& other ){
    
    mId = transformId;
    transformId++;
    
    localPos = other.localPos;
    rotation = other.rotation;
    localScale = other.localScale;
    
    if( other.hasParent() ){
        this->setParent( other.getParent() );
    }
    updateMatrices();
}


void Transform::onDestroy() {
    
    // clean up children
    if( children.size() > 0 ){
        for( auto& c : children ){
            c->removeParent(false, false);
        }
    }
    
    if(hasParent()){
        removeParent(false, true);
    }
}

Transform::~Transform(){
    children.clear();
}


void Transform::updateMatrices(bool updateChildren){
    
    // TODO: Cache the matrices transformss
    /*if( !needsUpdate() ){
        return;
    }*/
    
    glm::mat4 transform;
    transform *= glm::translate<float>(mat4(), localPos + anchorPoint);
    transform *= glm::toMat4<float>( mRotation ); //glm::rotate(localRotation, vec3(0,0,1));
    transform *= glm::scale<float>(mat4(), localScale );
    transform *= glm::translate<float>(mat4(), -anchorPoint );
    
    mCTransform = transform;
    
    if(parent)
    {
        mWorldTransform = parent->getWorldTransform() * mCTransform;
    }else{
        mWorldTransform = mCTransform;
    }

    mNeedsUpdate = false;
}

// CTransformation Functions ------

void Transform::setTransform(const mat4 &transform){
    
    mCTransform = transform;
    if(parent)
    {
        mWorldTransform = parent->mWorldTransform * mCTransform;
    }else{
        mWorldTransform = mCTransform;
    }
    
    mNeedsUpdate = true;
}

void Transform::set(Transform &other){
    
    setPos( other.getPos() );
    setRotation( other.getRotation() );
    setScale( other.getScale() );
    setAnchorPoint( other.getAnchorPoint() );
    
}


// Position -------

glm::vec3 Transform::getPos() {
    
    if(needsUpdate()){
        updateMatrices();
    }
    return localPos;
    
}

vec3 Transform::getWorldPos() {
    
    if( needsUpdate() ){
        updateMatrices(false);
    }
    
    vec4 p = mWorldTransform * vec4(anchorPoint, 1);
    return vec3(p.x, p.y, p.z);
}


void Transform::setWorldPos(const vec3& pos){
    
    if(parent)
    {
        auto newP = glm::inverse(parent->mWorldTransform) * glm::vec4(pos, 1);
        localPos = newP;
    }else{
        localPos = pos;
    }
    
    localPos -= anchorPoint;
    
    mNeedsUpdate = true;
}

// Scale -------

void Transform::setWorldScale(const vec3& scale){
    
    if(parent)
    {
        vec3 invScale = (1.0f /  parent->getWorldScale() );
        localScale =  localScale *  invScale;
    }
    else
    {
        localScale = scale;
    }
    
    mNeedsUpdate = true;
}

vec3 Transform::getWorldScale() {
    
    if( needsUpdate() ){
        updateMatrices(false);
    }
    
    if(parent)
    {
        return parent->getWorldScale() * localScale;
    }else{
        return localScale;
    }
}

glm::vec3 Transform::getScale(){
    
    if(needsUpdate()){
        updateMatrices();
    }
    
    return localScale;
}
    

// Rotation -------
void Transform::setWorld2dRotation( float radians ){
    
    auto q = glm::angleAxis( radians, glm::vec3( 0, 0, 1 ) );
    
    if(parent)
    {
        
        auto invParent = glm::inverse( parent->getWorldRotation() );
        mRotation = q * invParent;
        
    }else{
        mRotation = q;
    }
    
    mNeedsUpdate = true;
}

void Transform::setWorldRotation(const glm::quat& q ){
    
    if(parent)
    {
        auto invParent = glm::inverse( parent->getWorldRotation() );
        mRotation = q * invParent;
        
    }else{
        mRotation = q;
    }
    
    mNeedsUpdate = true;
}

glm::quat Transform::getWorldRotation() {
    
    if( needsUpdate() ){
        updateMatrices(false);
    }
    
    
    if(parent)
    {
        return mRotation * parent->getWorldRotation();
    }else{
        return mRotation;
    }
}

void Transform::setParent( Transform* _parent, bool keepWorldCTransform)
{
    
    // ASSERT, you cannot asign this transform parent to it self
    assert( (_parent->getId() != mId ) );

    parent = _parent;
    
    if( keepWorldCTransform ){
        setWorldPos( localPos );
        setWorldScale(localScale);
        setWorldRotation(mRotation);
    }
    
    parent->addChildToList( this );
    
    mNeedsUpdate = true;
}


//
void Transform::removeParent(bool keepWordCTransform, bool removeFromList){
    
    cout << "will remove transform child: " << getId() << endl;

    if(removeFromList)
        parent->removeChildFromList(this);
    
    //TODO clenup
    auto p = parent;
    if(p && keepWordCTransform){
        
        auto newPos =  mWorldTransform * vec4(0,0, 0, 1);
        localPos = vec3( newPos.x, newPos.y, newPos.z );
        
        auto newScale =  localScale * p->getWorldScale();
        localScale = vec3( newScale.x, newScale.y, newScale.z );
        
        mRotation = mRotation * p->getWorldRotation();
        
        parent = nullptr;
    }
    
    parent = nullptr;
    mNeedsUpdate = true;
}

Transform* Transform::findChild(const Transform* child ){
    
    
    if( child == nullptr ){
        return nullptr;
    }
    
    auto iter = std::find_if(children.begin(), children.end(), [child](const Transform* handle) -> bool {
        return (handle != nullptr) && (handle == child);
    });
    
    if( iter == children.end() ){
        return nullptr;
    }
    
    return *iter;
}


bool Transform::hasChild(const Transform* child, bool recursive ){
    
    auto found = findChild(child);
    
    if( found ){
        return true;
    }
    
    if( recursive  ){
        for( auto& c : children  ){
            
            if(  c->hasChild( this ) ){
                return true;
            }
        }
    }
    
    return false;
}


bool Transform::removeChildFromList( Transform* child){
    cout << "will remove transform: " << getId() << ", child: " << child->getId() << endl;
    auto findIt = findChild(child);
    
    if( findIt ){
        auto rmFn = [child](const Transform* t ) -> bool{
            return child == t;
        };
        
        children.erase(  std::remove_if( children.begin(), children.end(), rmFn ), children.end() );
        return true;
    }
    return false;
}

bool Transform::addChildToList( Transform* child){
    
    auto findIt = findChild( child );
    
    if( ! findIt ){
        children.push_back(child);
        return true;
    }
    
    return false;
}

Transform* Transform::getRoot(){
    
    Transform* current = this ;
    while( current->hasParent()  ){
        current = current->getParent();
    }
    
    return current;
}


void Transform::descendTree(const std::function<bool (Transform& parent)> &fn){
    
    
    fn( (*this) ); // continues if function returns bool
    
    for( auto &c : children ) {
        c->descendTree(fn);
    }
    
}


/*
ecs::EntityRef ImGui::DrawTree(const Transform* iTransform){
    
    static int selection_mask = (1 << 0);
    int node_clicked = -1;
    
    ecs::EntityRef selectedEntity = nullptr;
    
    
    std::function<void(const Transform*)> drawChildren = [&](const Transform* root ){
        
        
        ui::PushID("id");
        
        auto rootId = root->getEntity().lock()->getId();
        auto id_text = "e id: " + std::to_string( rootId );
        
        auto nodeName = std::to_string(rootId).c_str();
        bool isSelected = ((selection_mask & (1 << rootId)));
        
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((isSelected == true) ? ImGuiTreeNodeFlags_Selected : 0 ) ;
        
        if( isSelected ){
            selectedEntity = root->getEntity().lock();
        }
        
        
        if( !root->isLeaf() ){
            
            bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)rootId, node_flags, nodeName, rootId);
            
            if(ImGui::IsItemClicked()){
                node_clicked = rootId;
            }
            
            if( nodeOpen ){
                for( auto &child : root->getChildren() ) {
                    auto ptr = child;
                    drawChildren( ptr );
                }
                ImGui::TreePop();
            }
            
        }else{
            ImGui::TreeNodeEx((void*)(intptr_t)rootId, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen , nodeName, rootId);
            
            if(ImGui::IsItemClicked()){
                node_clicked = rootId;
            }
        }
        
        
        ui::PopID();
    }; // end of lambda
    
    
    drawChildren( iTransform );
    
    if( node_clicked != -1 ){
        selection_mask = (1 << node_clicked);
    }
    
    return  selectedEntity;
}
*/

