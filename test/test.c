#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double x;
    double y;
} Point_XY;

int main(void) {
    Point_XY array1[] = {{12, -8}, {-4, -8}};
    for (int i; i< 2; i++) {
        printf("Point %d: (%f, %f)\n", i+1, array1[i].x, array1[i].y);
    }
    
    Point_XY* array_XY;

    int aantal_punten = sizeof(array1) / sizeof(array1[0]);

    // Dynamisch geheugen toewijzen aan array_XY
    array_XY = malloc(aantal_punten * sizeof(Point_XY));
    if (array_XY == NULL) {
        // Foutafhandeling als malloc mislukt
        return -1; // Of een andere foutcode
    }

    // Kopieer de elementen van array1 naar array_XY
    for (int i = 0; i < aantal_punten; i++) {
        array_XY[i] = array1[i];
    }

    // Print de inhoud van array_XY
    for (int i = 0; i < aantal_punten; i++) {
        printf("Point %d: (%f, %f)\n", i+1, array_XY[i].x, array_XY[i].y);
        printf( "%d \n", aantal_punten);
    }

    // Geef het dynamisch toegewezen geheugen vrij
    free(array_XY);

    return 0;
}

//https://www.onlinegdb.com/online_c_compiler