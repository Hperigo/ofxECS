//
//  Transform.h
//
//  Created by Henrique on 8/21/17.
//
//

#ifndef TransformSystem_h
#define TransformSystem_h

#include "ecs/Manager.h"
#include "Transform.hpp"
#include "Drawables.hpp"

struct TransformSystem : ecs::System{
    
    void update() override{
        
        auto& transforms = getManager()->getComponentsArray<Transform>();
        for(auto& t : transforms){
            // TODO: only update leaf nodes or do some type of cache
            t->updateMatrices();
        }
    }
    
    
    void drawTransforms(){
        
        auto transforms = getManager()->getComponentsArray<Transform>();
        
        
        for( auto& t : transforms) {
            
            
//            if( auto d = dynamic_cast<ecs::IDrawable*>( t->getEntity() ) ){
//                if( ! d->isDrawable() ){
//                    return;
//                }
//            }
            
            auto mat = t->getWorldTransform();
            
            ofPushMatrix();
            ofSetMatrixMode(OF_MATRIX_MODELVIEW);
            ofMultMatrix( mat );

            ofSetColor( ofColor::red );
            ofNoFill();
            ofDrawRectangle( -5, -5, 10, 10 );
            ofSetColor( ofColor::white );
            ofDrawBitmapString( std::to_string( t->getId() ) , 0, 0);
            ofFill();
            ofPopMatrix();
            
        }
        
    }
        
        /*
        auto transforms = getManager()->getComponentsArray<Transform>();
        
        
        for( auto& trans : transforms ){
            
            // draw parent child relation
            if( trans->hasParent() ){
                
                auto parentHandle = trans->getParent();
                ci::gl::drawLine(trans->getWorldPos(),  parentHandle->getWorldPos() );
                
            }
            
            
            {
                // draw AnchorPoint
                ci::gl::ScopedModelMatrix m;
                ci::gl::translate( trans->getWorldPos() );
                ci::gl::drawSolidRect( { -5,-5, 5,5 }  );
                
            }
            
            {
                ci::gl::ScopedModelMatrix m;
                ci::gl::multModelMatrix( trans->getWorldTransform() );
                ci::gl::translate( trans->getAnchorPoint() );
                const float length = 50;
                // x axis
                ci::gl::color(ci::Color( 1.0f, 0, 0 ));
                ci::gl::drawLine( glm::vec3(0,0, 0), glm::vec3(length, 0, 0) );
                
                // y axis
                ci::gl::color( ci::Color( 1.0f, 1.0f, 0.0f ) );
                ci::gl::drawLine( glm::vec3( 0,0 ,0), glm::vec3( 0, -length,0 ) );
                
                ci::gl::color( ci::Color::white());
            }
        }

         */
    
    
};


#endif /* TransformSystem_h */
