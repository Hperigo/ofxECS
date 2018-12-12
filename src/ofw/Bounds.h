//
//  Bounds.h
//  SuffolkSlides
//
//  Created by Henrique on 12/4/18.
//

#ifndef Bounds_h
#define Bounds_h

#include "ofRectangle.h"
#include "ImGui.h"
#include "Component.h"
    
struct Bounds : public ecs::Component {
    
    Bounds() {
        
    }
    
    Bounds(const ofRectangle& rect ) : boundRect( rect ){
        
    }
    
    Bounds( float x, float y, float w, float h ) : boundRect( x,y,w,h ){
        
    }
    
    
    void drawUi() override  {
        
        ImGui::DragFloat2("x, y:", &boundRect.position.x );
        glm::vec2 size( boundRect.width, boundRect.height );
        if( ImGui::DragFloat2( "size: ", &size.x ) ){
            boundRect.setSize( size.x, size.y );
        }
    }
    
    ofRectangle boundRect;
};



#endif /* Bounds_h */

