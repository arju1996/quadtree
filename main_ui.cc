#include <iostream>
#include <string>
#include "quad.h"
#include <random>

#include "Math_helper_2.h"


#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// template <typename OBJECT_NAME>
// void print(QuadTree<OBJECT_NAME>* root) {
//     if(root == nullptr)
//         return;
//     else {
//         std::cout<<"boundry:"<<root->boundry_<<"\n";
//         std::cout<<"points_:"<<root->points_.size()<<"\n";
//         std::cout<<"isLeaf_:"<<root->isLeaf_<<"\n";
//         print(root->tl_child_);
//         print(root->tr_child_);
//         print(root->bl_child_);
//         print(root->br_child_);
//     }
// }

class Boid {
    public:
    Math::Point2i position;
};

auto position_getter = [](const Boid& obj) -> Math::Point2i {
    return obj.position;
};

void drawQuadTree(QuadTree<Boid, Math::Point2i, Math::Recti>* root) {
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
            const Math::Point2i& item = position_getter(*node);
            DrawCircle(item.x, item.y, 2, point_color);
        }
        drawQuadTree(root->tl_child_);
        drawQuadTree(root->tr_child_);
        drawQuadTree(root->bl_child_);
        drawQuadTree(root->br_child_);
    }
}

Math::Point2i GenerateRandomVector2Positive(int yMax, int xMax) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, xMax);
    std::uniform_int_distribution<> dist2(0, yMax);
    return {dist(gen), dist2(gen)};
}

void populate(std::vector<Math::Point2i>& points,  Math::Recti boundry, int count) {
    for(int i = 0; i<count; ++i) {
        Math::Point2i a = GenerateRandomVector2Positive(boundry.br.y, boundry.br.x);
        points.push_back(a);
    }
}

void populate(std::vector<Boid*> &boids,  Math::Recti boundry, int count) {
    for(int i = 0; i<count; ++i) {
        Math::Point2i a = GenerateRandomVector2Positive(boundry.br.y, boundry.br.x);
        boids.push_back(new Boid{a});
    }
}

int main() {
    std::cout<<"hello world;";

    std::vector<Math::Point2i> points;
    std::vector<Boid*> data;
    Math::Recti boundry;
    int capacity = 5;
    int max_depth = 8;
    int n = 5000;

    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 1000;
    const int CELL_SIZE = 40;
    int FPS = 30;
    

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(FPS);

    //populate(data, { {0,0},{WINDOW_WIDTH, WINDOW_HEIGHT} }, n);


    while(!WindowShouldClose()) {

        data.clear();
        populate(data, {{0,0},{WINDOW_WIDTH, WINDOW_HEIGHT}}, n);
        QuadTreeMaker<Boid, Math::Point2i, Math::Recti> a;
        QuadTree<Boid, Math::Point2i, Math::Recti>* root = a.BuildTree(data, position_getter, {{0,0}, {WINDOW_WIDTH, WINDOW_HEIGHT}}, capacity, max_depth);

        Boid search_item_boid = *data[12];
        std::vector<Boid*> neighbors = a.findNeighbors(root, position_getter(search_item_boid), position_getter);
        BeginDrawing();
        ClearBackground(BLACK);

        // draw points;
        // draw search target
        DrawCircleLines(position_getter(search_item_boid).x, position_getter(search_item_boid).y, 5, DARKBLUE);
        // draw neighbor
        for(auto data : neighbors) {
            const auto &item = position_getter(*data);
            DrawCircleLines(item.x, item.y, 5, BLUE);
        }
        // draw quadtree
        drawQuadTree(root);

        // draw fps;
        // root->Draw(root);
        std::string fps_string = "FPS = "+std::to_string(GetFPS());
        DrawText(fps_string.c_str(), 20, 20, 30, LIGHTGRAY);

        EndDrawing();
        std::cout<<"fps_string : "<<fps_string<<"\n";
        delete root;
        for(auto item: data)
            delete item;
    }
    return 0;
}