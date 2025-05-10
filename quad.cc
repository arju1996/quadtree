#include "quad.h"

// QuadTreeMaker::QuadTreeMaker(std::vector<Math::Point2i> data, Math::Recti boundry, int capacity, int maxDepath) {
//     makeQuadTree();
// }

template <typename OBJECT_NAME, typename PointType, typename RectType>
QuadTree<OBJECT_NAME, PointType, RectType>::QuadTree(std::vector<OBJECT_NAME*> data, PositionGetter position_getter, RectType boundry, int capacity, int depth)
    : boundry_(boundry)
    , capacity_(capacity)
    , depth_(depth)
    , isLeaf_(false)
    , tl_child_(nullptr), tr_child_(nullptr), bl_child_(nullptr), br_child_(nullptr) 
    {
        // BuildTree(data);
}
