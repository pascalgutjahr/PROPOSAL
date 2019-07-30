#include "PROPOSAL/density_distr/density_splines.h" 
#include "PROPOSAL/math/MathMethods.h" 
#include "PROPOSAL/math/Spline.h" 
#include <functional>
#include <algorithm>
#include <iostream>

Density_splines::Density_splines(const Axis& axis, 
                                 const Spline& splines) :
    Density_distr(axis),
    spline_( splines.clone() ),
    integrated_spline_( splines.clone() )
{
    integrated_spline_->Antiderivative(0);
}

Density_splines::Density_splines(const Density_splines& dens_splines):
    Density_distr( dens_splines ),
    spline_( dens_splines.spline_ ),
    integrated_spline_( dens_splines.integrated_spline_)
{
}

double Density_splines::Helper_function(Vector3D xi, 
                                        Vector3D direction, 
                                        double res, 
                                        double l) const 
{
    double aux = Integrate(xi, direction, l) - Integrate(xi, direction, 0) + res;
    
    return aux;
}

double Density_splines::helper_function(Vector3D xi, 
                                        Vector3D direction, 
                                        double res, 
                                        double l) const 
{
    double delta = axis_->GetEffectiveDistance(xi, direction);

    return spline_->evaluate(axis_->GetDepth(xi) + l * delta);
}

double Density_splines::Correct(Vector3D xi, 
                                Vector3D direction,
                                double res) const 
{
    std::function<double(double)> F = std::bind(&Density_splines::Helper_function, 
                                                this, 
                                                xi, 
                                                direction, 
                                                res, 
                                                std::placeholders::_1);
    
    std::function<double(double)> dF = std::bind(&Density_splines::helper_function, 
                                                this, 
                                                xi, 
                                                direction, 
                                                res,
                                                std::placeholders::_1);

    res = NewtonRaphson(F, dF, 0, 1e7, 1.e-6);

    return res;
}

double Density_splines::Integrate(Vector3D xi, 
                                  Vector3D direction, 
                                  double l) const
{
    double delta = axis_->GetEffectiveDistance(xi, direction);

    return integrated_spline_->evaluate(axis_->GetDepth(xi) + l * delta);
}

double Density_splines::Calculate(Vector3D xi, 
                                     Vector3D direction, 
                                     double distance) const
{
    return Integrate(xi, direction, distance) - Integrate(xi, direction, 0);
}

double Density_splines::GetCorrection(Vector3D xi) const
{
    return spline_->evaluate(axis_->GetDepth(xi));
}

