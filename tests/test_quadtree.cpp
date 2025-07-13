#include <iostream>
#include <vector>
#include <cassert>
#include <memory>


#include "quad.h"

// Assuming you have a simple Point struct
struct Point {
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
    Point operator+(const Point& other) const { return Point(x + other.x, y + other.y); }
    Point operator/(float val) const { return Point(x / val, y / val); }
};

// Simple Rectangle for boundaries
struct Rectangle {
    Point tl, br;  // top-left, bottom-right
    Rectangle() : tl(0, 0), br(0, 0) {}
    Rectangle(Point tl, Point br) : tl(tl), br(br) {}
    bool contains(const Point& p) const {
        return p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y;
    }
};

// Simple test object
struct TestObject {
    Point position;
    int id;
    TestObject(float x, float y, int id) : position(x, y), id(id) {}
};

// Test functions
void testBasicInsertion() {
    std::cout << "Testing basic insertion..." << std::endl;
    
    // Create maker and position getter
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    // Create tree with capacity 2
    Rectangle boundary(Point(0, 0), Point(100, 100));
    auto tree = quadtree.BuildTree({}, positionGetter, boundary, 2, 3);
    
    // Test 1: Insert single object
    TestObject obj1(10, 10, 1);
    quadtree.insert(tree, &obj1);
    assert(tree->getPoints().size() == 1);
    assert(tree->getisleaf() == true);
    std::cout << "✓ Single insertion passed" << std::endl;
    
    // Test 2: Insert within capacity
    TestObject obj2(20, 20, 2);
    quadtree.insert(tree, &obj2);
    std::cout << "tree->getPoints().size() = "<<tree->getPoints().size()<< std::endl;
    std::cout << "tree->getisleaf() = "<<tree->getisleaf()<< std::endl;
    assert(tree->getPoints().size() == 2);
    assert(tree->getisleaf() == true);
    std::cout << "✓ Within capacity insertion passed" << std::endl;
}

void testSubdivision() {
    std::cout << "Testing subdivision..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    auto tree = quadtree.BuildTree({}, positionGetter, boundary, 2, 3);
    
    // Insert objects to trigger subdivision
    TestObject obj1(10, 10, 1);  // Top-left
    TestObject obj2(20, 20, 2);  // Top-left
    TestObject obj3(30, 30, 3);  // Top-left (should trigger subdivision)
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    
    assert(tree->getisleaf() == false);
    assert(tree->getTLChild() != nullptr);
    assert(tree->getTRChild() != nullptr);
    assert(tree->getBLChild() != nullptr);
    assert(tree->getBRChild() != nullptr);
    
    std::cout << "✓ Subdivision triggered correctly" << std::endl;
}

void testDistributedInsertion() {
    std::cout << "Testing distributed insertion..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    auto tree = quadtree.BuildTree({}, positionGetter, boundary, 2, 3);
    
    // Insert objects in different quadrants
    TestObject obj1(25, 25, 1);  // Top-left
    TestObject obj2(75, 25, 2);  // Top-right
    TestObject obj3(25, 75, 3);  // Bottom-left
    TestObject obj4(75, 75, 4);  // Bottom-right
    TestObject obj5(10, 10, 5);  // Another top-left (should trigger subdivision)
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    quadtree.insert(tree, &obj4);
    quadtree.insert(tree, &obj5);
    
    assert(tree->getisleaf() == false);
    
    // Check that objects are distributed correctly
    std::cout << "tree->getTLChild()->getPoints().size() = "<<tree->getTLChild()->getPoints().size() << std::endl;
    assert(tree->getTLChild()->getPoints().size() == 2);  // obj1 and obj5
    assert(tree->getTRChild()->getPoints().size() == 1);  // obj2
    assert(tree->getBLChild()->getPoints().size() == 1);  // obj3
    assert(tree->getBRChild()->getPoints().size() == 1);  // obj4
    
    std::cout << "✓ Objects distributed correctly across quadrants" << std::endl;
}

void testClusteredPoints() {
    std::cout << "Testing clustered points (your original concern)..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    // auto tree = quadtree.BuildTree({}, positionGetter, boundary, 2, 3);
    auto tree = quadtree.BuildTree({}, positionGetter, boundary, 7, 3);
    
    // Insert many objects in same area
    std::vector<TestObject> objects;
    for (int i = 0; i < 10; ++i) {
        objects.emplace_back(10 + i, 10 + i, i);  // All in top-left area
        quadtree.insert(tree, &objects.back());
    }
    
    // Should have subdivided recursively
    assert(tree->getisleaf() == false);
    
    // Check that subdivision happened in top-left
    assert(tree->getTLChild()->getisleaf() == false);
    
    std::cout << "✓ Clustered points handled with recursive subdivision" << std::endl;
}

void testEdgeCases() {
    std::cout << "Testing edge cases..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    auto tree = quadtree.BuildTree({}, positionGetter, boundary, 2, 3);
    
    // Test boundary points
    TestObject obj1(0, 0, 1);    // Corner
    TestObject obj2(50, 50, 2);  // Center
    TestObject obj3(100, 100, 3); // Other corner
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    
    // Should trigger subdivision
    assert(tree->getisleaf() == false);
    
    std::cout << "✓ Boundary points handled correctly" << std::endl;
}

void runAllTests() {
    std::cout << "Running QuadTree Tests..." << std::endl;
    std::cout << "=========================" << std::endl;
    
    try {
        testBasicInsertion();
        testSubdivision();
        testDistributedInsertion();
        testClusteredPoints();
        testEdgeCases();
        
        std::cout << "\n🎉 All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << std::endl;
    }
}

int main() {
    runAllTests();
    return 0;
}