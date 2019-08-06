
#include <algorithm>

#include "PROPOSAL/crossection/ComptonIntegral.h"
#include "PROPOSAL/crossection/ComptonInterpolant.h"
#include "PROPOSAL/crossection/factories/ComptonFactory.h"
#include "PROPOSAL/crossection/parametrization/Compton.h"

#include "PROPOSAL/Logging.h"

using namespace PROPOSAL;

ComptonFactory::ComptonFactory()
        : compton_map_str_()
        , compton_map_enum_()
        , string_enum_()
{
    // Register all compton parametrizations in lower case!

    Register("comptonkleinnishina", KleinNishina, &ComptonKleinNishina::create);
    Register("none", None, nullptr); //empty parametrization
}

// ------------------------------------------------------------------------- //
ComptonFactory::~ComptonFactory()
{
    compton_map_str_.clear();
    compton_map_enum_.clear();
    string_enum_.clear();
}

// ------------------------------------------------------------------------- //
void ComptonFactory::Register(const std::string& name, Enum enum_t, RegisterFunction create)
{
    compton_map_str_[name]    = create;
    compton_map_enum_[enum_t] = create;
    string_enum_.insert(BimapStringEnum::value_type(name, enum_t));
}

// --------------------------------------------------------------------- //
// Most general creation
// --------------------------------------------------------------------- //

// ------------------------------------------------------------------------- //
CrossSection* ComptonFactory::CreateCompton(const ParticleDef& particle_def,
                                                          const Medium& medium,
                                                          const EnergyCutSettings& cuts,
                                                          const Definition& def) const
{
    if(def.parametrization == ComptonFactory::Enum::None){
        log_fatal("Can't return Compton Crosssection if parametrization is None");
        return NULL;
    }

    ComptonMapEnum::const_iterator it = compton_map_enum_.find(def.parametrization);

    if (it != compton_map_enum_.end())
    {
        return new ComptonIntegral(*it->second(particle_def, medium, cuts, def.multiplier));
    } else
    {
        log_fatal("Compton %s not registered!", typeid(def.parametrization).name());
        return NULL; // Just to prevent warnings
    }
}

// ------------------------------------------------------------------------- //
CrossSection* ComptonFactory::CreateCompton(const ParticleDef& particle_def,
                                                          const Medium& medium,
                                                          const EnergyCutSettings& cuts,
                                                          const Definition& def,
                                                          InterpolationDef interpolation_def) const
{
    if(def.parametrization == ComptonFactory::Enum::None){
        log_fatal("Can't return Compton Crosssection if parametrization is None");
        return NULL;
    }

    ComptonMapEnum::const_iterator it = compton_map_enum_.find(def.parametrization);

    if (it != compton_map_enum_.end())
    {
        return new ComptonInterpolant(*it->second(particle_def, medium, cuts, def.multiplier), interpolation_def);
    } else
    {
        log_fatal("Compton %s not registered!", typeid(def.parametrization).name());
        return NULL; // Just to prevent warnings
    }
}

// ------------------------------------------------------------------------- //
ComptonFactory::Enum ComptonFactory::GetEnumFromString(const std::string& name)
{
    std::string name_lower = name;
    std::transform(name.begin(), name.end(), name_lower.begin(), ::tolower);

    BimapStringEnum::left_const_iterator it = string_enum_.left.find(name_lower);
    if (it != string_enum_.left.end())
    {
        return it->second;
    } else
    {
        log_fatal("Compton %s not registered!", name.c_str());
        return ComptonFactory::Fail; // Just to prevent warinngs

    }
}

// ------------------------------------------------------------------------- //
std::string ComptonFactory::GetStringFromEnum(const ComptonFactory::Enum& enum_t)
{
    BimapStringEnum::right_const_iterator it = string_enum_.right.find(enum_t);
    if (it != string_enum_.right.end())
    {
        return it->second;
    } else
    {
        log_fatal("Compton %s not registered!", typeid(enum_t).name());
        return ""; // Just to prevent warnings
    }
}
