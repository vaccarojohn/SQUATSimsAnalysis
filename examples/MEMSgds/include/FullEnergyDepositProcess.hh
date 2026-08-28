#ifndef FULL_ENERGY_DEPOSIT_PROCESS_HH
#define FULL_ENERGY_DEPOSIT_PROCESS_HH

#include "G4VDiscreteProcess.hh"
#include "G4Step.hh"
#include "G4Track.hh"

class FullEnergyDepositProcess : public G4VDiscreteProcess {
public:
    FullEnergyDepositProcess(G4String name = "FullEnergyDeposit");
    ~FullEnergyDepositProcess() override = default;

    G4bool IsApplicable(const G4ParticleDefinition& particle) override;

    G4VParticleChange* PostStepDoIt(const G4Track& track, const G4Step& step) override;
// Required implementation to avoid abstract class error
    G4double GetMeanFreePath(const G4Track&, G4double, G4ForceCondition*) override;

};

#endif
