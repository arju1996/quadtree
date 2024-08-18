#include <iostream>
#include "quad.h"
#include <random>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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

void drawQuadTree(QuadTree* root) {
    if(root == nullptr)
        return;
    else {
        // std::cout<<"boundry:"<<root->boundry_<<"\n";
        DrawRectangleLines(
            root->boundry_.tl.x
            , root->boundry_.tl.y
            , root->boundry_.br.x - root->boundry_.tl.x
            , root->boundry_.br.y - root->boundry_.tl.y,
            // RED
            { 230, 41, 55, 75 }
        );
        for(auto item : root->points_) {
            DrawCircle(item.x, item.y, 2, GREEN);
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
        // std::cout<<"point : "<<a<<"\n";
        points.push_back(a);
    }


    // //  for(int i = 0; i<n; ++i) {
    //     points.push_back({400,400});
    //     points.push_back({300,300});
    //     points.push_back({200,200});
    //     points.push_back({700,700});
    // // }
}

int main() {
    std::cout<<"hello world;";

    std::vector<Math::Point2i> points;
    Math::Recti boundry;
    int capacity = 5;
    int depth = 0;
    int max_depth = 8;
    int n = 1000;


    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 1000;
    const int CELL_SIZE = 40;
    // int FPS = 30;
    int FPS = 1;
    

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(FPS);

/*
    populate(points, {{0,0},{WINDOW_WIDTH, WINDOW_HEIGHT}}, 1000);
    QuadTreeMaker a;
    QuadTree* root = a.BuildTree(points, {{0,0}, {WINDOW_WIDTH, WINDOW_HEIGHT}}, capacity, depth, 3);
    // print(root);

    std::cout<<"points[12] : "<<points[12];
    std::vector<Math::Point2i> neighbors = a.findNeighbors(root, points[12]);
    delete root;
*/

    while(!WindowShouldClose()) {

        points.clear();
        populate(points, {{0,0},{WINDOW_WIDTH, WINDOW_HEIGHT}}, n);
        QuadTreeMaker a;
        QuadTree* root = a.BuildTree(points, {{0,0}, {WINDOW_WIDTH, WINDOW_HEIGHT}}, capacity, depth, max_depth);
        // print(root);

        // std::cout<<"points[12] : "<<points[12];
        std::vector<Math::Point2i> neighbors = a.findNeighbors(root, points[12]);

        BeginDrawing();
        ClearBackground(BLACK);

        // draw points;
        DrawCircleLines(points[12].x, points[12].y, 5, DARKBLUE);
        for(auto item : neighbors) {
            DrawCircleLines(item.x, item.y, 5, BLUE);
        }
        drawQuadTree(root);

        // draw splitting;
        std::string fps_string = "FPS = "+std::to_string(GetFPS());
        DrawText(fps_string.c_str(), 20, 20, 10, BLACK);

        EndDrawing();
        std::cout<<"fps_string : "<<fps_string<<"\n";
        delete root;


    }
    return 0;
}