#include <iostream>
#include <vector>
#include <cassert>
#include <memory>
#include <algorithm>

#include "quad.h"

#include "test_helper.h"


// Helper function to count total objects in tree
int countObjectsInTree(QuadTreeNode<TestObject, Point, Rectangle>* node) {
    if (!node) return 0;
    
    int count = node->getPoints().size();
    if (!node->getisleaf()) {
        count += countObjectsInTree(node->getTLChild().get());
        count += countObjectsInTree(node->getTRChild().get());
        count += countObjectsInTree(node->getBLChild().get());
        count += countObjectsInTree(node->getBRChild().get());
    }
    return count;
}

// Helper function to find object in tree
bool findObjectInTree(QuadTreeNode<TestObject, Point, Rectangle>* node, TestObject* target) {
    if (!node) return false;
    
    // Check current node
    for (auto* obj : node->getPoints()) {
        if (obj == target) return true;
    }
    
    // Check children if not leaf
    if (!node->getisleaf()) {
        return findObjectInTree(node->getTLChild().get(), target) ||
               findObjectInTree(node->getTRChild().get(), target) ||
               findObjectInTree(node->getBLChild().get(), target) ||
               findObjectInTree(node->getBRChild().get(), target);
    }
    
    return false;
}

void testRemoveFromLeafNode() {
    std::cout << "Testing remove from leaf node..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    // Insert objects
    TestObject obj1(10, 10, 1);
    TestObject obj2(20, 20, 2);
    TestObject obj3(30, 30, 3);
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    quadtree.insert(tree, &obj3);
    
    assert(tree->getPoints().size() == 3);
    assert(tree->getisleaf() == true);
    
    // Remove middle object
    quadtree.remove(tree, &obj2);
    
    assert(tree->getPoints().size() == 2);
    assert(!findObjectInTree(tree.get(), &obj2));
    assert(findObjectInTree(tree.get(), &obj1));
    assert(findObjectInTree(tree.get(), &obj3));
    
    std::cout << "✓ Remove from leaf node passed" << std::endl;
}

void testRemoveLastObjectFromLeaf() {
    std::cout << "Testing remove last object from leaf..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    // Insert single object
    TestObject obj1(10, 10, 1);
    quadtree.insert(tree, &obj1);
    
    assert(tree->getPoints().size() == 1);
    assert(tree->getisleaf() == true);
    
    // Remove the only object
    quadtree.remove(tree, &obj1);
    
    assert(tree->getPoints().size() == 0);
    assert(tree->getisleaf() == true);
    
    std::cout << "✓ Remove last object from leaf passed" << std::endl;
}

void testRemoveFromSubdividedTree() {
    std::cout << "Testing remove from subdivided tree..." << std::endl;
    
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
    int initialCount = countObjectsInTree(tree.get());
    assert(initialCount == 5);
    
    // Remove object from top-right quadrant
    quadtree.remove(tree, &obj2);
    
    assert(countObjectsInTree(tree.get()) == 4);
    assert(!findObjectInTree(tree.get(), &obj2));
    assert(findObjectInTree(tree.get(), &obj1));
    assert(findObjectInTree(tree.get(), &obj3));
    assert(findObjectInTree(tree.get(), &obj4));
    assert(findObjectInTree(tree.get(), &obj5));
    
    std::cout << "✓ Remove from subdivided tree passed" << std::endl;
}

void testRemoveNonExistentObject() {
    std::cout << "Testing remove non-existent object..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    // Insert objects
    TestObject obj1(10, 10, 1);
    TestObject obj2(20, 20, 2);
    TestObject nonExistent(99, 99, 999);
    
    quadtree.insert(tree, &obj1);
    quadtree.insert(tree, &obj2);
    
    int initialCount = countObjectsInTree(tree.get());
    assert(initialCount == 2);
    
    // Try to remove non-existent object (should not crash or affect tree)
    quadtree.remove(tree, &nonExistent);
    
    assert(countObjectsInTree(tree.get()) == 2);
    assert(findObjectInTree(tree.get(), &obj1));
    assert(findObjectInTree(tree.get(), &obj2));
    
    std::cout << "✓ Remove non-existent object handled gracefully" << std::endl;
}

void testRemoveFromEmptyTree() {
    std::cout << "Testing remove from empty tree..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));

    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 5, 3);
    
    TestObject obj1(10, 10, 1);
    
    assert(countObjectsInTree(tree.get()) == 0);
    
    // Try to remove from empty tree (should not crash)
    quadtree.remove(tree, &obj1);
    
    assert(countObjectsInTree(tree.get()) == 0);
    assert(tree->getisleaf() == true);
    
    std::cout << "✓ Remove from empty tree handled gracefully" << std::endl;
}

void testRemoveMultipleObjects() {
    std::cout << "Testing remove multiple objects..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 3, 3);
    
    // Insert multiple objects
    std::vector<TestObject> objects;
    objects.reserve(10);
    for (int i = 0; i < 10; ++i) {
        objects.emplace_back(10 + i * 5, 10 + i * 5, i);
        quadtree.insert(tree, &objects.back());
    }
    
    assert(countObjectsInTree(tree.get()) == 10);
    
    // Remove every other object
    for (int i = 0; i < 10; i += 2) {
        quadtree.remove(tree, &objects[i]);
    }
    
    assert(countObjectsInTree(tree.get()) == 5);
    
    // Check that correct objects remain
    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0) {
            assert(!findObjectInTree(tree.get(), &objects[i]));
        } else {
            assert(findObjectInTree(tree.get(), &objects[i]));
        }
    }
    
    std::cout << "✓ Remove multiple objects passed" << std::endl;
}

void testRemoveFromDeepTree() {
    std::cout << "Testing remove from deeply nested tree..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 1, 5); // Small capacity, deep tree
    
    // Insert clustered objects to create deep nesting
    std::vector<TestObject> objects;
    objects.reserve(8);
    for (int i = 0; i < 8; ++i) {
        objects.emplace_back(10 + i * 0.5f, 10 + i * 0.5f, i);
        quadtree.insert(tree, &objects.back());
    }
    
    assert(tree->getisleaf() == false);
    int initialCount = countObjectsInTree(tree.get());
    assert(initialCount == 8);
    
    // Remove from deeply nested structure
    quadtree.remove(tree, &objects[3]);
    
    assert(countObjectsInTree(tree.get()) == 7);
    assert(!findObjectInTree(tree.get(), &objects[3]));
    
    std::cout << "✓ Remove from deep tree passed" << std::endl;
}

void testRemoveAllObjectsSequentially() {
    std::cout << "Testing remove all objects sequentially..." << std::endl;
    
    QuadTree<TestObject, Point, Rectangle> quadtree;
    auto positionGetter = [](const TestObject& obj) { return obj.position; };
    
    Rectangle boundary(Point(0, 0), Point(100, 100));
    QuadTreeNode<TestObject, Point, Rectangle>* parent = nullptr;
    auto tree = quadtree.BuildTree(parent, {}, positionGetter, boundary, 2, 3);
    
    // Insert objects
    std::vector<TestObject> objects;
    objects.emplace_back(25, 25, 1);  // Top-left
    objects.emplace_back(75, 25, 2);  // Top-right
    objects.emplace_back(25, 75, 3);  // Bottom-left
    objects.emplace_back(75, 75, 4);  // Bottom-right
    objects.emplace_back(10, 10, 5);  // Another top-left
    
    for (auto& obj : objects) {
        quadtree.insert(tree, &obj);
    }
    
    assert(countObjectsInTree(tree.get()) == 5);
    assert(tree->getisleaf() == false);
    
    // Remove all objects one by one
    for (auto& obj : objects) {
        quadtree.remove(tree, &obj);
    }
    
    assert(countObjectsInTree(tree.get()) == 0);
    // Tree structure may still exist but should be empty
    
    std::cout << "✓ Remove all objects sequentially passed" << std::endl;
}

void runAllRemoveTests() {
    std::cout << "Running QuadTree Remove Tests..." << std::endl;
    std::cout << "================================" << std::endl;
    
    try {
        testRemoveFromLeafNode();
        testRemoveLastObjectFromLeaf();
        testRemoveFromSubdividedTree();
        testRemoveNonExistentObject();
        testRemoveFromEmptyTree();
        testRemoveMultipleObjects();
        testRemoveFromDeepTree();
        testRemoveAllObjectsSequentially();
        
        std::cout << "\n🎉 All remove tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Remove test failed: " << e.what() << std::endl;
    }
}

// int main() {
//     runAllRemoveTests();
//     return 0;
// }