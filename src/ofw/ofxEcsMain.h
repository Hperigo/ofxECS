//
//  ofxEcsMain.h
//  SuffolkSlides
//
//  Created by Henrique on 12/10/18.
//

#ifndef ofxEcsMain_h
#define ofxEcsMain_h

#include "Manager.h"

#include "Drawables.hpp"
#include "TransformSystem.h"


typedef  std::shared_ptr<class ofxEcsManager> ofxEcsManagerRef;
class ofxEcsManager : public ecs::Manager {
public:
    static ofxEcsManagerRef create() {
        return std::make_shared<ofxEcsManager>();
    }
    
    ofxEcsManager() {
        ecs::Manager();
        
//        mEntityRender = ecs::DrawSystem::getInstance();
    }
    
    void setup() {
        
        mTransformSys = createSystem<TransformSystem>().get();
        mEntityUpdator = createSystem<ecs::UpdateSystem>();
        mEntityRender = createSystem<ecs::DrawSystem2D>();
        

        ecs::Manager::setup();
    }
    
    void update() {

        ecs::Manager::update();
//        mEntityRender->update();
        
    }
    
    
    void draw() {
        ecs::Manager::draw();
//        mEntityRender->draw();
    }
    
    
    std::shared_ptr<ecs::DrawSystem2D> getDefaultDrawSystem(){
        return mEntityRender;
    }

    std::shared_ptr<ecs::UpdateSystem> getDefaultUpdateSystem(){
        return mEntityUpdator;
    }
    
    std::shared_ptr<ecs::DrawSystem2D> mEntityRender;
    std::shared_ptr<ecs::UpdateSystem> mEntityUpdator;
    TransformSystem* mTransformSys;
};


#endif /* ofxEcsMain_h */
