#include "quad.h"

// QuadTreeMaker::QuadTreeMaker(std::vector<Math::Point2i> data, Math::Recti boundry, int capacity, int maxDepath) {
//     makeQuadTree();
// }


QuadTree::QuadTree(std::vector<Math::Point2i> data, Math::Recti boundry, int capacity, int depth)
    : boundry_(boundry)
    , capacity_(capacity)
    , depth_(depth)
    , isLeaf_(false)
    , tl_child_(nullptr), tr_child_(nullptr), bl_child_(nullptr), br_child_(nullptr) 
    {
        // BuildTree(data);
}

QuadTree::~QuadTree() {
    delete tl_child_;
    delete tr_child_;
    delete bl_child_;
    delete br_child_;
}

QuadTree* QuadTreeMaker::BuildTree(std::vector<Math::Point2i> points,
        Math::Recti boundry,
        int capacity,
        int depth,
        int max_depth) {
    
    // step1: split to 4;
    // step2: find and assign points to 4 of them;
    // step3: call BuildTree


    //edge case
    QuadTree* current = new QuadTree();

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

    std::vector<Math::Point2i> points_tl, points_tr, points_bl, points_br; 
    for (auto item: points) {
        // is a point within which of the crossing?
        if(item.x < mid.x) {
            if(item.y < mid.y) {
                points_tl.push_back(item);
            } else {
                points_bl.push_back(item);
            }
        } else {
            if(item.y < mid.y) {
                points_tr.push_back(item);
            } else {
                points_br.push_back(item);
            }
        }
    }
    
    // first find the data that will come in tl_child;
    current->tl_child_ = BuildTree(points_tl, tl, capacity, depth + 1, max_depth);
    current->tr_child_ = BuildTree(points_tr, tr, capacity, depth + 1, max_depth);
    current->bl_child_ = BuildTree(points_bl, bl, capacity, depth + 1, max_depth);
    current->br_child_ = BuildTree(points_br, br, capacity, depth + 1, max_depth);
    current->boundry_ = boundry;
    current->capacity_ = capacity;
    current->depth_ = depth;
    current->isLeaf_ = false;
    return current;
}

std::vector<Math::Point2i> QuadTreeMaker::findNeighbors(QuadTree* root, Math::Point2i point) {
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
            //     std::cout<<"neightbor : "<<item<<"\n";
            // }
            return root->points_;
        } else {
            Math::Point2i mid = (root->boundry_.tl + root->boundry_.br) / 2.0f;
            if(point.x < mid.x) {
                if(point.y < mid.y) {
                    return findNeighbors(root->tl_child_, point);
                } else {
                    return findNeighbors(root->bl_child_, point);
                }
            } else {
                if(point.y < mid.y) {
                    return findNeighbors(root->tr_child_, point);
                } else {
                    return findNeighbors(root->br_child_, point);
                }
            }
        }
    }
}