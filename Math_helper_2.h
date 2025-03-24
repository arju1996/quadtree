//
// Created by sreelatha on 15/7/24.
//

#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#define  PI 3.1415

#include <cmath>
#include <algorithm>
// ugly
#include <iostream>
// ugly ends here
namespace Math {

    // struct Vector2;

    struct Point2i {
        int x, y;
        Point2i() = default;
        Point2i(int x, int y) : x(x), y(y){

        }
        // Point2i(Vector2 vector) : x(vector.x), y(vector.y) {
        // }
        Point2i operator+(const Point2i& other) const {
            return {x+other.x, y+other.y};
        }

        Point2i operator-(const Point2i& other) const {
            return {x-other.x, y-other.y};
        }

        Point2i operator/(float div) const {
            return {x/div, y/div};
        }
        
        friend std::ostream& operator<<(std::ostream& os, const Point2i& point) {
        os << "(" << point.x << ", " << point.y << ")";
        return os;
        }
    };
    struct Point2f {
        float x, y;
    };
    template<typename T>
    struct Point2 {
        T x, y;
    };

    struct Recti {
        Point2i tl;
        Point2i br;
        friend std::ostream& operator<<(std::ostream& os, const Recti& rect) {
        os << "Top-left: " << rect.tl << ", Bottom-right: " << rect.br;
        return os;
        }

        bool contains(Point2i point) {
            if((point.x >= tl.x && point.x <= br.x)
                && (point.x >= tl.x && point.x <= br.x)) {
                    return true;
            } else return false;
        }
    };
    //
    struct Vector2 {
        float x, y;

        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {
        }
        Vector2(Point2i point) : x(point.x), y(point.y) {
        }

        Vector2 operator+(const Vector2& other) const {
            return {x+other.x, y+other.y};
        }
        Vector2 operator-(const Vector2& other) const {
            return {x-other.x, y-other.y};
        }

        Vector2 operator*(const Vector2& other) const {
            return {x*other.x, y*other.y};
        }
        Vector2 operator/(const Vector2& other) const {
            return {x/other.x, y/other.y};
        }
        Vector2 operator*(const float scale) const {
            return {x*scale, y*scale};
        }
        Vector2 operator/(const float scale) const {
            return {x/scale, y/scale};
        }


        void normalize() {
            float magnitude = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
            if (magnitude != 0 && magnitude > 1) {
            // if (magnitude != 0 ) {
                x /= magnitude;
                y /= magnitude;
            }
        }

        void limitMagnitude(float limit) {
            float magnitude = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
            if(magnitude > limit) {
                x = x * limit / magnitude;
                y = y * limit / magnitude;
            }
        }

        void minMagnitude(float limit) {
            float magnitude = std::sqrt(std::pow(x, 2) + std::pow(y, 2));
            if(magnitude < limit) {
                x = x * limit / magnitude;
                y = y * limit / magnitude;
            }
        }

        void setMagnitude(float target_mag) {
            float mag = getMagnitude();
            x = x * target_mag / mag;
            y = y * target_mag / mag;
        }

        // this is awesome but im not able to derive this equation on my own
        // http://www.sunshine2k.de/articles/coding/vectorreflection/vectorreflection.html
        void reflect(const Vector2& normal) {
            // Assuming normal vector is already normalized
            float dot_product = 2.0f * (x * normal.x + y * normal.y);
            x = x - dot_product * normal.x;
            y = y - dot_product * normal.y;
        }

        float Distance(const Vector2 &other) const {
            // distance between this and other
            // this->position_, other.position_
            return sqrt(
                (x - other.x) *
                (x - other.x) +
                (y - other.y) *
                (y - other.y)
            );
        }

        Vector2 GetPerpendicularVector() const {
            return {y, -x};
        }

        std::pair<Vector2, Vector2> GetPerpendicularVectors() const {
            return {{y, -x}, {-y, x}};
        }

        float getMagnitude() const {
            return sqrt(x*x + y*y);
        }

        float angleDegree(Math::Vector2 vector) const {
            const float EPSILON = 1e-6; // Define EPSILON as a small positive number

            float mag1 = this->getMagnitude();
            float mag2 = vector.getMagnitude();

            if (mag1 < EPSILON || mag2 < EPSILON) {
                return 0.0f; // Default angle if magnitudes are zero or too small
            }

            float costheta = (x * vector.x + y * vector.y) /
                             (mag1 * mag2);
            costheta = std::clamp(costheta, -1.0f, 1.0f);
            float angle  = acos(costheta);
            return angle * (180.0 / PI);
        }

        // w.r.to x axis
        float angleRadian() const {
            return atan2(y, x);
        }
    };
}

#endif //MATH_HELPER_H
