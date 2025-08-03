// Assuming you have a simple Point struct
struct Point {
    float x, y;
    Point(float x = 0, float y = 0) : x(x), y(y) {}
    Point operator+(const Point& other) const { return Point(x + other.x, y + other.y); }
    Point operator/(float val) const { return Point(x / val, y / val); }
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

// Simple Rectangle for boundaries
struct Rectangle {
    Point tl, br;  // top-left, bottom-right
    Rectangle() : tl(0, 0), br(0, 0) {}
    Rectangle(Point tl, Point br) : tl(tl), br(br) {}
    bool contains(const Point& p) const {
        return p.x >= tl.x && p.x <= br.x && p.y >= tl.y && p.y <= br.y;
    }
    bool intersects(const Rectangle& other) const {
        return !(br.x < other.tl.x || tl.x > other.br.x || br.y < other.tl.y || tl.y > other.br.y);
    }
};

// Simple test object
struct TestObject {
    Point position;
    int id;
    TestObject(float x, float y, int id) : position(x, y), id(id) {}
    bool operator==(const TestObject& other) const { return id == other.id; }
};