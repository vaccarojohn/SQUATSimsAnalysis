#include "FullEnergyDepositProcess.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleChange.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"

#include "G4Gamma.hh"

FullEnergyDepositProcess::FullEnergyDepositProcess(G4String name)
    : G4VDiscreteProcess(name) {

}

G4bool FullEnergyDepositProcess::IsApplicable(
    const G4ParticleDefinition& particle)
{
    // Apply only to gamma tracks.
    return &particle == G4Gamma::GammaDefinition();
}


G4VParticleChange* FullEnergyDepositProcess::PostStepDoIt(
    const G4Track& track, const G4Step& step) {
    
    aParticleChange.Initialize(track);
    // make sure we're in the Si

    if (track.GetVolume()->GetLogicalVolume()->GetName() == "fSiliconLogical") {
       
        aParticleChange.ProposeLocalEnergyDeposit(track.GetKineticEnergy());
        aParticleChange.ProposeEnergy(0.0);
        aParticleChange.ProposeTrackStatus(fStopAndKill);

    }

    return &aParticleChange;
}

G4double FullEnergyDepositProcess::GetMeanFreePath(
    const G4Track& track,
    G4double,
    G4ForceCondition* condition)
{

    *condition = NotForced;

    const G4VPhysicalVolume* volume = track.GetVolume();

    if (volume == nullptr) {
        return DBL_MAX;
    }

    const G4LogicalVolume* logicalVolume =
        volume->GetLogicalVolume();

    if (logicalVolume == nullptr) {
        return DBL_MAX;
    }

    if (logicalVolume->GetName() == "fSiliconLogical") {
        //https://refractiveindex.info/?shelf=main&book=Si&page=Franta-10K
        //use absorption coef at 620 nm (2 eV)
        return 4.74 * um;
    }

    // Process is inactive outside the silicon volume.
    return DBL_MAX;
}
