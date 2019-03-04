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

#include <iostream>

/*
@Notes:
 make a separate draw target
 
*/

class Transform;

namespace ecs {
    // foward declare
    class DrawTarget;

    // Interface for drawble object
    struct IDrawable {
        
        IDrawable();
        IDrawable( DrawTarget* iDrawTarget );
        virtual ~IDrawable();
        virtual void draw() = 0;
        
        void setDrawTarget( std::shared_ptr<DrawTarget> iDrawTarget );
        
        bool hasDrawTarget() { return (drawTargetOwner != nullptr);  }
        
        void setDrawable( bool d ){
            drawable = d;
        }
        
        // transverse a transform tree and set all drawables in there to true or false
        static void setTree( Transform* t, bool d );
        
        bool isDrawable(){
            return drawable;
        }
        
        void drawAtTop();

        size_t listPositionIndex = -1;
        
    protected:
        
        bool drawable = true;
        
        int drawTargetId = -1;
        DrawTarget* drawTargetOwner = nullptr;

        friend  DrawTarget;
    };



    // simple draw target
    struct DrawTarget{
        
        
        DrawTarget(){
            id_count++;
            _id = id_count;
        }
    
        
        virtual void update(){
            // implement render strategy, z sort, added order sort etc...
        }
        
        
        // actually draw stuff
        virtual void draw(){
			_drawCalls = 0;
            // set matrices, bind FBO etc...
            for(auto d : mDrawables){
                if(d->isDrawable())
                {
					_drawCalls += 1;
                    d->draw();
                }
            }
        }
        
        
        
        //TODO: add on top of, bellow, etc...
        void addDrawable( IDrawable* iDrawable ){
            iDrawable->drawTargetId = mDrawables.size();
            iDrawable->drawTargetOwner = this;
            mDrawables.push_back( iDrawable );
            iDrawable->listPositionIndex = mDrawables.size() - 1;
        }
        
        
        void removeDrawable( IDrawable* drawable ){
            
            if(  drawable != nullptr ){
                
                size_t listPositionIndex = drawable->listPositionIndex;
                auto it = std::find(mDrawables.begin(), mDrawables.end(), drawable);
                
                if ( it != mDrawables.end()) {
                    mDrawables.erase(it);
                }else{
                    std::cout << "error, drawable not found in this target" << std::endl;
                }
            }
        }
        
        void swapDrawablesOrder(IDrawable* a, IDrawable* b);
        
        int getDrawableCount(){
            return mDrawables.size();
        }


		int getNumOfDrawCalls() {
			return _drawCalls;
		}

        
    protected:

		std::vector<IDrawable*> mDrawables;

        uint32_t _id;
        static uint32_t id_count;

		uint32_t _drawCalls = 0;
        friend class DrawSystem;
    };




    // performs the actuall drawing
    class DrawSystem{
        
    public:
        DrawSystem(){
            
            auto t = std::make_shared<DrawTarget>();
            mDrawTargets["default"] = t;
        }
        
        
        static DrawSystem* getInstance();
        
        
        void draw(){
            
            for(auto& d : mDrawTargets){
                if(d.second)
                    d.second->draw();
            }
        }
        
        void addDrawTarget(const std::shared_ptr<DrawTarget> iDrawTarget ){
            mDrawTargets[ std::to_string( iDrawTarget->_id ) ] = iDrawTarget;
        };

        void addDrawTarget(const std::string& name,  const std::shared_ptr<DrawTarget> iDrawTarget ){
            mDrawTargets[name] =  iDrawTarget;
        };

        
        void setDefaultDrawTarget( const std::shared_ptr<DrawTarget> iDrawTarget ){
            mDrawTargets["default"] = iDrawTarget;
        }
        
        
        std::shared_ptr<DrawTarget> getDefaultDrawTarget(){
            return mDrawTargets["default"];
        }
        
        
    private:
        
        static DrawSystem* mInstance; // singleton instance
        std::map <std::string, std::shared_ptr<DrawTarget>> mDrawTargets;
    };
}
#endif /* Drawables_hpp */
