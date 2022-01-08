#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
// #include "ansi_escapes.h"

#ifdef __WIN32
    #include<windows.h>
# endif
// #else
//    #include <Python.h>
// #endif

#define FRAME_TIME 10
#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH_ACTUAL SCREEN_WIDTH*2


void sleepFor(unsigned int mSecond)
{
#ifdef WIN32
    Sleep(mSecond);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = mSecond / 1000;
    ts.tv_nsec = (mSecond % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (mSecond >= 1000)
    sleep(mSecond / 1000);
    usleep((mSecond % 1000) * 1000);
#endif
}

struct Position{
    double x;
    double y;
};

struct ConicSection{
    double A; // major axis
    double B; // rotation
    double C; // minor axis
    double D; // x translation
    double E; // y translation
    double F; // scale
    struct Position center;
};

struct Ellipse{
    double majAx;
    double minAx;
    double theta;
};

struct ConicSection translateConic(struct ConicSection myConic, double x, double y)
{
// General structure 
    struct ConicSection newConic = myConic;
    newConic.center.x = newConic.center.x-x;
    newConic.center.y = newConic.center.y-y;


    newConic.A = myConic.A; 
    newConic.B = myConic.B; 
    newConic.C = myConic.C;
    newConic.D = 2*myConic.A*x + myConic.B*y + myConic.D;
    newConic.E = myConic.B*x + 2*myConic.C*y + myConic.E;
    newConic.F = myConic.A*x*x + myConic.B*x*y + myConic.C*y*y + myConic.D*x + myConic.E*y + myConic.F;

    return newConic;
}

struct ConicSection rotateConic(struct ConicSection myConic, double a)
{
// General structure 
    struct ConicSection newConic = myConic;

    newConic.A = myConic.A*cos(a)*cos(a) + myConic.B*cos(a)*sin(a) + myConic.C*sin(a)*sin(a);
    newConic.B = -2*myConic.A*cos(a)*sin(a) + myConic.B*(cos(a)*cos(a) - sin(a)*sin(a)) +2*myConic.C*cos(a)*sin(a);
    newConic.C = myConic.A*sin(a)*sin(a) - myConic.B*cos(a)*sin(a) + myConic.C*cos(a)*cos(a);
    newConic.D = myConic.D*cos(a) + myConic.E*sin(a);
    newConic.E = -myConic.D*sin(a) + myConic.E*cos(a);
    newConic.F = myConic.F;

    return newConic;
}

struct ConicSection rotateConicSelf(struct ConicSection myConic, double a)
{
// General structure 
    double xCenter = myConic.center.x;
    double yCenter = myConic.center.y;
    myConic = translateConic(myConic, xCenter, yCenter); 
    
    struct ConicSection newConic = myConic;



    newConic.A = myConic.A*cos(a)*cos(a) + myConic.B*cos(a)*sin(a) + myConic.C*sin(a)*sin(a);
    newConic.B = -2*myConic.A*cos(a)*sin(a) + myConic.B*(cos(a)*cos(a) - sin(a)*sin(a)) +2*myConic.C*cos(a)*sin(a);
    newConic.C = myConic.A*sin(a)*sin(a) - myConic.B*cos(a)*sin(a) + myConic.C*cos(a)*cos(a);
    newConic.D = myConic.D*cos(a) + myConic.E*sin(a);
    newConic.E = -myConic.D*sin(a) + myConic.E*cos(a);
    newConic.F = myConic.F;
    
    newConic = translateConic(newConic, -xCenter, -yCenter); 
    
    return newConic;
}

double calculateConic(int x, int y, struct ConicSection myConic)
{
    double result;
// Genearal structure
    result = myConic.A*(x*x) + myConic.B*(x*y) + myConic.C*(y*y);
    result += myConic.D*(x) + myConic.E*(y) + myConic.F;

    return result;
}

void drawConic(struct ConicSection myConic)
{
    printf("\x1b[H");
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH_ACTUAL];
    memset(screen, '_', sizeof(screen)/sizeof(char));
    
    // printConicAtributes(myConic);
    for(int y=1; y<=SCREEN_HEIGHT; y++){
        for(int x=1; x<=SCREEN_WIDTH; x++){
            if(calculateConic(x, y, myConic) <= 1.0f){
                screen[(y-1)][2*(x-1)] = ' ';
                screen[(y-1)][2*(x-1)+1] = ' ';
            }
        }
    }

    for(int i=0; i<SCREEN_HEIGHT; i++){
        for(int j=0; j<SCREEN_WIDTH_ACTUAL; j++){
            printf("%c", screen[i][j]);
        }
        printf("\n");
    }
    sleepFor(200);
}


struct ConicSection getEllipse(double majAx, double minAx, double theta)
{
// General structure
    struct ConicSection newEllipse;
    newEllipse.A = minAx*minAx;
    newEllipse.B = 0;
    newEllipse.C = majAx*majAx;
    newEllipse.D = 0;
    newEllipse.E = 0;
    newEllipse.F = - (newEllipse.A)*(newEllipse.C);
    newEllipse.center.x = 0;
    newEllipse.center.y = 0;
    if(theta){
        newEllipse = rotateConic(newEllipse, theta);
    }
    newEllipse = translateConic(newEllipse, -SCREEN_WIDTH/2, -SCREEN_WIDTH/2);

    return newEllipse;
}

struct ConicSection getHyperbola(double majAx, double minAx, double theta)
{
// General structure
    struct ConicSection newEllipse;
    newEllipse.A = minAx*minAx;
    newEllipse.B = 0;
    newEllipse.C = -majAx*majAx;
    newEllipse.D = 0;
    newEllipse.E = 0;
    newEllipse.F = (newEllipse.A)*(newEllipse.C);
    newEllipse.center.x = 0;
    newEllipse.center.y = 0;
    if(theta){
        newEllipse = rotateConic(newEllipse, theta);
    }
    newEllipse = translateConic(newEllipse, -SCREEN_WIDTH/2, -SCREEN_WIDTH/2);

    return newEllipse;
}

void printConicAtributes(struct ConicSection myConic){
    printf("{%lf, %lf}, A=%lf, B=%lf, C=%lf, D=%lf, E=%lf, F=%lf\n", myConic.center.x, myConic.center.y,
                            myConic.A, myConic.B, myConic.C, myConic.D, myConic.E, myConic.F);
}

int main()
{
    printf("\x1b[2J");
    int a, b;
    printf("what type of conic section do you want?(1 - Ellipse, 2 - Hyperbola): ");
    scanf("%d", &a);
    struct ConicSection myConic;
    if(a == 1){
        printf("Give the size of the ellipse: ");
        scanf("%d %d", &a, &b);
        myConic = getEllipse(a, b, 0);
    }
    else if(a == 2){
        printf("Give the size of the hyperbola: ");
        scanf("%d %d", &a, &b);
        myConic = getHyperbola(a, b, 0);
    }
    else{
        printf("Invalid input.\n");
        return 0;
    }

    for(double i=0; i<100; i+=0.2){
        drawConic(rotateConicSelf(myConic, i));
    }
    #if __WIN32
        system("pause");
    #endif
}
