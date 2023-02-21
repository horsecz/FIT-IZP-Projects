/*
 * IZP:             PROJECT 2
 * 
 * @author:         Dominik Horky
 * @login:          xhorky32
 * @academic year:  2019/20
 * @lecture group:  1BIT
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define U_T 25.8563e-3
#define I_0 1e-12

// I_p - I_r == I_0 * (e^(U_p / U_t) - 1) - (U_r / R)
double equation(double U_0, double U_p, double R)
{
    double count = (I_0 * (exp(U_p / U_T) - 1)) - ((U_0 - U_p) / R);
    return count;
}

// find & return U_p by bisection method
double diode(double U_0, double R, double EPS)
{
    double min = 0;
    double max = U_0;
    double middle = 0;
    double res_min;
    double res_max;
    double res_middle;
    
    if (R == INFINITY) // infinity resistance leads into zero voltage and current
    {
        return middle;
    }
    
    while((max - min) >= EPS)
    {      
        middle = (min + max) / 2;
        if (middle == max || middle == min || res_middle == 0)  // if interval can not be divided (because values are too low)
        {
            break;
        }
        
        res_min = equation(U_0, min, R);
        res_max = equation(U_0, max, R);
        res_middle = equation(U_0, middle, R);
        if ((res_middle > 0 && res_min > 0) || (res_middle < 0 && res_min < 0))
        {
            min = middle;
        }
        else if ((res_middle > 0 && res_max > 0) || (res_middle < 0 && res_max < 0))
        {
            max = middle;
        }
    }
    return middle;
}

// arguments check, output result (U_p and I_p)
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Bad usage.\nUse: ./proj2 U0 R EPS\n");
        return EXIT_FAILURE;
    }
    else
    {
        char *endptr;
        double arguments[3];
        for (int i = 0; i < 3; i++) // saves 'double' number value to arguments[i] (0 - 2) and checks if input argv[i+1] (1 - 3) is correct
        {
            arguments[i] = strtod(argv[i+1], &endptr);
            if (argv[i+1][0] == '\0' || *endptr != '\0' || arguments[i] < 0 || (arguments[i] == 0 && (i == 1 || i == 2)) || arguments[i] != arguments[i])
            {
                fprintf(stderr, "error: invalid arguments\n");
                return EXIT_FAILURE;
            }
        }
        
        double U_p = diode(arguments[0], arguments[1], arguments[2]);
        double I_p = (I_0 * (exp(U_p / U_T) - 1));
        fprintf(stdout, "Up=%g V\nIp=%g A\n", U_p, I_p);
    }
    return EXIT_SUCCESS;
}
        
