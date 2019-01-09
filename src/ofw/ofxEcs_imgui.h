//
//  ecs_imgui.h
//  EcsImgui
//
//  Created by Henrique on 12/3/18.
//

#ifndef ecs_imgui_h
#define ecs_imgui_h


#include "ofxImGui.h"
#include "Manager.h"
#include "Transform.hpp"
#include "Bounds.h"

namespace ImGui{
    
    inline bool EcsTransform( Transform * t ){
        
        ImGui::Text("Entity id: %u", t->getEntity()->getId() );
        
        bool b = false;
        b |= ImGui::DragFloat3("position", &(t->getPosPtr()->x));
        b |= ImGui::DragFloat3("anchor", &(t->getAnchorPointPtr()->x));
        b |= ImGui::DragFloat3("scale", &(t->getScalePtr()->x), 0.01);
        
        if ( b ){
            t->updateMatrices();
        }
        
        auto rot = t->getRotationRadians();
        if( ImGui::DragFloat("rotation", &rot, 0.01) ){
            t->setRotation(rot);
            b = true;
        }
        
        return b;
    }
    
    inline bool SelectTransform( Transform* t, Transform*& outputTransform ){
        
        static int selection_mask = (1 << 0);
        int node_clicked = -1;
        
        ecs::EntityRef selectedEntity = nullptr;
        
        
        std::function<void(Transform*&, Transform*&)> drawChildren = [&](Transform*& root, Transform*& out ){
            
            
            ImGui::PushID("id");
            
            auto rootId = root->getId();//  root->getEntity().lock()->getId();
            auto id_text = "e id: " + std::to_string( rootId );
            
            auto nodeName = std::to_string(rootId).c_str();
            bool isSelected = ((selection_mask & (1 << rootId)));
            
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((isSelected == true) ? ImGuiTreeNodeFlags_Selected : 0 ) ;
            
            if( isSelected ){
                out  = root;
            }
            
            if( !root->isLeaf() ){
                
                bool nodeOpen =  ImGui::TreeNodeEx((void*)(intptr_t)rootId, node_flags, "id: %lu", rootId);

                if(ImGui::IsItemClicked()){
                    node_clicked = rootId;
                }
                
                if( nodeOpen ){
                    for( auto &child : root->getChildren() ) {
                        auto ptr = child;
                        drawChildren( ptr, out );
                    }
                    ImGui::TreePop();
                }
                
            }else{
                
                ImGui::TreeNodeEx((void*)(intptr_t)rootId, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "id: %lu", rootId);
                if(ImGui::IsItemClicked()){
                    node_clicked = rootId;
                }
            }
            
            
            ImGui::PopID();
        }; // end of lambda
        
        drawChildren( t, outputTransform);
        
        if( node_clicked != -1 ){
            selection_mask = (1 << node_clicked);
            return  true;
        }
        
        return  false;
    }
}


#endif /* ecs_imgui_h */
