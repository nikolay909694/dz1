/**
 * @file jarvis_hull.h
 * @brief Convex Hull calculation using Jarvis March algorithm
 * @author Your Name
 * @date Creation Date
 */

 #pragma once

 #include <vector>
 #include <algorithm>
 #include <sstream>
 
 /**
  * @struct HullPoint
  * @brief Represents a 2D point with integer coordinates
  */
 struct HullPoint {
     int x; ///< X coordinate of the point
     int y; ///< Y coordinate of the point
 };
 
 /**
  * @brief Determines orientation of three points
  * @param a First reference point
  * @param b Second reference point
  * @param c Third point to check orientation
  * @return 0 if collinear, 1 if clockwise, 2 if counter-clockwise
  */
 inline int checkOrientation(const HullPoint& a, const HullPoint& b, const HullPoint& c) {
     int cross_product = (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
     return (cross_product == 0) ? 0 : ((cross_product > 0) ? 1 : 2);
 }
 
 /**
  * @brief Computes convex hull using Jarvis March algorithm
  * @param point_set Input set of points
  * @return Vector of points forming the convex hull in order
  */
 inline std::vector<HullPoint> computeJarvisHull(const std::vector<HullPoint>& point_set) {
     size_t n = point_set.size();
     if (n < 3) return {};
 
     // Find leftmost point
     size_t leftmost_pos = 0;
     for (size_t i = 1; i < n; ++i) {
         if (point_set[i].x < point_set[leftmost_pos].x || 
            (point_set[i].x == point_set[leftmost_pos].x && 
             point_set[i].y < point_set[leftmost_pos].y)) {
             leftmost_pos = i;
         }
     }
 
     std::vector<HullPoint> hull_points;
     size_t current_pos = leftmost_pos, next_pos;
 
     do {
         hull_points.push_back(point_set[current_pos]);
         next_pos = (current_pos + 1) % n;
 
         for (size_t i = 0; i < n; ++i) {
             if (checkOrientation(point_set[current_pos], point_set[i], point_set[next_pos]) == 2) {
                 next_pos = i;
             }
         }
 
         current_pos = next_pos;
     } while (current_pos != leftmost_pos);
 
     return hull_points;
 }
 
 /**
  * @brief Formats hull points to string representation
  * @param points Vector of HullPoints to format
  * @return String in format "x,y x,y ..."
  */
 inline std::string formatHull(const std::vector<HullPoint>& points) {
     std::ostringstream stream;
     for (const auto& pt : points) {
         stream << pt.x << "," << pt.y << " ";
     }
     return stream.str();
 }