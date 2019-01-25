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
        auto c = addComponent<ofColor>();
        *c = ofColor::white;
        
    }
    
    
    void draw() override {

        if( auto t = getComponent<Transform>() ){
            
            glm::vec2 offset;
            if( alignRight ){
                offset.x = -mFont.getStringBoundingBox(mText, 0,0).width;
            }
            
            
            ofPushStyle();
            
            auto transformMat = getComponent<Transform>()->getWorldTransform();
            
            ofSetColor( *getComponent<ofColor>() );
            
            ofPushMatrix();
            ofSetMatrixMode(OF_MATRIX_MODELVIEW);
            ofMultMatrix( transformMat );
            

            mFont.drawString( mText, offset.x, offset.y );
            ofPopMatrix();
            ofPopStyle();
        }
    }
    
    
    
    bool alignRight = false;
    ofTrueTypeFont mFont;
    std::string  mText;
};

#endif /* TextEntity_h */
