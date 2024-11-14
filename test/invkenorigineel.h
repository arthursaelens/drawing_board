#include <stdio.h>
#include <math.h>

#define PI 3.14159265

double length_arm1 = 16;
double length_arm2 = 17;


typedef struct{
    double x;
    double y;
} Point_XY;

typedef struct{
    Point_XY Points;
    int length;
} line;


//struct for angles of arms 1->lower servo, 2-> upper servo
typedef struct{
    double theta1;
    double theta2;
} Point_angles;

//stuct containing pws (puls_widths for servos) for a point
typedef struct{
    double pw1;
    double pw2;
} Point_pws;



//forw kin for checking
void forward_kinematics(double l1, double l2, double q1, double q2, double *x, double *y) {
    // Calculate the x and y coordinates
    *x = l1 * cos(q1) + l2 * cos(q1 + q2);
    *y = l1 * sin(q1) + l2 * sin(q1 + q2);
}

//use inv kinematics to asign theta1 and theta2 (checks for quadrants bcs of problems with atan())
Point_angles XY_to_angles(Point_XY xy_point, double l1, double l2) {

    Point_angles point_angles;

    point_angles.theta2 = acos((pow(xy_point.x,2) + pow(xy_point.y,2) - pow(l1,2) - pow(l2,2))/(2*l1*l2));

    double alpha = atan2(xy_point.y, xy_point.x);
    double beta = atan(l2 * sin(point_angles.theta2)/ (l1 + l2 * cos(point_angles.theta2)));
    
    point_angles.theta1 = alpha - beta;

    printf("theta2: %fpi \n", point_angles.theta2/PI);
    printf("theta1: %fpi \n", point_angles.theta1/PI);

    //calculate forw kins with thetas from formula
    double test_x;
    double test_y;
    forward_kinematics(l1, l2, point_angles.theta1, point_angles.theta2, &test_x, &test_y);
    
    //check results
    if (test_x - xy_point.x > 0.2 || test_y - xy_point.y > 0.2) {
        printf("OEEEPPSSS\n");
        printf("actual x: %f, from arms: %f \nactual y: %f, from arms: %f\n", xy_point.x, test_x, xy_point.y, test_y);
    }

    return point_angles;
}

//pulse_width = -0.5411*theta + 2.3833

Point_pws angles_to_pws(Point_angles p) {
    Point_pws P;
    P.pw1 = -.5411 * p.theta1 + 2.3833;
    P.pw2 = -.5411 * p.theta2 + 2.3833;
    return P;
}




void print_positions(Point_XY points[], int num_points) {
    
    for (int i = 0; i < num_points; i++) {
        Point_angles point_angle_i;
        Point_pws point_pws_i;
        point_angle_i = XY_to_angles(points[i], length_arm1, length_arm2);
        point_pws_i = angles_to_pws(point_angle_i);

        printf( "Position %d: x = %f, y= %f, theta_1 = %f, theta_2 = %f, pulse_width1 = %f; pulse_width2 = %f \n", 
        i, points[i].x, points[i].y, point_angle_i.theta1, point_angle_i.theta2, point_pws_i.pw1, point_pws_i.pw2);
    }
}

void main() {
    Point_XY array_XY[4] = {{8,0},{10,0}, {10,-2},{8,-2}};

    Point_pws array_PW[4];
    for (int j = 0; j < 4; j++ ) {
        Point_angles point_angle_j = XY_to_angles(array_XY[j],length_arm1,length_arm2);
        
        array_PW[j] = angles_to_pws(point_angle_j);
    }
}