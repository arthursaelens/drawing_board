#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "invken.h"
#define PI 3.14159265

/**
 * @brief Calculates the number of interpolated points between two points.
 * 
 * This function calculates the number of interpolated points needed based on the 
 * distance between two points and a specified separation distance.
 * 
 * @param[in] deltaX The difference in the x-coordinates between the two points.
 * @param[in] deltaY The difference in the y-coordinates between the two points.
 * @param[in] separation The desired separation distance between interpolated points.
 * @return The number of interpolated points.
 */
int calculateNumberOfInterpolatedPoints(float deltaX, float deltaY, float separation) {
    float dist = fabs(sqrt(deltaX * deltaX + deltaY * deltaY));
    return (int)floor(dist / separation);
}

/**
 * @brief Interpolates points between two given points.
 * 
 * This function generates a set of interpolated points between two given points with a specified separation distance.
 * 
 * @param[in] point1 The first point.
 * @param[in] point2 The second point.
 * @param[in] separation The desired separation distance between interpolated points.
 * @return Pointer to an array of interpolated points.
 * @retval NULL if memory allocation fails.
 */
Point_XY* interpol(Point_XY point1, Point_XY point2, float separation) {
    double deltaX = point2.x - point1.x;
    double deltaY = point2.y - point1.y;

    int numInterpolatedPoints = calculateNumberOfInterpolatedPoints(deltaX, deltaY, separation);

    Point_XY* interpolatedPoints = (Point_XY*)malloc(numInterpolatedPoints * sizeof(Point_XY));
    if (interpolatedPoints == NULL) {
        // Memory allocation failed
        return NULL;
    }

    double stepX = deltaX / (numInterpolatedPoints + 1);
    double stepY = deltaY / (numInterpolatedPoints + 1);

    for (int i = 0; i < numInterpolatedPoints; i++) {
        interpolatedPoints[i].x = point1.x + (i + 1) * stepX;
        interpolatedPoints[i].y = point1.y + (i + 1) * stepY;
    }

    return interpolatedPoints;
}

/**
 * @brief Extends an array of points with interpolated points.
 * 
 * This function extends an array of points by interpolating points between each pair of points in the original array. 
 * Optionally, it can also interpolate points between the last and first points to form a closed loop.
 * 
 * @param[in] array_XY The original array of points.
 * @param[in] arraySize The size of the original array.
 * @param[in] distanceBetweenPoints The desired separation distance between interpolated points.
 * @param[out] interpolatedArraySize Pointer to an integer where the size of the interpolated array will be stored.
 * @param[in] connect Flag indicating whether to interpolate points between the last and first points to form a closed loop.
 * @return Pointer to the extended array of points, including interpolated points.
 * @retval NULL if memory allocation fails.
 */
Point_XY* extendArrayWithInterpolation(Point_XY* array_XY, int arraySize, float distanceBetweenPoints, int* interpolatedArraySize, int connect) {
    float deltaX;
    float deltaY;
    int numInterpolatedPoints;

    *interpolatedArraySize = 0;
    for (int i = 0; i < arraySize - 1; i++) {
        deltaX = array_XY[i + 1].x - array_XY[i].x;
        deltaY = array_XY[i + 1].y - array_XY[i].y;
        *interpolatedArraySize += calculateNumberOfInterpolatedPoints(deltaX, deltaY, distanceBetweenPoints);
    }

    if (connect) {
        deltaX = array_XY[0].x - array_XY[arraySize - 1].x;
        deltaY = array_XY[0].y - array_XY[arraySize - 1].y;
        *interpolatedArraySize += calculateNumberOfInterpolatedPoints(deltaX, deltaY, distanceBetweenPoints);
    }

    *interpolatedArraySize += arraySize;

    Point_XY* interpolatedArray = (Point_XY*)malloc((*interpolatedArraySize) * sizeof(Point_XY));
    if (interpolatedArray == NULL) {
        return NULL;
    }

    int currentIndex = 0;
    for (int i = 0; i < arraySize; i++) {
        interpolatedArray[currentIndex++] = array_XY[i];

        if (i < arraySize - 1) {
            deltaX = array_XY[i + 1].x - array_XY[i].x;
            deltaY = array_XY[i + 1].y - array_XY[i].y;
            numInterpolatedPoints = calculateNumberOfInterpolatedPoints(deltaX, deltaY, distanceBetweenPoints);

            Point_XY* interpolatedPoints = interpol(array_XY[i], array_XY[i + 1], distanceBetweenPoints);
            for (int j = 0; j < numInterpolatedPoints; j++) {
                interpolatedArray[currentIndex++] = interpolatedPoints[j];
            }

            free(interpolatedPoints);
        }
    }

    if (connect) {
        deltaX = array_XY[0].x - array_XY[arraySize - 1].x;
        deltaY = array_XY[0].y - array_XY[arraySize - 1].y;
        numInterpolatedPoints = calculateNumberOfInterpolatedPoints(deltaX, deltaY, distanceBetweenPoints);

        Point_XY* interpolatedPoints = interpol(array_XY[arraySize - 1], array_XY[0], distanceBetweenPoints);
        for (int j = 0; j < numInterpolatedPoints; j++) {
            interpolatedArray[currentIndex++] = interpolatedPoints[j];
        }

        free(interpolatedPoints);
    }

    return interpolatedArray;
}
