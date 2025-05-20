#include <iostream>
#include <string>
#include "quad.h"
#include <random>

#include "Math_helper_2.h"
#include "quadtree_drawing_raylib.h"


#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

class Boid {
    public:
    Math::Point2i position;
};

auto position_getter = [](const Boid& obj) -> Math::Point2i {
    return obj.position;
};

Math::Point2i GenerateRandomVector2Positive(int yMax, int xMax) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, xMax);
    std::uniform_int_distribution<> dist2(0, yMax);
    return {dist(gen), dist2(gen)};
}

void populate(std::vector<Boid*> &boids,  Math::Recti boundry, int count) {
    for(int i = 0; i<count; ++i) {
        Math::Point2i a = GenerateRandomVector2Positive(boundry.br.y, boundry.br.x);
        boids.push_back(new Boid{a});
    }
}

int main() {
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

    while(!WindowShouldClose()) {
        data.clear();
        populate(data, {{0,0},{WINDOW_WIDTH, WINDOW_HEIGHT}}, n);

        QuadTreeMaker<Boid, Math::Point2i, Math::Recti> a;
        std::unique_ptr<QuadTree<Boid, Math::Point2i, Math::Recti>> root = a.BuildTree(data, position_getter, {{0,0}, {WINDOW_WIDTH, WINDOW_HEIGHT}}, capacity, max_depth);

        Boid search_item_boid = *data[12];
        std::vector<Boid*> neighbors = a.findNeighbors(root.get(), position_getter(search_item_boid));
        
        BeginDrawing();
        ClearBackground(BLACK);

        // draw search target
        DrawCircleLines(position_getter(search_item_boid).x, position_getter(search_item_boid).y, 5, DARKBLUE);
        // draw neighbors
        for(auto neighbor : neighbors) {
            const auto &item = position_getter(*neighbor);
            DrawCircleLines(item.x, item.y, 5, BLUE);
        }
        // draw quadtree
        QuadtreeRaylibDrawing::drawQuadTree(root.get(), position_getter);
        // QuadtreeRaylibDrawing::print(root.get());

        // draw fps;
        std::string fps_string = "FPS = "+std::to_string(GetFPS());
        DrawText(fps_string.c_str(), 20, 20, 30, LIGHTGRAY);

        EndDrawing();
        std::cout<<"fps_string : "<<fps_string<<"\n";
        for(auto item: data)
            delete item;
    }
    return 0;
}