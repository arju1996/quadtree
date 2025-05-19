#pragma once

#include "quad.h"
#include <raylib.h> 

namespace QuadtreeRaylibDrawing {
    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void drawQuadTree(QuadTree<OBJECT_NAME, PointType, BoundType>* root, typename QuadTree<OBJECT_NAME, PointType, BoundType>::PositionGetter position_getter) {
                const Color boundary_color {230, 41, 55, 75};
        const Color point_color {GREEN};

        if(root == nullptr)
            return;
        else {
            const BoundType& boundry = root->getBoundry();
            // not sure if its a good idea to access tl of a template
            DrawRectangleLines(
                boundry.tl.x
                , boundry.tl.y
                , boundry.br.x - boundry.tl.x
                , boundry.br.y - boundry.tl.y
                , boundary_color
            );
            for(auto node : root->getPoints()) {
                const PointType& item = position_getter(*node);
                DrawCircle(item.x, item.y, 2, point_color);
            }
            drawQuadTree(root->getTLChild().get(), position_getter);
            drawQuadTree(root->getTRChild().get(), position_getter);
            drawQuadTree(root->getBLChild().get(), position_getter);
            drawQuadTree(root->getBRChild().get(), position_getter);

        }
    }
}