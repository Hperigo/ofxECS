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
        
        mFbo.allocate(2400, 1380, GL_RGBA);
        
    }
    
    void draw() override  {
        
        mFbo.begin();
        
        ofBackground( ofColor::black );
        
        ecs::DrawTarget::draw();
        
        mFbo.end();
        
    }

    
    ofFbo mFbo;
};


#endif /* DrawTargetFbo_h */
