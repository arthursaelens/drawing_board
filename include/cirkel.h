#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "invken.h"  
#define PI 3.14159265

/**
 * @brief Generates an array of points on a circle.
 * 
 * This function generates an array of Point_XY structures representing points on a circle 
 * with a given radius and center. The number of points generated is specified by the user.
 * 
 * @param[in] radius The radius of the circle.
 * @param[in] centerX The x-coordinate of the circle's center.
 * @param[in] centerY The y-coordinate of the circle's center.
 * @param[in] numPoints The number of points to generate on the circle.
 * @return Pointer to an array of Point_XY structures representing the points on the circle.
 * @retval NULL if memory allocation fails.
 */
Point_XY* generatePointsOnCircle(double radius, double centerX, double centerY, int numPoints) {
    Point_XY* points = (Point_XY*)malloc(numPoints * sizeof(Point_XY));
    if (points == NULL) {
        // Memory allocation failed
        return NULL;
    }

    double angleIncrement = 2 * PI / numPoints;
    double angle = 0.0;

    for (int i = 0; i < numPoints; i++) {
        points[i].x = centerX + radius * cos(angle);
        points[i].y = centerY + radius * sin(angle);
        angle += angleIncrement;
    }

    return points;
}
