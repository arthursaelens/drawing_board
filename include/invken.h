#ifndef INVKEN
#define INVKEN

#include <stdio.h>
#include <math.h>

#define PI 3.14159265


/**
 * @brief Struct representing a point in XY coordinates.
 */
typedef struct {
    float x; /**< X coordinate. */
    float y; /**< Y coordinate. */
} Point_XY;

/**
 * @brief Struct representing the angles of the robot arms.
 * 
 * theta1 is the angle for the lower servo, and theta2 is the angle for the upper servo.
 */
typedef struct {
    float theta1; /**< Angle for the lower servo. */
    float theta2; /**< Angle for the upper servo. */
} Point_angles;

/**
 * @brief Struct representing the pulse widths for the servos at a point.
 */
typedef struct {
    float pw1; /**< Pulse width for the lower servo. */
    float pw2; /**< Pulse width for the upper servo. */
} Point_pws;



/**
 * @brief Calculates the forward kinematics for the robot arm.
 * 
 * This function computes the x and y coordinates of the robot arm's end effector 
 * based on the given lengths of the arm segments and the joint angles.
 * 
 * @param[in] l1 Length of the first arm segment.
 * @param[in] l2 Length of the second arm segment.
 * @param[in] q1 Angle of the first joint (lower servo).
 * @param[in] q2 Angle of the second joint (upper servo).
 * @param[out] x Pointer to store the calculated x coordinate.
 * @param[out] y Pointer to store the calculated y coordinate.
 */
void forward_kinematics(double l1, double l2, double q1, double q2, double *x, double *y) {
    // Calculate the x and y coordinates
    *x = l1 * cos(q1) + l2 * cos(q1 + q2);
    *y = l1 * sin(q1) + l2 * sin(q1 + q2);
}


/**
 * @brief Calculates the joint angles using inverse kinematics.
 * 
 * This function computes the angles theta1 and theta2 required for the robot arm 
 * to reach a specified point in the XY plane. It handles quadrant issues with atan().
 * 
 * @param[in] xy_point The target point in XY coordinates.
 * @param[in] l1 Length of the first arm segment.
 * @param[in] l2 Length of the second arm segment.
 * @return The calculated joint angles.
 */
Point_angles XY_to_angles(Point_XY xy_point, double l1, double l2) {
    Point_angles point_angles;

    point_angles.theta2 = -acos((pow(xy_point.x, 2) + pow(xy_point.y, 2) - pow(l1, 2) - pow(l2, 2)) / (2 * l1 * l2));

    double alpha = atan2(xy_point.y, xy_point.x);
    double beta = atan2((l2 * sin(point_angles.theta2)), (l1 + l2 * cos(point_angles.theta2)));
    
    point_angles.theta1 = (alpha - beta);

    // Calculate forward kinematics with thetas from the formula
    double test_x;
    double test_y;
    forward_kinematics(l1, l2, point_angles.theta1, point_angles.theta2, &test_x, &test_y);
    
    return point_angles;
}


/**
 * @brief Converts joint angles to pulse widths for the servos.
 * 
 * This function maps the joint angles theta1 and theta2 to the corresponding pulse widths 
 * required for the servos to achieve these angles.
 * 
 * @param[in] p The joint angles.
 * @return The pulse widths for the servos.
 */
Point_pws angles_to_pws(Point_angles p) {
    Point_pws P;
    P.pw1 = -0.6048 * p.theta1 + 2.0; // aan de hand van metingen tussen pulswidths en hoeken hebben we dit lineair verband gekregen
    P.pw2 = -0.6048 * p.theta2 + 0.55;

    return P;
}


#endif //INVKEN
