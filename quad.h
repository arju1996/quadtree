#include <vector>
#include "Math_helper_2.h"


class QuadTree {
public:
    QuadTree() = default;
    QuadTree(std::vector<Math::Point2i> data, Math::Recti boundry, int capacity, int depth);
    ~QuadTree();
// private:
    Math::Recti boundry_;
    int capacity_;
    int depth_;
    bool isLeaf_;
    std::vector<Math::Point2i> points_;
    QuadTree* tl_child_, *tr_child_, *bl_child_, *br_child_;

    //
    // void BuildTree(std::vector<Math::Point2i> points);
    // QuadTree* BuildTree(std::vector<Math::Point2i> points);
    // QuadTree* BuildTree(std::vector<Math::Point2i> points,  Math::Recti boundry, int capacity, int depth);
};


class QuadTreeMaker {
public:
    QuadTree* BuildTree(std::vector<Math::Point2i> points,  Math::Recti boundry, int capacity, int depth, int max_depth);
    std::vector<Math::Point2i> findNeighbors(QuadTree* root, Math::Point2i point);
};