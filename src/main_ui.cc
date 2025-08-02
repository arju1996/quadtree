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
    bool operator==(const Boid& other) const {
        return position == other.position;
    }
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
    n = 16;

    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 1000;
    const int CELL_SIZE = 40;
    int FPS = 30;
    

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(FPS);

    // populate(data, {{0,0},{WINDOW_WIDTH, WINDOW_HEIGHT}}, n);
    QuadTree<Boid, Math::Point2i, Math::Recti> quadtree;
    std::unique_ptr<QuadTreeNode<Boid, Math::Point2i, Math::Recti>> root;

    // create
    root = quadtree.BuildTree(data, position_getter, {{0,0}, {WINDOW_WIDTH, WINDOW_HEIGHT}}, capacity, max_depth, 0);

    // search
    // Boid search_item_boid = *data[12];
    // std::vector<Boid*> neighbors = quadtree.findNeighbors(root.get(), position_getter(search_item_boid));

    //
    Boid* search_item_boid;
    std::vector<Boid*> neighbors;
    search_item_boid = new Boid{{0, 0}};

    Math::Point2i offset1 = {50, 50};
    Math::Point2i offset2 = {51, 51};
    Math::Recti search_area{offset1, offset2}; 
    bool removed = false;
    //
    while(!WindowShouldClose()) {
        // data.clear();

        // root = a.BuildTree(data, position_getter, {{0,0}, {WINDOW_WIDTH, WINDOW_HEIGHT}}, capacity, max_depth);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // Add boid at mouse position
            Math::Point2i mouse_pos = {GetMouseX(), GetMouseY()};
            Boid* new_boid = new Boid{mouse_pos};
            data.push_back(new_boid);
            quadtree.insert(root, new_boid);
        }

        if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            // Add boid at mouse position
            Math::Point2i mouse_pos = {GetMouseX(), GetMouseY()};
            search_item_boid = new Boid{mouse_pos};
            
            // neighbors = quadtree.findNeighbors(root.get(), position_getter(*search_item_boid));
            auto search_item_location = position_getter(*search_item_boid);
            Math::Point2i offset = {50, 50};
            auto search_item_rect_tl = search_item_location - offset;
            auto search_item_rect_br = search_item_location + offset;
            search_area = {search_item_rect_tl, search_item_rect_br};
            neighbors = quadtree.findNeighbors(root.get(), search_area);
            
            std::cout<<"FOUND  "<<neighbors.size()<<" neighbours "<<"\n";
            removed = false; // to show neighbors
        }

        if(IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
            int count = 0;
            for(auto boid : neighbors) {
                quadtree.remove(root, boid);
                // remove the boid from data after removing from qtree
                auto found = std::find(data.begin(), data.end(), boid);
                if(found != data.end()) {
                    data.erase(found);
                    count++;
                }
            }
            std::cout<<"removed  "<<count<<" neighbours from data "<<"\n";
            removed = true;
        }


        BeginDrawing();
        ClearBackground(BLACK);

        if(!removed) {
            // draw search target
            DrawCircleLines(position_getter(*search_item_boid).x, position_getter(*search_item_boid).y, 5, DARKBLUE);
            // draw rect;
            const Color boundary_color {BLUE};
            DrawRectangleLines(
                search_area.tl.x
                , search_area.tl.y
                , search_area.br.x - search_area.tl.x
                , search_area.br.y - search_area.tl.y
                , boundary_color
            );
            // draw neighbors
            for(auto neighbor : neighbors) {
                const auto &item = position_getter(*neighbor);
                DrawCircleLines(item.x, item.y, 5, BLUE);
            }
        }

        // draw quadtree
        QuadtreeRaylibDrawing::drawQuadTree(root.get(), position_getter);
        // QuadtreeRaylibDrawing::print(root.get());
        // QuadtreeRaylibDrawing::drawQuadTreeToMatrix(root.get(), position_getter);

        // draw fps;
        std::string fps_string = "FPS = "+std::to_string(GetFPS());
        DrawText(fps_string.c_str(), 20, 20, 30, LIGHTGRAY);

        EndDrawing();
    }

    // Cleanup
    for(auto* boid : data) {
        delete boid;
    }
    
    CloseWindow();
    return 0;
}