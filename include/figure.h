#include "invken.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

/**
 * @brief Struct representing a line consisting of multiple points.
 * 
 * This struct holds an array of Point_XY instances, which are the basic building blocks.
 */
typedef struct {
    int numPoints;             /**< Number of points in the line. */
    const Point_XY *points;    /**< Array of points representing the line. */
} Line;

/**
 * @brief Struct representing a figure composed of multiple lines.
 * 
 * This struct holds an array of Line instances, representing a figure to be drawn.
 */
typedef struct {
    Line *lines;    /**< Array of lines that make up the figure. */
    int numLines;   /**< Number of lines in the figure. */
} Figure;


/**
 * @brief Creates a Line from a string representing the points.
 * 
 * This function parses a string to initialize the points of a line. It is not used 
 * because it requires dynamic memory allocation, which is not feasible due to memory constraints.
 * 
 * @note This method cannot be used because for PROGMEM we need the line point arrays to be constants.
 * 
 * @param[in] pointsString The string representing the points in the format "{x1, y1, x2, y2, ...}".
 * @param[in] numPoints The number of points to be initialized.
 * @return The initialized Line.
 */
/*
Line makeLine0(const char *pointsString, int numPoints) {

    // Method to create a line from a string
    // We don't want to initialize every point in a line separately
    // so this method parses a string representing the points

    // We do not use this method because we don't have the memory capacities for it

    Line line;
    line.numPoints = numPoints;

    line.points = (Point_XY *)malloc(line.numPoints * sizeof(Point_XY)); // Allocate memory

    // Parse the string to extract coordinates and initialize points
    const char *delimiters = "{}, ";
    char *token;
    char *strCopy = strdup(pointsString); // Make a copy of the input string
    token = strtok(strCopy, delimiters);
    for (int i = 0; i < numPoints && token != NULL; i++) {
        line.points[i].x = atof(token);
        token = strtok(NULL, delimiters);
        line.points[i].y = atof(token);
        token = strtok(NULL, delimiters);
    }
    free(strCopy); // Free the memory allocated by strdup

    return line;
}
*/


/**
 * @brief Creates a Line from an array of points.
 * 
 * This helper function initializes a Line using a given array of points.
 * 
 * @param[in] points Array of points.
 * @param[in] numPoints Number of points in the array.
 * @return The initialized Line.
 */
Line makeLine(Point_XY *points, int numPoints) {

    // Hulpfunctie voor lines te maken in main (Helper function to create lines in main)

    Line line;
    line.numPoints = numPoints;
    line.points = points;
    return line;
}

/**
 * @brief Creates a Figure from an array of lines.
 * 
 * This function initializes a Figure using a given array of lines.
 * 
 * @param[in] lines Array of lines.
 * @param[in] numLines Number of lines in the array.
 * @return The initialized Figure.
 */
Figure makeFigure(Line *lines, int numLines) {

    // Hulpfunctie voor figuren te maken in main (Helper function to create figures in main)

    Figure figure;
    figure.numLines = numLines;
    figure.lines = lines;
    return figure;
}