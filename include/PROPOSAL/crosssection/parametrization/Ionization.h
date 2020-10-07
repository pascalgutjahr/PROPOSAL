
/******************************************************************************
 *                                                                            *
 * This file is part of the simulation tool PROPOSAL.                         *
 *                                                                            *
 * Copyright (C) 2017 TU Dortmund University, Department of Physics,          *
 *                    Chair Experimental Physics 5b                           *
 *                                                                            *
 * This software may be modified and distributed under the terms of a         *
 * modified GNU Lesser General Public Licence version 3 (LGPL),               *
 * copied verbatim in the file "LICENSE".                                     *
 *                                                                            *
 * Modifcations to the LGPL License:                                          *
 *                                                                            *
 *      1. The user shall acknowledge the use of PROPOSAL by citing the       *
 *         following reference:                                               *
 *                                                                            *
 *         J.H. Koehne et al.  Comput.Phys.Commun. 184 (2013) 2070-2090 DOI:  *
 *         10.1016/j.cpc.2013.04.001                                          *
 *                                                                            *
 *      2. The user should report any bugs/errors or improvments to the       *
 *         current maintainer of PROPOSAL or open an issue on the             *
 *         GitHub webpage                                                     *
 *                                                                            *
 *         "https://github.com/tudo-astroparticlephysics/PROPOSAL"            *
 *                                                                            *
 ******************************************************************************/

#pragma once

#include "PROPOSAL/EnergyCutSettings.h"
#include "PROPOSAL/crosssection/parametrization/Parametrization.h"
#include "PROPOSAL/medium/Medium.h"
#include "PROPOSAL/crosssection/CrossSection.h"

namespace PROPOSAL {
namespace crosssection {
    class Ionization : public Parametrization {
    protected:
        EnergyCutSettings cuts_;

        double Delta(const Medium&, double beta, double gamma) const;

    public:
        Ionization(const EnergyCutSettings&);
        virtual ~Ionization() = default;

        using only_stochastic = std::false_type;
        using component_wise = std::false_type;

        virtual double FunctionToDEdxIntegral(
            const ParticleDef&, const Medium&, double, double) const = 0;
        virtual tuple<double, double> GetKinematicLimits(
            const ParticleDef&, const Medium&, double) const noexcept
            = 0;
        double FunctionToDE2dxIntegral(
            const ParticleDef&, const Medium&, double, double) const;
        virtual double DifferentialCrossSection(
            const ParticleDef&, const Medium&, double, double) const = 0;
        double DifferentialCrossSection(
                const ParticleDef&, const Component&, double, double) const final;
        tuple<double, double> GetKinematicLimits(
                const ParticleDef&, const Component&, double) const final;
        double GetLowerEnergyLim(const ParticleDef&) const noexcept override;
    };

    class IonizBetheBlochRossi : public Ionization {
        double InelCorrection(
            const ParticleDef&, const Medium&, double, double) const;
        double CrossSectionWithoutInelasticCorrection(
            const ParticleDef&, const Medium&, double, double) const;

    public:
        IonizBetheBlochRossi(const EnergyCutSettings&);
        using base_param_t = Ionization;

        tuple<double, double> GetKinematicLimits(
            const ParticleDef&, const Medium&, double) const noexcept final;
        double DifferentialCrossSection(
            const ParticleDef&, const Medium&, double, double) const final;
        double FunctionToDEdxIntegral(
            const ParticleDef&, const Medium&, double, double) const final;
    };

    struct IonizBergerSeltzerBhabha : public Ionization {
        IonizBergerSeltzerBhabha(const EnergyCutSettings&);
        using base_param_t = Ionization;

        tuple<double, double> GetKinematicLimits(
            const ParticleDef&, const Medium&, double) const noexcept final;
        double DifferentialCrossSection(
            const ParticleDef&, const Medium&, double, double) const final;
        double FunctionToDEdxIntegral(
            const ParticleDef&, const Medium&, double, double) const final;
    };

    struct IonizBergerSeltzerMoller : public Ionization {
        IonizBergerSeltzerMoller(const EnergyCutSettings&);
        using base_param_t = Ionization;

        tuple<double, double> GetKinematicLimits(
            const ParticleDef&, const Medium&, double) const noexcept final;
        double DifferentialCrossSection(
            const ParticleDef&, const Medium&, double, double) const final;
        double FunctionToDEdxIntegral(
            const ParticleDef&, const Medium&, double, double) const final;
    };

// Factory pattern functions

template <typename P, typename M>
using ioniz_func_ptr = cross_t_ptr<P, M>(*)(P, M, std::shared_ptr<const
        EnergyCutSettings>, bool);

template <typename Param, typename P, typename M>
cross_t_ptr<P, M> create_ioniz(P p_def, M medium,std::shared_ptr<const
        EnergyCutSettings> cuts, bool interpol) {
    auto param = Param(*cuts);
    return make_crosssection(param, p_def, medium, cuts, interpol);
}

template<typename P, typename M>
static std::map<std::string, ioniz_func_ptr<P, M>> ioniz_map = {
        {"BetheBlochRossi", create_ioniz<IonizBetheBlochRossi, P, M>},
        {"BergerSeltzerBhabha", create_ioniz<IonizBergerSeltzerBhabha, P, M>},
        {"BergerSeltzerMoller", create_ioniz<IonizBergerSeltzerMoller, P, M>}
};

template<typename P, typename M>
cross_t_ptr<P, M> make_ionization(P p_def, M medium, std::shared_ptr<const
        EnergyCutSettings> cuts, bool interpol, const nlohmann::json& config){
    if (!config.contains("parametrization"))
        throw std::logic_error("No parametrization passed for ionization");

    std::string param_name = config["parametrization"];
    auto it = ioniz_map<P, M>.find(param_name);
    if (it == ioniz_map<P, M>.end())
        throw std::logic_error("Unknown parametrization for ionization");

    return it->second(p_def, medium, cuts, interpol);
}

} // crosssection
} // namespace PROPOSAL
