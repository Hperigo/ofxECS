//
//  DrawTargetFbo.h
//  SuffolkSlides
//
//  Created by Henrique on 12/6/18.
//

#ifndef DrawTargetFbo_h
#define DrawTargetFbo_h

#include "Drawables.hpp"
#include "ofGraphics.h"
#include "ofFbo.h"


class ofxDrawTargetFbo : public ecs::DrawTarget {
    
public:
    ofxDrawTargetFbo(){
        
        mFbo.allocate(1920, 1080, GL_RGBA);
        
    }
    
    void draw() override  {
        
        mFbo.begin();
        
        ofBackground( ofColor::paleVioletRed );
        
        ecs::DrawTarget::draw();
        
        mFbo.end();
        
    }

    
    ofFbo mFbo;
};


#endif /* DrawTargetFbo_h */
