#include <iostream>
#include <vector>
#include <cassert>
#include <memory>
#include <algorithm>
#include <cmath>

#include "quad.h"


#include "test_helper.h"

// Helper function to calculate distance
float distance(const Point& a, const Point& b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Helper function to check if vector contains object
bool containsObject(const std::vector<TestObject*>& vec, TestObject* obj) {
    return std::find(vec.begin(), vec.end(), obj) != vec.end();
}

// Helper function to get all objects in tree (for verification)
std::vector<TestObject*> getAllObjectsInTree(QuadTreeNode<TestObject, Point, Rectangle>* node) {
    std::vector<TestObject*> result;
    if (!node) return result;
    
    // Add objects from current node
    for (auto* obj : node->getPoints()) {
        result.push_back(obj);
    }
    
    // Add objects from children if not leaf
    if (!node->getisleaf()) {
        auto tl = getAllObjectsInTree(node->getTLChild().get());
        auto tr = getAllObjectsInTree(node->getTRChild().get());
        auto bl = getAllObjectsInTree(node->getBLChild().get());
        auto br = getAllObjectsInTree(node->getBRChild().get());
        
        result.insert(result.end(), tl.begin(), tl.end());
        result.insert(result.end(), tr.begin(), tr.end());
        result.insert(result.end(), bl.begin(), bl.end());
        result.insert(result.end(), br.begin(), br.end());
    }
    
    return result;
}

void testFindNeighborsInLeafNode() {
    std::cout << "Testing find neighbors in leaf node..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    // Insert objects in same leaf
    TestObject obj1(10, 10, 1);
    TestObject obj2(20, 20, 2);
    TestObject obj3(30, 30, 3);
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    
    assert(tree->getisleaf() == true);
    
    // Find neighbors of a point in the same leaf
    Point queryPoint(15, 15);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // Should find all objects in the leaf
    assert(neighbors.size() == 3);
    assert(containsObject(neighbors, &obj1));
    assert(containsObject(neighbors, &obj2));
    assert(containsObject(neighbors, &obj3));
    
    std::cout << "✓ Find neighbors in leaf node passed" << std::endl;
}

void testFindNeighborsInSubdividedTree() {
    std::cout << "Testing find neighbors in subdivided tree..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 2, 3);
    
    // Insert objects to trigger subdivision
    TestObject obj1(25, 25, 1);  // Top-left
    TestObject obj2(75, 25, 2);  // Top-right
    TestObject obj3(25, 75, 3);  // Bottom-left
    TestObject obj4(75, 75, 4);  // Bottom-right
    TestObject obj5(10, 10, 5);  // Another top-left
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    quadtree.insert(tree, &obj4);
    quadtree.insert(tree, &obj5);
    
    assert(tree->getisleaf() == false);
    
    // Query point in top-left quadrant
    Point queryPoint(20, 20);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // Should find objects in the same quadrant
    assert(neighbors.size() == 2);
    assert(containsObject(neighbors, &obj1));
    assert(containsObject(neighbors, &obj5));
    
    std::cout << "✓ Find neighbors in subdivided tree passed" << std::endl;
}

void testFindNeighborsAtBoundary() {
    std::cout << "Testing find neighbors at boundary..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 2, 3);
    
    // Insert objects near the center (boundary of quadrants)
    TestObject obj1(49, 49, 1);  // Top-left, near center
    TestObject obj2(51, 49, 2);  // Top-right, near center
    TestObject obj3(49, 51, 3);  // Bottom-left, near center
    TestObject obj4(51, 51, 4);  // Bottom-right, near center
    TestObject obj5(25, 25, 5);  // Far in top-left
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    quadtree.insert(tree, &obj4);
    quadtree.insert(tree, &obj5);
    
    assert(tree->getisleaf() == false);
    
    // Query at exact center
    Point queryPoint(50, 50);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // The exact behavior depends on your implementation
    // But it should find neighbors in the relevant quadrant(s)
    assert(neighbors.size() >= 1);
    
    std::cout << "✓ Find neighbors at boundary passed" << std::endl;
}

void testFindNeighborsEmptyTree() {
    std::cout << "Testing find neighbors in empty tree..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    // Query in empty tree
    Point queryPoint(50, 50);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    assert(neighbors.size() == 0);
    
    std::cout << "✓ Find neighbors in empty tree passed" << std::endl;
}

void testFindNeighborsOutsideBoundary() {
    std::cout << "Testing find neighbors outside boundary..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    // Insert objects within boundary
    TestObject obj1(10, 10, 1);
    TestObject obj2(90, 90, 2);
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    
    // Query outside boundary
    Point queryPoint(150, 150);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // Should return empty or handle gracefully
    assert(neighbors.size() == 0);
    
    std::cout << "✓ Find neighbors outside boundary passed" << std::endl;
}

void testFindNeighborsWithClusteredObjects() {
    std::cout << "Testing find neighbors with clustered objects..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 3, 3);
    
    // Insert clustered objects
    std::vector<TestObject> cluster1;
    cluster1.reserve(5);
    for (int i = 0; i < 5; ++i) {
        cluster1.emplace_back(10 + i, 10 + i, i);
        quadtree.insert(tree, &cluster1.back());
    }
    
    std::vector<TestObject> cluster2;
    cluster2.reserve(5);
    for (int i = 0; i < 5; ++i) {
        cluster2.emplace_back(80 + i, 80 + i, i + 100);
        quadtree.insert(tree, &cluster2.back());
    }
    
    // Query near first cluster
    Point queryPoint(12, 12);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // Should find objects in the same region
    assert(neighbors.size() > 0);
    
    // Verify that found neighbors are actually from the nearby cluster
    for (auto* neighbor : neighbors) {
        assert(distance(queryPoint, neighbor->position) < distance(queryPoint, Point(82, 82)));
    }
    
    std::cout << "✓ Find neighbors with clustered objects passed" << std::endl;
}

void testFindNeighborsAtCorners() {
    std::cout << "Testing find neighbors at corners..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 2, 3);
    
    // Insert objects at corners
    TestObject obj1(0, 0, 1);      // Top-left corner
    TestObject obj2(100, 0, 2);    // Top-right corner
    TestObject obj3(0, 100, 3);    // Bottom-left corner
    TestObject obj4(100, 100, 4);  // Bottom-right corner
    TestObject obj5(50, 50, 5);    // Center
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    quadtree.insert(tree, &obj4);
    quadtree.insert(tree, &obj5);
    
    // Query at top-left corner
    Point queryPoint(0, 0);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    assert(neighbors.size() >= 1);
    assert(containsObject(neighbors, &obj1));
    
    std::cout << "✓ Find neighbors at corners passed" << std::endl;
}

void testFindNeighborsMultipleQueries() {
    std::cout << "Testing find neighbors with multiple queries..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 2, 3);
    
    // Insert objects in different quadrants
    TestObject obj1(25, 25, 1);  // Top-left
    TestObject obj2(75, 25, 2);  // Top-right
    TestObject obj3(25, 75, 3);  // Bottom-left
    TestObject obj4(75, 75, 4);  // Bottom-right
    TestObject obj5(10, 10, 5);  // Another top-left
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    quadtree.insert(tree, &obj4);
    quadtree.insert(tree, &obj5);
    
    // Multiple queries in different areas
    std::vector<Point> queryPoints = {
        Point(20, 20),  // Top-left
        Point(80, 20),  // Top-right
        Point(20, 80),  // Bottom-left
        Point(80, 80)   // Bottom-right
    };
    
    for (const auto& queryPoint : queryPoints) {
        auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
        assert(neighbors.size() >= 1);
        
        // Verify neighbors are in expected quadrant
        for (auto* neighbor : neighbors) {
            Point pos = neighbor->position;
            if (queryPoint.x < 50 && queryPoint.y < 50) {
                // Top-left query should find top-left objects
                assert(pos.x <= 50 && pos.y <= 50);
            } else if (queryPoint.x > 50 && queryPoint.y < 50) {
                // Top-right query should find top-right objects
                assert(pos.x >= 50 && pos.y <= 50);
            } else if (queryPoint.x < 50 && queryPoint.y > 50) {
                // Bottom-left query should find bottom-left objects
                assert(pos.x <= 50 && pos.y >= 50);
            } else if (queryPoint.x > 50 && queryPoint.y > 50) {
                // Bottom-right query should find bottom-right objects
                assert(pos.x >= 50 && pos.y >= 50);
            }
        }
    }
    
    std::cout << "✓ Find neighbors with multiple queries passed" << std::endl;
}

void testFindNeighborsDeepTree() {
    std::cout << "Testing find neighbors in deep tree..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 1, 5); // Small capacity, deep tree
    
    // Insert many objects in a small area to create deep nesting
    std::vector<TestObject> objects;
    for (int i = 0; i < 10; ++i) {
        objects.emplace_back(10 + i * 0.5f, 10 + i * 0.5f, i);
        quadtree.insert(tree, &objects.back());
    }
    
    // Add some objects far away
    TestObject farObj1(90, 90, 100);
    TestObject farObj2(95, 95, 101);
    quadtree.insert(tree, &farObj1);
    quadtree.insert(tree, &farObj2);
    
    assert(tree->getisleaf() == false);
    
    // Query near the clustered objects
    Point queryPoint(12, 12);
    auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // Should find local neighbors, not distant ones
    assert(neighbors.size() > 0);
    
    // Verify that we don't get the far objects
    assert(!containsObject(neighbors, &farObj1));
    assert(!containsObject(neighbors, &farObj2));
    
    std::cout << "✓ Find neighbors in deep tree passed" << std::endl;
}

void testFindNeighborsPerformance() {
    std::cout << "Testing find neighbors performance with many objects..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(1000, 1000));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 10, 4);
    
    // Insert many objects across the space
    std::vector<TestObject> objects;
    objects.reserve(100);
    for (int i = 0; i < 100; ++i) {
        float x = (i % 10) * 100 + 50;
        float y = (i / 10) * 100 + 50;
        objects.emplace_back(x, y, i);
        quadtree.insert(tree, &objects.back());
    }
    
    // Perform multiple queries
    std::vector<Point> queryPoints = {
        Point(150, 150),
        Point(550, 550),
        Point(850, 850),
        Point(250, 750)
    };
    
    for (const auto& queryPoint : queryPoints) {
        auto neighbors = quadtree.findNeighbors(tree.get(), queryPoint);
        
        // Should find some neighbors but not all objects
        assert(neighbors.size() > 0);
        assert(neighbors.size() < 100); // Should be more efficient than linear search
        
        // Verify found neighbors are reasonable
        for (auto* neighbor : neighbors) {
            float dist = distance(queryPoint, neighbor->position);
            assert(dist < 500); // Should be reasonably close
        }
    }
    
    std::cout << "✓ Find neighbors performance test passed" << std::endl;
}

void testFindNeighborsConsistency() {
    std::cout << "Testing find neighbors consistency..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 3, 3);
    
    // Insert objects
    std::vector<TestObject> objects;
    for (int i = 0; i < 20; ++i) {
        objects.emplace_back(i * 5, i * 5, i);
        quadtree.insert(tree, &objects.back());
    }
    
    // Query same point multiple times
    Point queryPoint(50, 50);
    auto neighbors1 = quadtree.findNeighbors(tree.get(), queryPoint);
    auto neighbors2 = quadtree.findNeighbors(tree.get(), queryPoint);
    auto neighbors3 = quadtree.findNeighbors(tree.get(), queryPoint);
    
    // Results should be consistent
    assert(neighbors1.size() == neighbors2.size());
    assert(neighbors2.size() == neighbors3.size());
    
    // Same objects should be found
    for (auto* obj : neighbors1) {
        assert(containsObject(neighbors2, obj));
        assert(containsObject(neighbors3, obj));
    }
    
    std::cout << "✓ Find neighbors consistency test passed" << std::endl;
}

void runAllFindNeighborsTests() {
    std::cout << "Running QuadTree FindNeighbors Tests..." << std::endl;
    std::cout << "=======================================" << std::endl;
    
    try {
        testFindNeighborsInLeafNode();
        testFindNeighborsInSubdividedTree();
        testFindNeighborsAtBoundary();
        testFindNeighborsEmptyTree();
        testFindNeighborsOutsideBoundary();
        testFindNeighborsWithClusteredObjects();
        testFindNeighborsAtCorners();
        testFindNeighborsMultipleQueries();
        testFindNeighborsDeepTree();
        testFindNeighborsPerformance();
        testFindNeighborsConsistency();
        
        std::cout << "\n🎉 All findNeighbors tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n❌ FindNeighbors test failed: " << e.what() << std::endl;
    }
}

// int main() {
//     runAllFindNeighborsTests();
//     return 0;
// }