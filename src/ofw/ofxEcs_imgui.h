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

        if( ImGui::GetIO().KeyShift ){
            if(ImGui::DragFloat("scale", &(t->getScalePtr()->x), 0.001)){
                t->getScalePtr()->y = t->getScalePtr()->x;
                t->getScalePtr()->z = t->getScalePtr()->x;
                b  |= true;
            }
        }else{
            b |= ImGui::DragFloat3("scale", &(t->getScalePtr()->x), 0.01);
        }
        
        if ( b ){
            t->updateMatrices();
        }
        
        auto rot = t->get2dRotation();
        if( ImGui::DragFloat("rotation", &rot, 0.01) ){
            t->set2dRotation(rot);
            b = true;
        }
        
        return b;
    }
    
    inline bool SelectTransform( Transform* t, Transform*& outputTransform ){
        
        static long int selectedNode = -1;
        long int node_clicked = -1;
        
        ecs::EntityRef selectedEntity = nullptr;
        
        
        std::function<void(Transform*&, Transform*&)> drawChildren = [&](Transform*& root, Transform*& out ){
            
            auto rootId = root->getEntity()->getId();//  root->getEntity().lock()->getId();
            auto id_text = "e id: " + std::to_string( rootId );
            
            ImGui::PushID(id_text.c_str());

            auto nodeName = root->getEntity()->getName();
            bool isSelected = selectedNode == rootId;
            
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((isSelected == true) ? ImGuiTreeNodeFlags_Selected : 0 ) ;
            
            if( isSelected ){
                out  = root;
            }
            
            
            
            if( !root->isLeaf() ){
                
                bool nodeOpen =  ImGui::TreeNodeEx((void*)(intptr_t)rootId, node_flags, "%lu: %s", rootId, nodeName.c_str() );

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
                
                ImGui::TreeNodeEx((void*)(intptr_t)rootId, node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, "%lu: %s", rootId, nodeName.c_str() );
                if(ImGui::IsItemClicked()){
                    node_clicked = rootId;
                }
            }
            
            
            ImGui::PopID();
        }; // end of lambda
        
        drawChildren( t, outputTransform);
        
        if( node_clicked != -1 ){
            selectedNode = node_clicked;
            return  true;
        }
        
        return  false;
    }
}


#endif /* ecs_imgui_h */
