//
//  TextureEntity.h
//  SuffolkSlides
//
//  Created by Henrique on 12/10/18.
//

#ifndef TextureEntity_h
#define TextureEntity_h

#include "Drawables.hpp"
#include "Entity.h"

#include "Transform.hpp"
#include "ofTexture.h"

#include "ImGui.h"

class TextureEntity : public ecs::Entity, public ecs::IDrawable {
    
public:
    ~TextureEntity(){
        
    }
    TextureEntity(){
        
    }
    
    TextureEntity(const std::string& path ){
        
        this->onSetup = [&, path]{
            
            addComponent<ofTexture>();
            addComponent<Transform>();
            loadTexture( path );
        };
    }
    
    
    void setup() override  {

    }
    
    
    void setTexture(const ofTexture& texture ){
        *getComponent<ofTexture>() = texture;
    }
    
    void loadTexture(const std::string& path ){
    
        ofTexture tex;
        ofLoadImage( tex , path);
        setTexture( tex );
        
        imagePath = path;
        
    }
    
    
    void draw() override {
        
        if( hasComponent<ofTexture>() ){
        
            ofPushMatrix();
            ofSetMatrixMode(OF_MATRIX_MODELVIEW);
            ofMultMatrix( getComponent<Transform>()->getWorldTransform() );
            
            getComponent<ofTexture>()->draw(0,0);
            
            ofPopMatrix();
        }
        
    }
    
    
    void drawUi() override {
        
        ImGui::Text("image path: %s", imagePath.c_str() );
        
        if( ImGui::Button("refresh image") ){
            loadTexture( imagePath );
        }
        
    }
private:
    std::string imagePath = "";
};

#endif /* TextureEntity_h */
