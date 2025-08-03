#include <iostream>

// Declarations of all test functions
void runAllRemoveTests();
void runAllFindNeighborsTests();
void runAllCreationTests();

int main() {
    std::cout << "Running QuadTree Tests...\n";
    std::cout << "=========================\n";

    try {
        runAllCreationTests();
        runAllRemoveTests();
        runAllFindNeighborsTests();

        std::cout << "\n🎉 All tests passed!\n";
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << "\n";
    }

    return 0;
}