//
//  Updatable.hpp
//  SuffolkSlides
//
//  Created by Henrique on 12/5/18.
//

#ifndef Updatable_hpp
#define Updatable_hpp

#include <stdio.h>

#include <memory>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <iostream>


namespace ecs {
    // foward declare
    class UpdateTarget;
    
    // Interface for drawble object
    struct IUpdatable {
        
        IUpdatable();
        IUpdatable( UpdateTarget* iUpdateTarget );
        virtual ~IUpdatable();
        virtual void update() = 0;
        
        void setUpdateTarget( std::shared_ptr<UpdateTarget>& iUpdateTarget );
        
        bool hasUpdateTarget() { return (UpdateTargetOwner != nullptr);  }
        
        virtual void setUpdatable( bool d ){
            Updatable = d;
        }
        
        bool isUpdatable(){
            return Updatable;
        }
        
        size_t listPositionIndex = -1;
        
    protected:
        
        bool Updatable = true;
        
        int UpdateTargetId = -1;
        UpdateTarget* UpdateTargetOwner = nullptr;
        
        friend  UpdateTarget;
    };
    
    
    
    // simple draw target
    struct UpdateTarget{
        
        
        UpdateTarget(){
            id_count++;
            _id = id_count;
        }
        
        virtual void update(){
            mUpdateCalls = 0;
            for(auto d : mUpdatables){
                if(d->isUpdatable())
                {
                    mUpdateCalls++;
                    d->update();
                }
            }
        }
        
        
        //TODO: add on top of, bellow, etc...
        void addUpdatable( IUpdatable* IUpdatable ){
            
            IUpdatable->UpdateTargetId = mUpdatables.size();
            IUpdatable->UpdateTargetOwner = this;
            mUpdatables.push_back( IUpdatable );
            
            IUpdatable->listPositionIndex = mUpdatables.size() - 1 ;
        }
        
        
        void removeUpdatable( IUpdatable* Updatable ){
            
            if(  Updatable != nullptr ){
                
                size_t listPositionIndex = Updatable->listPositionIndex;
                auto it = std::find(mUpdatables.begin(), mUpdatables.end(), Updatable);
                
                if ( it != mUpdatables.end()) {
                    mUpdatables.erase(it);
                }else{
                    std::cout << "error, Updatable not found in this target" << std::endl;
                }
                
            }
        }
        
        int getUpdatableCount(){
            return mUpdatables.size();
        }
        
        std::vector<IUpdatable*> mUpdatables;
        
        uint32_t getNumOfUpdateCalls(){ return mUpdateCalls; }
        
    protected:
        
        uint32_t mUpdateCalls;
        uint32_t _id;
        static uint32_t id_count;
        
        friend class UpdateSystem;
    };
    
    
    
    
    // performs the actuall drawing
    class UpdateSystem{
        
    public:
        UpdateSystem(){
            auto t = std::make_shared<UpdateTarget>();
            mUpdateTargets["default"] = t;
        }
        
        
        static UpdateSystem* getInstance();
        
        
        void update(){
            
            if( doUpdate ){
                for(auto& d : mUpdateTargets){
                    d.second->update();
                }
            }
        }
        
        void addUpdateTarget(const std::shared_ptr<UpdateTarget> iUpdateTarget ){
            mUpdateTargets[ std::to_string( iUpdateTarget->_id ) ] = iUpdateTarget;
        };
        
        void addUpdateTarget(const std::string& name,  const std::shared_ptr<UpdateTarget> iUpdateTarget ){
            mUpdateTargets[name] =  iUpdateTarget;
        };
        
        
        
        std::shared_ptr<UpdateTarget> getDefaultUpdateTarget(){
            return mUpdateTargets["default"];
        }
        
        bool doUpdate = true;
        
    private:
        
        static UpdateSystem* mInstance; // singleton instance
        std::map <std::string, std::shared_ptr<UpdateTarget>> mUpdateTargets;
    };

}

#endif /* Updatable_hpp */
