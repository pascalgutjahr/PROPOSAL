#include "PROPOSAL/crossection/CrossSectionDNDX/CrossSectionDNDXInterpolant.h"
#include <cmath>

using namespace PROPOSAL;

namespace PROPOSAL {
double transform_relativ_loss(double v_cut, double v_max, double v)
{
    if (v_cut == 0)
        return v_cut;
    return v_cut * std::exp(v * std::log(v_max / v_cut));
}
}

unique_ptr<Interpolant> CrossSectionDNDXInterpolant::build_dndx(
    Interpolant2DBuilder::Definition interpol_def, const InterpolationDef& def)
{
    interpol_def.function2d = [this](double energy, double v) {
        return CrossSectionDNDXIntegral::Calculate(
            energy, v, transform_relativ_loss);
    };
    return Helper::InitializeInterpolation("dNdx",
        make_unique<Interpolant2DBuilder>(interpol_def), hash_cross_section,
        def);
}

double CrossSectionDNDXInterpolant::Calculate(
    double energy, double v, v_trafo_t)
{
    return dndx->Interpolate(energy, v);
}

double CrossSectionDNDXInterpolant::GetUpperLimit(
    double energy, double rate, v_trafo_t)
{
    auto lim = GetIntegrationLimits(energy);
    auto v = dndx->FindLimit(energy, rate);
    return transform_relativ_loss(get<MIN>(lim), get<MAX>(lim), v);
}

namespace PROPOSAL {
Interpolant2DBuilder::Definition build_dndx_interpol_def(
    const crosssection::Parametrization& param, const ParticleDef& p_def,
    const InterpolationDef& def)
{
    Interpolant2DBuilder::Definition interpol_def;
    interpol_def.max1 = def.nodes_cross_section;
    interpol_def.x1min = param.GetLowerEnergyLim(p_def);
    interpol_def.x1max = def.max_node_energy;
    interpol_def.max2 = def.nodes_cross_section;
    interpol_def.x2min = 0.0;
    interpol_def.x2max = 1.0;
    interpol_def.romberg1 = def.order_of_interpolation;
    interpol_def.isLog1 = true;
    interpol_def.romberg2 = def.order_of_interpolation;
    interpol_def.rombergY = def.order_of_interpolation;
    interpol_def.rationalY = true;
    return interpol_def;
}
} // namespace PROPOSAL