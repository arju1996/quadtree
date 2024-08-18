#include <vector>
#include "Math_helper_2.h"

template <typename OBJECT_NAME>
class QuadTree {
public:
    using PositionGetter = std::function<Math::Point2i(const OBJECT_NAME&)>;
    QuadTree() = default;
    // QuadTree(std::vector<Math::Point2i> data, Math::Recti boundry, int capacity, int depth);
    QuadTree(std::vector<OBJECT_NAME*> data, PositionGetter position_getter, Math::Recti boundry, int capacity, int depth);
    ~QuadTree();
// private:
    Math::Recti boundry_;
    int capacity_;
    int depth_;
    bool isLeaf_;
    std::vector<OBJECT_NAME*> points_;
    QuadTree<OBJECT_NAME>* tl_child_, *tr_child_, *bl_child_, *br_child_;

    //
    // void BuildTree(std::vector<Math::Point2i> points);
    // QuadTree* BuildTree(std::vector<Math::Point2i> points);
    // QuadTree* BuildTree(std::vector<Math::Point2i> points,  Math::Recti boundry, int capacity, int depth);
};

template <typename OBJECT_NAME>
class QuadTreeMaker {
public:
    using PositionGetter = std::function<Math::Point2i(const OBJECT_NAME&)>;
    QuadTree<OBJECT_NAME>* BuildTree(std::vector<OBJECT_NAME*> points, PositionGetter position_getter, Math::Recti boundry, int capacity, int max_depth, int depth = 0);
    std::vector<OBJECT_NAME*> findNeighbors(QuadTree<OBJECT_NAME>* root, Math::Point2i point, PositionGetter position_getter);
};



//imple
template <typename OBJECT_NAME>
QuadTree<OBJECT_NAME>* QuadTreeMaker<OBJECT_NAME>::BuildTree(std::vector<OBJECT_NAME*> points,
        PositionGetter position_getter,
        Math::Recti boundry,
        int capacity,
        int max_depth,
        int depth
        ) {
    
    // step1: split to 4;
    // step2: find and assign points to 4 of them;
    // step3: call BuildTree


    //edge case
    QuadTree<OBJECT_NAME>* current = new QuadTree<OBJECT_NAME>();

    if(points.size() <= capacity || depth >= max_depth) {
        current->boundry_ = boundry;
        current->capacity_ = capacity;
        current->depth_ = depth;
        current->isLeaf_ = true;
        current->points_ = points;
        current->tl_child_ = nullptr;
        current->tr_child_ = nullptr;
        current->bl_child_ = nullptr;
        current->br_child_ = nullptr;

        return current;
    }


    Math::Recti tl, tr, bl, br;

    Math::Point2i mid = (boundry.tl + boundry.br) / 2.0f;
    tl.tl = boundry.tl;
    tl.br = mid;

    tr.tl = {mid.x, boundry.tl.y};
    tr.br = {boundry.br.x, mid.y};

    bl.tl = {boundry.tl.x, mid.y};
    bl.br = {mid.x, boundry.br.y};

    br.tl = mid;
    br.br = boundry.br;

    std::vector<OBJECT_NAME*> points_tl, points_tr, points_bl, points_br; 
    for (auto obj: points) {
        const Math::Point2i& item = position_getter(*obj);
        // is a point within which of the crossing?
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                points_tl.push_back(obj);
            } else {
                points_bl.push_back(obj);
            }
        } else {
            if(item.y < mid.y) {
                points_tr.push_back(obj);
            } else {
                points_br.push_back(obj);
            }
        }
    }
    
    // first find the data that will come in tl_child;
    current->tl_child_ = BuildTree(points_tl, position_getter, tl, capacity, max_depth, depth + 1);
    current->tr_child_ = BuildTree(points_tr, position_getter, tr, capacity, max_depth, depth + 1);
    current->bl_child_ = BuildTree(points_bl, position_getter, bl, capacity, max_depth, depth + 1);
    current->br_child_ = BuildTree(points_br, position_getter, br, capacity, max_depth, depth + 1);
    current->boundry_ = boundry;
    current->capacity_ = capacity;
    current->depth_ = depth;
    current->isLeaf_ = false;
    return current;
}

// ok looks like this dont need the PositionGetter
template <typename OBJECT_NAME>
std::vector<OBJECT_NAME*> QuadTreeMaker<OBJECT_NAME>::findNeighbors(QuadTree<OBJECT_NAME>* root, Math::Point2i point, PositionGetter position_getter) {
    // need function to check if a point is inside a box/boundry
    if (root == nullptr) {
        return {};
    }
    if(!root->boundry_.contains(point)) {
        return {};
    } else {
        if(root->isLeaf_) {
            // return all items in this
            // for(auto item: root->points_) {
            //     const Math::Point2i& item2 = position_getter(*item);
            //     std::cout<<"neightbor : "<<item2<<"\n";
            // }
            return root->points_;
        } else {
            Math::Point2i mid = (root->boundry_.tl + root->boundry_.br) / 2.0f;
            if(point.x < mid.x) {
                if(point.y < mid.y) {
                    return findNeighbors(root->tl_child_, point, position_getter);
                } else {
                    return findNeighbors(root->bl_child_, point, position_getter);
                }
            } else {
                if(point.y < mid.y) {
                    return findNeighbors(root->tr_child_, point, position_getter);
                } else {
                    return findNeighbors(root->br_child_, point, position_getter);
                }
            }
        }
    }
}

template <typename OBJECT_NAME>
QuadTree<OBJECT_NAME>::~QuadTree() {
    delete tl_child_;
    delete tr_child_;
    delete bl_child_;
    delete br_child_;
}   