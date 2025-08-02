#pragma once

#include "quad.h"
#include <raylib.h> 

// #include <stack>

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>

namespace QuadtreeRaylibDrawing {
    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void drawQuadTree(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, typename QuadTreeNode<OBJECT_NAME, PointType, BoundType>::PositionGetter position_getter) {
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


    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void print(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root) {
        if(root == nullptr)
            return;
        else {
            std::cout<<"boundry:"<<root->getBoundry()<<"\n";
            std::cout<<"points_:"<<root->getPoints().size()<<"\n";
            std::cout<<"isLeaf_:"<<root->getisleaf()<<"\n";
            print(root->getTLChild().get());
            print(root->getTRChild().get());
            print(root->getBLChild().get());
            print(root->getBRChild().get());
        }
    }

    // AI CODE BELOW,  DONT SPEND TOO MUCH TIME
    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void drawQuadTree2(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, typename QuadTreeNode<OBJECT_NAME, PointType, BoundType>::PositionGetter position_getter) {
        const Color boundary_color {230, 41, 55, 75};
        const Color point_color {GREEN};
        
        if(root == nullptr)
            return;
        
        // Use a stack to simulate the recursive call stack
        std::stack<QuadTreeNode<OBJECT_NAME, PointType, BoundType>*> nodeStack;
        nodeStack.push(root);
        
        while(!nodeStack.empty()) {
            QuadTreeNode<OBJECT_NAME, PointType, BoundType>* current = nodeStack.top();
            nodeStack.pop();
            
            if(current == nullptr)
                continue;
            
            // Draw the boundary for current node
            const BoundType& boundry = current->getBoundry();
            DrawRectangleLines(
                boundry.tl.x,
                boundry.tl.y,
                boundry.br.x - boundry.tl.x,
                boundry.br.y - boundry.tl.y,
                boundary_color
            );
            
            // Draw all points in current node
            for(auto node : current->getPoints()) {
                const PointType& item = position_getter(*node);
                DrawCircle(item.x, item.y, 2, point_color);
            }
            
            // Push children onto stack (in reverse order to maintain same traversal order)
            if(current->getBRChild().get() != nullptr)
                nodeStack.push(current->getBRChild().get());
            if(current->getBLChild().get() != nullptr)
                nodeStack.push(current->getBLChild().get());
            if(current->getTRChild().get() != nullptr)
                nodeStack.push(current->getTRChild().get());
            if(current->getTLChild().get() != nullptr)
                nodeStack.push(current->getTLChild().get());
        }
    }


    // ADDED WITH THE SECOND PHASE OF DEBUGGING THE DUPLICATE DATA ISSUE;
    // Option 1: Draw to a 2D character matrix
    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void drawQuadTreeToMatrix(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, 
                            typename QuadTreeNode<OBJECT_NAME, PointType, BoundType>::PositionGetter position_getter,
                            int width = 80, int height = 40) {
        
        if(root == nullptr) return;
        
        // Create a 2D matrix filled with spaces
        std::vector<std::vector<char>> matrix(height, std::vector<char>(width, ' '));
        
        // Find bounds of the entire quadtree
        const BoundType& rootBound = root->getBoundry();
        float minX = rootBound.tl.x;
        float maxX = rootBound.br.x;
        float minY = rootBound.tl.y;
        float maxY = rootBound.br.y;
        
        std::stack<QuadTreeNode<OBJECT_NAME, PointType, BoundType>*> nodeStack;
        nodeStack.push(root);
        
        while(!nodeStack.empty()) {
            auto* current = nodeStack.top();
            nodeStack.pop();
            
            if(current == nullptr) continue;
            
            const BoundType& boundry = current->getBoundry();
            
            // Convert world coordinates to matrix coordinates
            int x1 = (int)((boundry.tl.x - minX) / (maxX - minX) * (width - 1));
            int y1 = (int)((boundry.tl.y - minY) / (maxY - minY) * (height - 1));
            int x2 = (int)((boundry.br.x - minX) / (maxX - minX) * (width - 1));
            int y2 = (int)((boundry.br.y - minY) / (maxY - minY) * (height - 1));
            
            // Ensure bounds are within matrix
            x1 = std::max(0, std::min(width - 1, x1));
            y1 = std::max(0, std::min(height - 1, y1));
            x2 = std::max(0, std::min(width - 1, x2));
            y2 = std::max(0, std::min(height - 1, y2));
            
            // Draw rectangle boundaries
            for(int x = x1; x <= x2; x++) {
                if(y1 < height) matrix[y1][x] = '-';
                if(y2 < height) matrix[y2][x] = '-';
            }
            for(int y = y1; y <= y2; y++) {
                if(x1 < width) matrix[y][x1] = '|';
                if(x2 < width) matrix[y][x2] = '|';
            }
            
            // Draw corner characters
            if(y1 < height && x1 < width) matrix[y1][x1] = '+';
            if(y1 < height && x2 < width) matrix[y1][x2] = '+';
            if(y2 < height && x1 < width) matrix[y2][x1] = '+';
            if(y2 < height && x2 < width) matrix[y2][x2] = '+';
            
            // Draw points
            for(auto node : current->getPoints()) {
                const PointType& item = position_getter(*node);
                int px = (int)((item.x - minX) / (maxX - minX) * (width - 1));
                int py = (int)((item.y - minY) / (maxY - minY) * (height - 1));
                
                px = std::max(0, std::min(width - 1, px));
                py = std::max(0, std::min(height - 1, py));
                
                matrix[py][px] = 'X';
            }
            
            // Push children
            if(current->getBRChild().get() != nullptr)
                nodeStack.push(current->getBRChild().get());
            if(current->getBLChild().get() != nullptr)
                nodeStack.push(current->getBLChild().get());
            if(current->getTRChild().get() != nullptr)
                nodeStack.push(current->getTRChild().get());
            if(current->getTLChild().get() != nullptr)
                nodeStack.push(current->getTLChild().get());
        }
        
        // Print to console
        for(const auto& row : matrix) {
            for(char c : row) {
                std::cout << c;
            }
            std::cout << '\n';
        }
    }

    // Option 2: Save to file in a simple format
    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void saveQuadTreeToFile(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, 
                        typename QuadTreeNode<OBJECT_NAME, PointType, BoundType>::PositionGetter position_getter,
                        const std::string& filename) {
        
        std::ofstream file(filename);
        if(!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }
        
        if(root == nullptr) {
            file << "Empty quadtree\n";
            return;
        }
        
        std::stack<std::pair<QuadTreeNode<OBJECT_NAME, PointType, BoundType>*, int>> nodeStack;
        nodeStack.push({root, 0});
        
        while(!nodeStack.empty()) {
            auto [current, depth] = nodeStack.top();
            nodeStack.pop();
            
            if(current == nullptr) continue;
            
            // Indentation for tree structure
            std::string indent(depth * 2, ' ');
            
            const BoundType& boundry = current->getBoundry();
            file << indent << "Node [" << boundry.tl.x << "," << boundry.tl.y 
                << " to " << boundry.br.x << "," << boundry.br.y << "]\n";
            
            // Print points in this node
            for(auto node : current->getPoints()) {
                const PointType& item = position_getter(*node);
                file << indent << "  Point: (" << item.x << ", " << item.y << ")\n";
            }
            
            // Push children with increased depth
            if(current->getBRChild().get() != nullptr)
                nodeStack.push({current->getBRChild().get(), depth + 1});
            if(current->getBLChild().get() != nullptr)
                nodeStack.push({current->getBLChild().get(), depth + 1});
            if(current->getTRChild().get() != nullptr)
                nodeStack.push({current->getTRChild().get(), depth + 1});
            if(current->getTLChild().get() != nullptr)
                nodeStack.push({current->getTLChild().get(), depth + 1});
        }
        
        file.close();
        std::cout << "QuadTree saved to " << filename << std::endl;
    }

    // Option 3: Simple text tree structure
    template <typename OBJECT_NAME, typename PointType, typename BoundType>
    void printQuadTreeStructure(QuadTreeNode<OBJECT_NAME, PointType, BoundType>* root, 
                            typename QuadTreeNode<OBJECT_NAME, PointType, BoundType>::PositionGetter position_getter) {
        
        if(root == nullptr) {
            std::cout << "Empty quadtree\n";
            return;
        }
        
        std::stack<std::pair<QuadTreeNode<OBJECT_NAME, PointType, BoundType>*, std::string>> nodeStack;
        nodeStack.push({root, ""});
        
        while(!nodeStack.empty()) {
            auto [current, prefix] = nodeStack.top();
            nodeStack.pop();
            
            if(current == nullptr) continue;
            
            const BoundType& boundry = current->getBoundry();
            std::cout << prefix << "├─ [" << boundry.tl.x << "," << boundry.tl.y 
                    << " to " << boundry.br.x << "," << boundry.br.y << "] ";
            
            // Show point count
            auto points = current->getPoints();
            std::cout << "(" << points.size() << " points)\n";
            
            // Print points
            for(auto node : points) {
                const PointType& item = position_getter(*node);
                std::cout << prefix << "│  • (" << item.x << ", " << item.y << ")\n";
            }
            
            // Push children
            std::string childPrefix = prefix + "│  ";
            if(current->getBRChild().get() != nullptr)
                nodeStack.push({current->getBRChild().get(), childPrefix});
            if(current->getBLChild().get() != nullptr)
                nodeStack.push({current->getBLChild().get(), childPrefix});
            if(current->getTRChild().get() != nullptr)
                nodeStack.push({current->getTRChild().get(), childPrefix});
            if(current->getTLChild().get() != nullptr)
                nodeStack.push({current->getTLChild().get(), childPrefix});
        }
    }
}