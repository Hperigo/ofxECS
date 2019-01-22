//
//  TextEntity.h
//  SuffolkApp
//
//  Created by Henrique on 1/21/19.
//

#ifndef TextEntity_h
#define TextEntity_h

#include "Drawables.hpp"
#include "Entity.h"

#include "Transform.hpp"

#include "ofTrueTypeFont.h"


struct TextEntity : public ecs::Entity, ecs::IDrawable {
    
    TextEntity(){
        
    }
    
    TextEntity(const ofTrueTypeFont& font) : mFont(font){
        
    }
    
    TextEntity(const ofTrueTypeFont& font, const std::string& text  ) : mFont(font), mText(text){
        
        
    }
    
    
    void setup() override {
        
        addComponent<Transform>();
    }
    
    
    void draw() override {

        if( auto t = getComponent<Transform>() ){
            
            ofPushMatrix();
            ofSetMatrixMode(OF_MATRIX_MODELVIEW);
            
            ofMultMatrix( getComponent<Transform>()->getWorldTransform() );
            
            mFont.drawString( mText, 100, 100 );
            
            ofPopMatrix();
        }
    }
    
    
    ofTrueTypeFont mFont;
    std::string  mText;
};

#endif /* TextEntity_h */
