#ifndef PolarEquation_H
#define PolarEquation_H

#include <cmath>
#include <vector>
#include <algorithm>

// Include and alias
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

struct PolarEquation {

    // Used to define the equation
    struct Point {

        float angle;   // Angle in radians
        float radius;  // Radius at this angle
        float i;
        float j;

        Point(float angle, float radius) {
            
            this->angle = angle;
            this->radius = radius;
            this->i = cos(angle);
            this->j = sin(angle);
        }

        Json getState() {

            Json obj;

            obj["angle"] = angle;
            obj["radius"] = radius;
            obj["i"] = i;
            obj["j"] = j;

            return obj;
        }

        void setState(Json& obj) {

            angle = obj["angle"];
            radius = obj["radius"];
            i = obj["i"];
            j = obj["j"];
        }

        // Quick setter function
        set(float angle, float radius) {

            this->angle = angle;
            this->radius = radius;
            this->i = cos(angle);
            this->j = sin(angle);
        }
    };

    std::vector<Point> points;
    bool needsSorting = false;

    // Constructor
    PolarEquation() {

    }
    
    Json getState() {

        Json obj;

        obj["needsSorting"] = needsSorting;

        Json pointsArray = Json::array();

        for (auto& point : points) {
            pointsArray.push_back(point.getState());
        }

        obj["points"] = pointsArray;

        return obj;
    }

    // JSON Deserialization: Load the state of all points and needsSorting flag
    void setState(Json& obj) {

        needsSorting = obj.at("needsSorting").get<bool>();

        points.clear();

        for (auto& pointData : obj.at("points")) {

            Point point(0, 0);
            point.setState(pointData);
            points.push_back(point);
        }
    }

    // Add point in the correct sorted position with angle wrapping
    void addPoint(float angle, float radius) {

        // Find the correct insertion position using binary search
        auto it = std::lower_bound(points.begin(), points.end(), angle, [](const Point& p, float a) {
            return p.angle < a;
        });

        // Insert the point at the found position
        points.emplace(it, angle, radius);
    }

    // Remove a point at a specific angle if it exists
    void removePoint(float angle) {

        // Find the point with the specified angle
        auto it = std::lower_bound(points.begin(), points.end(), angle, [](const Point& p, float a) {
            return p.angle < a;
        });

        // Check if the angle matches (since std::lower_bound finds the first position not less than)
        if (it != points.end() && it->angle == angle) {
            points.erase(it);
        }
    }

    float atAngle(float angle) {

        if (points.empty()) { return 0.0f; }
        if (points.size() == 1) { return points[0].radius; }

        // Create a query point with the given angle to compute its unit vector
        Point query(angle, 1.0f);

        float weightedSum = 0.0f;
        float totalWeight = 0.0f;

        for (const auto& point : points) {

            // Calculate the dot product for similarity
            float cosineSimilarity = query.i * point.i + query.j * point.j;
            float dist = 1.f - cosineSimilarity;
            float weight = 1.f / (dist + .000001f);

            // Accumulate weighted radius and total weight
            weightedSum += weight * point.radius;
            totalWeight += weight;
        }

        // Return the weighted average radius, handling the case where totalWeight is zero
        return (totalWeight > 0.0f) ? (weightedSum / totalWeight) : 0.0f;
    }
};

#endif // PolarEquation_H
