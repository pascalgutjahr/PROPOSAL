#pragma once

#include "PROPOSAL/particle/Particle.h"
#include "PROPOSAL/secondaries/Parametrization.h"

namespace PROPOSAL {
namespace secondaries {
    struct EpairProduction : public Parametrization {
        EpairProduction() = default;
        virtual ~EpairProduction() = default;

        static constexpr InteractionType type
            = PROPOSAL::InteractionType::Epair;

        InteractionType GetInteractionType() const noexcept { return type; };

        virtual double CalculateRho(double, double, const Component&, double) = 0;
        virtual tuple<Vector3D, Vector3D> CalculateDirections(
            Vector3D, double, double, double)
            = 0;
        virtual tuple<double, double> CalculateEnergy(double, double) = 0;
    };
} // namespace secondaries
} // namespace PROPOSAL