#pragma once

#include "quad.h"
#include <raylib.h> 

namespace QuadtreeRaylibDrawing {
    template <typename OBJECT_NAME, typename PointType, typename RectType>
    void drawQuadTree(QuadTree<OBJECT_NAME, PointType, RectType>* root, typename QuadTree<OBJECT_NAME, PointType, RectType>::PositionGetter position_getter) {
                const Color boundary_color {230, 41, 55, 75};
        const Color point_color {GREEN};

        if(root == nullptr)
            return;
        else {
            DrawRectangleLines(
                root->boundry_.tl.x
                , root->boundry_.tl.y
                , root->boundry_.br.x - root->boundry_.tl.x
                , root->boundry_.br.y - root->boundry_.tl.y,
                boundary_color
            );
            for(auto node : root->points_) {
                const PointType& item = position_getter(*node);
                DrawCircle(item.x, item.y, 2, point_color);
            }
            drawQuadTree(root->tl_child_, position_getter);
            drawQuadTree(root->tr_child_, position_getter);
            drawQuadTree(root->bl_child_, position_getter);
            drawQuadTree(root->br_child_, position_getter);
        }
    }
}