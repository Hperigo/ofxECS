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
#include "ofGraphics.h"
#include "ofImage.h"


#include "SuffolkSettings.h"
#include "ImGui.h"

class TextureEntity : public ecs::Entity, public ecs::IDrawable {
    
public:
    ~TextureEntity(){
        
    }
    TextureEntity(){
        
    }
    
    TextureEntity(const std::string& path ){
        
        this->onLateSetup = [&, path]{
            addComponent<ofTexture>();
            loadTexture( path );
        };
    }
    
    
    void setup() override  {
        // add the transform here... and not on the callback
        addComponent<Transform>();
    }
    
    
    void setTexture(const ofTexture& texture ){
        
        if( !hasComponent<ofTexture>() ){
            addComponent<ofTexture>();
        }
        
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
            auto screenSize = sf::collums.screenSize;
            getComponent<ofTexture>()->draw(0,0,screenSize.x, screenSize.y);
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
