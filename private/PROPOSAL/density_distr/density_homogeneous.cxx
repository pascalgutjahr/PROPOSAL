#include "PROPOSAL/density_distr/density_homogeneous.h"                                                                                                    
#include <cmath>
#include <iostream>
 
Density_homogeneous::Density_homogeneous():
    Density_distr()
{}

Density_homogeneous::Density_homogeneous(const Density_homogeneous& dens_distr):
    Density_distr()
{
    (void) dens_distr;
}

double Density_homogeneous::Correct(Vector3D xi, 
                                    Vector3D direction, 
                                    double res) const
{
    (void) xi;
    (void) direction;

    return res; 
}

double Density_homogeneous::Integrate(Vector3D xi, 
                                      Vector3D direction, 
                                      double distance) const
{
    (void) xi;
    (void) direction;

    return distance; 
}

double Density_homogeneous::Calculate(Vector3D xi, 
                                      Vector3D direction, 
                                      double distance) const 
{
    (void)xi;
    (void)direction;

    return distance;
}

double Density_homogeneous::GetCorrection(Vector3D x) const
{
    (void)x;

    return 1;
}