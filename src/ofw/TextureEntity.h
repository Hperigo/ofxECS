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

#include "AlphaComponent.h"
#include "SuffolkSettings.h"
#include "ImGui.h"

class TextureEntity : public ecs::Entity, public ecs::IDrawable {
    
public:
    
    ~TextureEntity(){
        drawTargetOwner->removeDrawable( this );
    }
    
    TextureEntity(){
        
    }
    
    TextureEntity(const std::string& path ){
        
        this->onLateSetup = [&, path]{
            addComponent<ofImage>();
            loadTexture( path );
        };
    }
    
    
    void setup() override  {
        // add the transform here... and not on the callback
        addComponent<AlphaComponent>();
        addComponent<Transform>();
    }
    
    
    void setTexture(const ofImage& img ){
        
        if( !hasComponent<ofImage>() ){
            addComponent<ofImage>();
        }
        
        *getComponent<ofImage>() = img;
    }
    
    void loadTexture(const std::string& path ){
        
        ofImage img;
        img.load( path );
        imagePath = path;
        setTexture(img);
    }
    
    
    void releaseImage(){
        getComponent<ofImage>()->clear();
    }
    
    
    void draw() override {
        
        if( !isAlive() ){
            drawTargetOwner->removeDrawable( this );
            return;
        }
        
        if( hasComponent<ofImage>() && hasComponent<Transform>()  ){
            
            ofPushMatrix();
            ofSetMatrixMode(OF_MATRIX_MODELVIEW);
            
            ofMultMatrix( getComponent<Transform>()->getWorldTransform() );
            ofSetColor( 255, getComponent<AlphaComponent>()->alpha );
            ofEnableBlendMode( OF_BLENDMODE_ALPHA );
            
            auto texture = getComponent<ofImage>();
            
            ofRectangle targetScreenRect( 0, 0, sf::screenSize.x, sf::screenSize.y );
            ofRectangle r2(0,0, texture->getWidth(), texture->getHeight() );
            r2.scaleTo(targetScreenRect, ofAspectRatioMode::OF_ASPECT_RATIO_KEEP_BY_EXPANDING);
            
            
            getComponent<ofImage>()->draw(0,0,screenSize.x, screenSize.y);
            
            ofSetColor( 255, 255 );
            ofPopMatrix();
            
        }
        
    }
    
    std::string getPath() { return imagePath; }
    
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
