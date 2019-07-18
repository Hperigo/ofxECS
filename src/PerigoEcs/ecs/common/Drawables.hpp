//
//  Drawables.hpp
//  SuffolkSlides
//
//  Created by Henrique on 12/4/18.
//

#ifndef Drawables_hpp
#define Drawables_hpp

#include <stdio.h>

#include <memory>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <set>

#include <iostream>

/*
@Notes:
 make a separate draw target
 
*/

#include "System.h"
#include "Entity.h"

class Transform;


namespace ecs {

    struct Updatable : public ecs::Component {
        std::function<void()> mFn;
        
        bool doUpdate = true;
    };
    
    struct UpdateSystem : public ecs::System {
        void update() override;
        
        bool doUpdate = true;
        size_t getNumOfUpdateCalls() { return numOfUpdateCalls; }
        
        static void setTreeUpdatable( Transform* t, bool d );
        
    protected:
        size_t numOfUpdateCalls = 0;
    };
    
    
struct IDrawable {
    
    IDrawable(){
        
    }
    
    virtual void setup(){};
    

    // push states, fbos. glScissors.. etc..
    virtual void preDraw() {
        
    }
    
    // draw itself and children
    virtual void draw(){
        
    }
    
    // pop states, unbind fbos etc..
    virtual void postDraw() {
        
    }
    
    
    void setDrawable( bool b ) { mIsDrawable = b; }
    bool isDrawable() { return mIsDrawable; }

    static void setTreeVisible( Transform* t, bool d );
    
protected:
    bool mIsDrawable = true;
};

struct DrawSystem2D : public ecs::System{
    
    struct DefaultDrawLayer : public ecs::Component { };
    
    void setup () override {
        
    }
    
    void update() override;
    void draw() override;
    
    
    // Helper functions --
    
    size_t getNumOfDrawCalls(){ return numOfDrawCalls; };
    
protected:
    void drawEntity(Entity* t, uint32_t depth  );
    std::list<EntityRef> roots;
    
    size_t numOfDrawCalls = 0;
};

}// endof namespace

#endif /* Drawables_hpp */
