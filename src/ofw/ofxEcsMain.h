//
//  ofxEcsMain.h
//  SuffolkSlides
//
//  Created by Henrique on 12/10/18.
//

#ifndef ofxEcsMain_h
#define ofxEcsMain_h

#include "Manager.h"

#include "Updatable.hpp"
#include "Drawables.hpp"


typedef  std::shared_ptr<class ofxEcsManager> ofxEcsManagerRef;
class ofxEcsManager : public ecs::Manager {
public:
    static ofxEcsManagerRef create() {
        return std::make_shared<ofxEcsManager>();
    }
    
    void setup() {
        
        ecs::Manager::setup();
        
        mEntityUpdator = ecs::UpdateSystem::getInstance();
        mEntityRender = ecs::DrawSystem::getInstance();
    
    }
    
    void update() {

        ecs::Manager::update();
        mEntityUpdator->update();
        
    }
    
    
    void draw() {
        ecs::Manager::draw();
        mEntityRender->draw();
    }
    
    
    ecs::DrawSystem* getDefaultDrawSystem(){
        return mEntityRender;
    }
    
    ecs::UpdateSystem* getDefaultUpdateSystem(){
        return mEntityUpdator;
    }
    
    
    ecs::DrawSystem* mEntityRender;
    ecs::UpdateSystem* mEntityUpdator;
};


#endif /* ofxEcsMain_h */
