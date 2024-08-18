#include <iostream>
#include "quad.h"



void populate(std::vector<Math::Point2i>& points,  Math::Recti& boundry, int& capacity, int& depth) {
    int n = 10;
    boundry = {{0,0}, {1000,1000}};
    capacity = 2;
    depth = 0;
    for(int i = 0; i<n; ++i) {
        points.push_back({i,i});
    }
    //  for(int i = 0; i<n; ++i) {
        // points.push_back({400,400});
        // points.push_back({300,300});
        // points.push_back({200,200});
        // points.push_back({700,700});
    // }
}

void print(QuadTree* root) {
    if(root == nullptr)
        return;
    else {
        std::cout<<"boundry:"<<root->boundry_<<"\n";
        std::cout<<"points_:"<<root->points_.size()<<"\n";
        std::cout<<"isLeaf_:"<<root->isLeaf_<<"\n";
        // std::cout<<"points_:"<<root->points_.size()<<"\n";
        print(root->tl_child_);
        print(root->tr_child_);
        print(root->bl_child_);
        print(root->br_child_);
    }
}
int main() {
    std::cout<<"hello world;";

    std::vector<Math::Point2i> points;
    Math::Recti boundry;
    int capacity;
    int depth;
    populate(points, boundry, capacity, depth);
    QuadTreeMaker a;
    QuadTree* root = a.BuildTree(points, boundry, capacity, depth);
    print(root);

    return 0;
}