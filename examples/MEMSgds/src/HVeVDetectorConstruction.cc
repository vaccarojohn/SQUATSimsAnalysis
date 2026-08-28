#include "HVeVDetectorConstruction.hh"
#include "HVeVSensitivity.hh"
#include "HVeVConfigManager.hh"
#include "HVeVConfigMessenger.hh"

#include "HVeVPhononElectrode.hh"
#include "G4UnionSolid.hh"
#include "G4CMPLogicalBorderSurface.hh"
#include "G4CMPPhononElectrode.hh"
#include "G4CMPVElectrodePattern.hh"
#include "G4CMPSurfaceProperty.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Colour.hh"
#include "G4GeometryManager.hh"
#include "G4LatticeLogical.hh"
#include "G4LatticeManager.hh"
#include "G4LatticePhysical.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SolidStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4TransportationManager.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4CMPFieldManager.hh"
#include "G4CMPMeshElectricField.hh"
#include "G4UniformElectricField.hh"

#include "G4CMPConfigManager.hh"
#include "G4VNIELPartition.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
#include <fstream>
#include <vector>
#include <string>

std::vector<double> readCSV(const std::string& filename)
{
    std::vector<double> values;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    double value;
    while (file >> value) {
        values.push_back(value);
    }

    return values;
}
HVeVDetectorConstruction::HVeVDetectorConstruction()
  : fAluminum(0), fSilicon(0), fSilicon2(0), fLiquidHelium(0),
    fWorldPhys(0), topSurfProp(0), botSurfProp(0), wallSurfProp(0), gridSurfProp(0),
    electrodeSensitivity(0), fConstructed(false), 
    outputFileName(""), CTL(0.), CTR(0.), CBL(0.), CBR(0.),
    fEMField(nullptr), voltage(0.),implantEnergy(0.), implantlayer(0.) {;}

HVeVDetectorConstruction::~HVeVDetectorConstruction() {
  delete fEMField;
  delete topSurfProp;
  delete botSurfProp;
  delete wallSurfProp;
  delete gridSurfProp;
}


G4VPhysicalVolume* HVeVDetectorConstruction::Construct()
{
    if(fConstructed) {
        G4GeometryManager::GetInstance()->OpenGeometry();
        G4PhysicalVolumeStore::GetInstance()->Clean();
        G4LogicalVolumeStore::GetInstance()->Clean();
        G4SolidStore::GetInstance()->Clean();

        if (outputFileName != HVeVConfigManager::GetHitOutput()) {
        outputFileName = HVeVConfigManager::GetHitOutput();
            if (electrodeSensitivity) electrodeSensitivity->SetOutputFile(outputFileName);
        }


        if (voltage != HVeVConfigManager::GetVoltage()) {
            delete fEMField; fEMField = nullptr;
        }

        G4LatticeManager::GetLatticeManager()->Reset();

        G4CMPLogicalBorderSurface::CleanSurfaceTable();
    }

    voltage = HVeVConfigManager::GetVoltage();
   // std::cout << "Voltage: " << voltage/volt << std::endl;
    CTL = HVeVConfigManager::GetCTL()/eV;
   // std::cout << "CTL: " << CTL << std::endl;
    CTR = HVeVConfigManager::GetCTR()/eV;
   // std::cout << "CTR: " << CTR << std::endl;
    CBL = HVeVConfigManager::GetCBL()/eV;
   // std::cout << "CBL: " << CBL << std::endl;
    CBR = HVeVConfigManager::GetCBR()/eV;
   // std::cout << "CBR: " << CBR << std::endl;
    implantEnergy = HVeVConfigManager::GetImplantEnergy();
    if (implantEnergy == 60*keV){implantlayer = 285*nm + 84*3*nm;}
    if (implantEnergy == 40*keV){implantlayer = 200*nm + 67*3*nm;}
    if (implantEnergy == 20*keV){implantlayer = 108*nm + 44*3*nm;}

    DefineMaterials();
    SetupGeometry();
    fConstructed = true;

    return fWorldPhys;
}

void HVeVDetectorConstruction::DefineMaterials()
{
    G4NistManager
        * nistManager = G4NistManager::Instance();
    fAluminum = nistManager->FindOrBuildMaterial("G4_Al");
    fCu = nistManager->FindOrBuildMaterial("G4_Cu");
    fSilicon = nistManager->FindOrBuildMaterial("G4_Si");
    fSilicon2 = nistManager->FindOrBuildMaterial("G4_Ge");
    fLiquidHelium = nistManager->FindOrBuildMaterial("G4_AIR"); // TODO: Correct this
}

void HVeVDetectorConstruction::SetupGeometry()
{
    // Set up the liquid helium world volume
    G4VSolid
        * worldSolid = new G4Box("World", 16.*cm, 16.*cm, 16.*cm);
    G4LogicalVolume* worldLogical =
        new G4LogicalVolume(worldSolid, fLiquidHelium, "World");
    worldLogical->SetUserLimits(new G4UserLimits(10*mm, DBL_MAX, DBL_MAX, 0, 0));
    fWorldPhys = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "World", 0, false, 0);
    
    // Set up the silicon substrate
    G4VSolid
        * fSiliconSolid = new G4Box("fSiliconSolid", 
                                    dp_siliconChipDimX/2., 
                                    dp_siliconChipDimY/2., 
                                    dp_siliconChipDimZ/2.);//-implantlayer/2);

    G4LogicalVolume* fSiliconLogical = 
        new G4LogicalVolume(fSiliconSolid, fSilicon, "fSiliconLogical");
    G4VPhysicalVolume* SiPhys =
        new G4PVPlacement(0, G4ThreeVector(0,0,0), fSiliconLogical, "fSiliconPhys",
                          worldLogical, false, 0);

    // --------------------------------------------------------
    // Dimensions
    // --------------------------------------------------------

    G4double outerHalf = 5.0 * mm;      // outer holder boundary: [-5,5] mm
    G4double frameT    = 200.0 * um;    // holder side thickness
    G4double cornerArm = 5.0 * mm;      // length of each L arm
    G4double zThick    = 1.0 * mm;

    G4double zPos = dp_siliconChipDimZ / 2.0 + zThick / 2.0 -0.0001*nm;

    // --------------------------------------------------------
    // Make one L-shaped corner bracket
    // sx = +1 right, -1 left
    // sy = +1 top,   -1 bottom
    // --------------------------------------------------------

    auto MakeCornerBracket = [](
        const G4String& name,
        G4double frameT,
        G4double cornerArm,
        G4double zThick,
        G4double sx,
        G4double sy
    ) -> G4VSolid*
    {
        auto horiz = new G4Box(
            name + "_HorizLeg",
            cornerArm / 2.0,
            frameT / 2.0,
            zThick / 2.0
        );

        auto vert = new G4Box(
            name + "_VertLeg",
            frameT / 2.0,
            cornerArm / 2.0,
            zThick / 2.0
        );

        // Local origin is the OUTER corner.
        G4ThreeVector horizPos(
            -sx * cornerArm / 2.0,
            -sy * frameT / 2.0,
            0
        );

        G4ThreeVector vertPos(
            -sx * frameT / 2.0,
            -sy * cornerArm / 2.0,
            0
        );

        auto bracket = new G4UnionSolid(
            name + "_LBracket",
            horiz,
            vert,
            nullptr,
            vertPos - horizPos
        );

        auto dummy = new G4Box(
            name + "_Dummy",
            1.0 * nm,
            1.0 * nm,
            1.0 * nm
        );

        return new G4UnionSolid(
            name + "_Solid",
            dummy,
            bracket,
            nullptr,
            horizPos
        );
    };

    // --------------------------------------------------------
    // Per-corner geometry + boundary parameters
    // --------------------------------------------------------

    struct CornerDef {
        G4String name;
        G4double sx;
        G4double sy;
        G4ThreeVector pos;

        // Surface constructor parameters
        G4double p0;
        G4double p1;
        G4double p2;
        G4double p3;
        G4double p4;
        G4double p5;
        G4double p6;
        G4double p7;
    };

    std::vector<CornerDef> corners = {
        {
            "TopRight",
            +1.0,
            +1.0,
            G4ThreeVector(+outerHalf, +outerHalf, zPos),
            0.0, 1.0, 0.0, 1.0,
            CTR, 1.0, 0.0, 0.0
        },
        {
            "TopLeft",
            -1.0,
            +1.0,
            G4ThreeVector(-outerHalf, +outerHalf, zPos),
            0.0, 1.0, 0.0, 1.0,
            CTL, 1.0, 0.0, 0.0
        },
        {
            "BottomLeft",
            -1.0,
            -1.0,
            G4ThreeVector(-outerHalf, -outerHalf, zPos),
            0.0, 1.0, 0.0, 1.0,
            CBL, 1.0, 0.0, 0.0
        },
        {
            "BottomRight",
            +1.0,
            -1.0,
            G4ThreeVector(+outerHalf, -outerHalf, zPos),
            0.0, 1.0, 0.0, 1.0,
            CBR, 1.0, 0.0, 0.0
        }
    };

    // --------------------------------------------------------
    // Scattering properties
    // --------------------------------------------------------

    const G4double GHz = 1e9 * hertz;

    const std::vector<G4double> anhCoeffs =
        {0, 0, 0, 0, 0, 1.51e-14};

    const std::vector<G4double> diffCoeffs =
        {5.88e-2, 7.83e-4, -2.47e-6, 1.71e-8, -2.98e-11};

    const std::vector<G4double> specCoeffs =
        {0, 928, -2.03e-4, -3.21e-6, 3.1e-9, 2.9e-13};

    const G4double anhCutoff  = 520.;
    const G4double reflCutoff = 350.;

    // --------------------------------------------------------
    // Build, place, and assign unique boundary properties
    // --------------------------------------------------------

    for (size_t i = 0; i < corners.size(); i++) {

        const auto& c = corners[i];

        G4VSolid* cornerSolid = MakeCornerBracket(
            c.name + "CuHolder",
            frameT,
            cornerArm,
            zThick,
            c.sx,
            c.sy
        );

        auto cornerLogical = new G4LogicalVolume(
            cornerSolid,
            fCu,
            c.name + "CuHolderLogical"
        );

        auto cornerPhys = new G4PVPlacement(
            nullptr,
            c.pos,
            cornerLogical,
            c.name + "CuHolderPhysical",
            worldLogical,
            false,
            i,
            true
        );

        auto holdSurfProp = new G4CMPSurfaceProperty(
            "CuHolderSurface_" + c.name,
            c.p0, c.p1, c.p2, c.p3,
            c.p4, c.p5, c.p6, c.p7
        );

        holdSurfProp->AddScatteringProperties(
            anhCutoff,
            reflCutoff,
            anhCoeffs,
            diffCoeffs,
            specCoeffs,
            GHz,
            GHz,
            GHz
        );

        new G4CMPLogicalBorderSurface(
            "SiToCuHolder_" + c.name,
            SiPhys,
            cornerPhys,
            holdSurfProp
        );

        new G4CMPLogicalBorderSurface(
            "CuHolderToSi_" + c.name,
            cornerPhys,
            SiPhys,
            holdSurfProp
        );
    }
    // Set up lattice properties for silicon
    G4LatticeManager* LM = G4LatticeManager::GetLatticeManager();
    G4LatticeLogical* SiLogical = LM->LoadLattice(fSilicon, "Si");
    G4LatticePhysical* SiPhysical = new G4LatticePhysical(SiLogical);
    SiPhysical->SetMillerOrientation(1, 0, 0);
    LM->RegisterLattice(SiPhys, SiPhysical);

//    G4LatticeLogical* SiImplantLogical = LM->LoadLattice(fSilicon2, "Si");
  //  G4LatticePhysical* SiImplantPhysical = new G4LatticePhysical(SiImplantLogical);
//    SiImplantPhysical->SetMillerOrientation(1, 0, 0);
  //  LM->RegisterLattice(SiPhysImplant, SiImplantPhysical);

   // G4CMPConfigManager* config_manager = G4CMPConfigManager::Instance();
  //  const G4VNIELPartition* nielPartition = config_manager->GetNIELPartition(); 
    //for (int i=1; i<101; i++) {}
    // Set up the aluminum superconducting thin films
    /// Below is the first try, where I simplified it as a whole film covering the whole top/bottom surface.
    std::vector<double> xvals = readCSV("/sdf/home/c/cstonewh/fpd-beest/users/cstonewh/MEMSgds/src/xvals.csv");
    std::vector<double> yvals = readCSV("/sdf/home/c/cstonewh/fpd-beest/users/cstonewh/MEMSgds/src/yvals.csv");

    if (xvals.size() != yvals.size()) {
       throw std::runtime_error("xvals and yvals have different lengths");
   }

   //4VSolid* fAluminumTopSolid = new G4Box("aluminumTopSolid",
     //                                     139*um/2.,
       //                                   283*um/2.,
         //                                 dp_aluminumTopFilmDimZ/2.
          //                       );

    G4VSolid* fAluminumBottomSolid = new G4Box("aluminumBottomSolid",
                                                200*um/2.,
                                                200*um/2.,
                                                dp_aluminumTopFilmDimZ/2.
                                            );
    G4LogicalVolume* fAluminumBottomLogical =
        new G4LogicalVolume(fAluminumBottomSolid, fAluminum, "fAluminumBottomLogical");
   // G4VPhysicalVolume* aluminumTopPhysical = new G4PVPlacement(0,
     //                                                       G4ThreeVector(0., 0., (dp_siliconChipDimZ+dp_aluminumFilmDimZ)/2.),
       //                                                     fAluminumLogical,
           //                                                 "fAluminumPhysical",
         //                                                   worldLogical,
             //                                               false,
    //                                                        0);

    // centers in um
    std::vector<G4ThreeVector> squarePositions = {
        G4ThreeVector(-3146.0*um,   742.5*um,   0*um),
        G4ThreeVector(-3146.0*um,  2821.5*um,   0*um),
        G4ThreeVector(-3432.0*um, -2821.5*um,   0*um),
        G4ThreeVector(-3432.0*um,  -742.5*um,   0*um)
    };

    int sqID = 10000;  // keep copy numbers distinct from the regular pixels
    for (const auto& pos : squarePositions) {
        ++sqID;

        G4VPhysicalVolume* aluminumBotPhysical = new G4PVPlacement(
            0,
            G4ThreeVector(pos.x(), pos.y(), (dp_siliconChipDimZ + dp_aluminumTopFilmDimZ + 0.5*nm)/2.),
            fAluminumBottomLogical,
            "BondPad",
            worldLogical,
            false,
            sqID
        );

        const G4double GHz = 1e9 * hertz;
        const std::vector<G4double> anhCoeffs = {0, 0, 0, 0, 0, 1.51e-14};
        const std::vector<G4double> diffCoeffs =
          {5.88e-2, 7.83e-4, -2.47e-6, 1.71e-8, -2.98e-11};
        const std::vector<G4double> specCoeffs =
          {0,928, -2.03e-4, -3.21e-6, 3.1e-9, 2.9e-13};

        const G4double anhCutoff = 520., reflCutoff = 350.;   // Units external

        botSurfProp = new G4CMPSurfaceProperty("BotAlSurf", 0.0, 1.0, 0.0, 1.0,
    					   	        0.38, 1.0, 0.0, 0.0);
        botSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
    					 diffCoeffs, specCoeffs, GHz, GHz, GHz);
        //AttachPhononSensor(botSurfProp);
        new G4CMPLogicalBorderSurface("detBot", SiPhys, aluminumBotPhysical,
		    		botSurfProp);
        new G4CMPLogicalBorderSurface("detBot", aluminumBotPhysical, SiPhys,
		    		botSurfProp);

        //wallSurfProp = new G4CMPSurfaceProperty("WallSurf", 0.0, 1.0, 0.0, 0.0,
    					    	       //   0.0, 1.0, 0.0, 0.0);
        //wallSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
			//		  diffCoeffs, specCoeffs, GHz, GHz,GHz);
        //new G4CMPLogicalBorderSurface("detWall", SiPhys, aluminumBotPhysical,
		    		//wallSurfProp);
        }

    
    // Below is where the two readout channels are implemented.
    G4VSolid* fAluminumInnerTopSolid = new G4Box("aluminumTopSolid",
                                            139*um/2.,
                                            283*um/2.,
                                            dp_aluminumTopFilmDimZ/2.
                                    );

    G4LogicalVolume* fAluminumInnerTopLogical = 
                    new G4LogicalVolume(fAluminumInnerTopSolid, fAluminum, "fAluminumInnerTopLogical");
    int i = 0;

    for (size_t k = 0; k < xvals.size(); k++)
    {
        i++;

        G4VPhysicalVolume* aluminumInnerTopPhysical = new G4PVPlacement( 0, 
                                                                    G4ThreeVector(xvals[k]*um, yvals[k]*um, (dp_siliconChipDimZ + dp_aluminumTopFilmDimZ)/2.), 
                                                                    fAluminumInnerTopLogical,
                                                                    "fAluminumInnerTopPhysical",
                                                                    worldLogical,
                                                                    false,
                                                                    i
                                                                  );

        const G4double GHz = 1e9 * hertz;
        const std::vector<G4double> anhCoeffs = {0, 0, 0, 0, 0, 1.51e-14};
        const std::vector<G4double> diffCoeffs =
          {5.88e-2, 7.83e-4, -2.47e-6, 1.71e-8, -2.98e-11};
        const std::vector<G4double> specCoeffs =
          {0,928, -2.03e-4, -3.21e-6, 3.1e-9, 2.9e-13};

        const G4double anhCutoff = 520., reflCutoff = 350.;   // Units external

        topSurfProp = new G4CMPSurfaceProperty("TopAlSurf", 0.0, 1.0, 0.0, 1.0, // the first 4 entries deal with e/h absorption and reflection
    					  	        0.0, 1.0, 0.0, 1.0); // the last 4 entries deal with phonon absorption and reflection, [absorption, reflection, specular reflection ratio, ...]
        //                            0.77, 1.0, 0.0, 0.0);
        topSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
    					 diffCoeffs, specCoeffs, GHz, GHz, GHz);
        if (xvals[k] > 0){AttachPhononSensor(topSurfProp);}
        if (xvals[k] <= 0){AttachPhononSensor2(topSurfProp);}
       

        new G4CMPLogicalBorderSurface("detInnerTop", SiPhys, aluminumInnerTopPhysical,
		    		topSurfProp);
        new G4CMPLogicalBorderSurface("detInnerTop", aluminumInnerTopPhysical, SiPhys, 
		    		topSurfProp);

       // wallSurfProp = new G4CMPSurfaceProperty("WallSurf", 0.0, 1.0, 0.0, 0.0,
    	//				    	          0.0, 1.0, 0.0, 0.0);
      //  wallSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
		//			  diffCoeffs, specCoeffs, GHz, GHz,GHz);
       // new G4CMPLogicalBorderSurface("detWall2", SiPhys, aluminumInnerTopPhysical,
		//    		wallSurfProp);
    }

    G4Box* fAluminumGridSolid = new G4Box("fAluminumGrid", 
                                    dp_siliconChipDimX/2., 
                                    dp_siliconChipDimY/2., 
                                    dp_aluminumTopFilmDimZ/2.);
    G4LogicalVolume* fAluminumGridLogical = 
                    new G4LogicalVolume(fAluminumGridSolid, fAluminum, "fAluminumGridLogical");
    G4VPhysicalVolume* aluminumGridPhysical = new G4PVPlacement( 0, 
                                                                    G4ThreeVector(0., 0., (-dp_siliconChipDimZ - dp_aluminumTopFilmDimZ -0.01*nm)/2.), 
                                                                    fAluminumGridLogical,
                                                                    "fAluminumGridPhysical",
                                                                    worldLogical,
                                                                    false,
                                                                    0
                                                                  );


    // detector -- Note: "sensitive detector" is atttached to the silicon substrate
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    if (!electrodeSensitivity) {
        electrodeSensitivity = new HVeVSensitivity("Electrode");
        SDman->AddNewDetector(electrodeSensitivity);
        fSiliconLogical->SetSensitiveDetector(electrodeSensitivity);
//        fSiliconLogicalImplant->SetSensitiveDetector(electrodeSensitivity);
    }
    
    // Set up the electric field across the silicon substrate
    AttachField(fSiliconLogical);

    if (!fConstructed) {
        const G4double GHz = 1e9 * hertz;
        const std::vector<G4double> anhCoeffs = {0, 0, 0, 0, 0, 1.51e-14};
        const std::vector<G4double> diffCoeffs =
          {5.88e-2, 7.83e-4, -2.47e-6, 1.71e-8, -2.98e-11};
        const std::vector<G4double> specCoeffs =
          {0,928, -2.03e-4, -3.21e-6, 3.1e-9, 2.9e-13};

        const G4double anhCutoff = 520., reflCutoff = 350.;   // Units external

   //     topSurfProp = new G4CMPSurfaceProperty("TopAlSurf", 1.0, 0.0, 0.0, 0.0, // the first 4 entries deal with e/h absorption and reflection
    //					  	        0.34, 1.0, 0.0, 0.0); // the last 4 entries deal with phonon absorption and reflection, [absorption, reflection, specular reflection ratio, ...]
      //  topSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
    		//			 diffCoeffs, specCoeffs, GHz, GHz, GHz);
  //      AttachPhononSensor(topSurfProp);

        gridSurfProp = new G4CMPSurfaceProperty("GridAlSurf", 0.0, 1.0, 0.0, 1.0,
    					   	        0.00, 1.0, 0.0, 0.0);
                          //0.0015, 1.0, 0.0, 0.0);
        gridSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
    					 diffCoeffs, specCoeffs, GHz, GHz, GHz);
        AttachGrid(gridSurfProp);
       // AttachPhononSensor(topSurfProp);

        wallSurfProp = new G4CMPSurfaceProperty("WallSurf", 0.0, 1.0, 0.0, 1.0,
    					    	          0.001, 1.0, 0.0, 0.0);
        wallSurfProp->AddScatteringProperties(anhCutoff, reflCutoff, anhCoeffs,
					  diffCoeffs, specCoeffs, GHz, GHz,GHz);
       
    }

    // One whole film implementation below,
  
    new G4CMPLogicalBorderSurface("detGrid", SiPhys, aluminumGridPhysical,
		    		gridSurfProp);

    new G4CMPLogicalBorderSurface("detGrid", aluminumGridPhysical, SiPhys,
		    		gridSurfProp);


  //  new G4CMPLogicalBorderSurface("detTop", SiPhys, SiPhysImplant,
    //                            topSurfProp);

   // new G4CMPLogicalBorderSurface("detInnerTop", SiPhys, aluminumInnerTopPhysical,
		 //   		botSurfProp);
   // new G4CMPLogicalBorderSurface("detOuterTop", SiPhys, aluminumOuterTopPhysical,
		 //   		botSurfProp);
   // //new G4CMPLogicalBorderSurface("detBot", SiPhys, aluminumBotPhysical,
		    //		botSurfProp);
    new G4CMPLogicalBorderSurface("detWall", SiPhys, fWorldPhys,
		    		wallSurfProp);
    new G4CMPLogicalBorderSurface("detWall", aluminumGridPhysical, fWorldPhys,
		    		wallSurfProp);
    new G4CMPLogicalBorderSurface("detWall", fWorldPhys, SiPhys,
		    		wallSurfProp);
 //   new G4CMPLogicalBorderSurface("detWall", SiPhysImplant, fWorldPhys,
   //                             wallSurfProp);

    //                                        
    // Visualization attributes
    //
    worldLogical->SetVisAttributes(G4VisAttributes::Invisible);
    G4VisAttributes* simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
    simpleBoxVisAtt->SetVisibility(true);
    fSiliconLogical->SetVisAttributes(simpleBoxVisAtt);
    fAluminumInnerTopLogical->SetVisAttributes(simpleBoxVisAtt);
    //fAluminumOuterTopLogical->SetVisAttributes(simpleBoxVisAtt);
  //  fAluminumBottomLogical->SetVisAttributes(simpleBoxVisAtt);
}

void HVeVDetectorConstruction::
AttachPhononSensor(G4CMPSurfaceProperty* surfProp)
{
    if (!surfProp) return;

    auto sensorProp = surfProp->GetPhononMaterialPropertiesTablePointer();
   // sensorProp->AddConstProperty("filmAbsorption", 0.0);    // True sensor area
    sensorProp->AddConstProperty("filmAbsorption", 0.77);    // True sensor area
    sensorProp->AddConstProperty("filmThickness", 600.*nm);
    sensorProp->AddConstProperty("gapEnergy", 173.715e-6*eV);
    sensorProp->AddConstProperty("lowQPLimit", 3.);
    sensorProp->AddConstProperty("highQPLimit", 10.);
    sensorProp->AddConstProperty("phononLifetime", 242.*ps);
    sensorProp->AddConstProperty("phononLifetimeSlope", 0.29);
    sensorProp->AddConstProperty("vSound", 3.26*km/s);
    sensorProp->AddConstProperty("subgapAbsorption", 0.1);

    G4CMPPhononElectrode* electrode = new G4CMPPhononElectrode();
    electrode->SetVerboseLevel(2);
    
    //HVeVPhononElectrode* electrode = new HVeVPhononElectrode();
    //electrode->SetVerboseLevel(2);
    surfProp->SetPhononElectrode(electrode);

    // Attach electrode object to handle KaplanQP interface
    //surfProp->SetPhononElectrode(new G4CMPPhononElectrode);
    // TEST if the electrode object is attached
    //G4CMPPhononElectrode* electrode1 = surfProp->GetPhononElectrode();
  //  G4CMPVElectrodePattern* electrode1 = surfProp->GetPhononElectrode();
}
void HVeVDetectorConstruction::
AttachPhononSensor2(G4CMPSurfaceProperty* surfProp)
{
    if (!surfProp) return;

    auto sensorProp = surfProp->GetPhononMaterialPropertiesTablePointer();
   // sensorProp->AddConstProperty("filmAbsorption", 0.0);    // True sensor area
    sensorProp->AddConstProperty("filmAbsorption", 0.7);    // True sensor area
    sensorProp->AddConstProperty("filmThickness", 600.*nm);
    sensorProp->AddConstProperty("gapEnergy", 173.715e-6*eV);
    sensorProp->AddConstProperty("lowQPLimit", 3.);
    sensorProp->AddConstProperty("highQPLimit", 10.);
    sensorProp->AddConstProperty("phononLifetime", 242.*ps);
    sensorProp->AddConstProperty("phononLifetimeSlope", 0.29);
    sensorProp->AddConstProperty("vSound", 3.26*km/s);
    sensorProp->AddConstProperty("subgapAbsorption", 0.1);

    G4CMPPhononElectrode* electrode = new G4CMPPhononElectrode();
    electrode->SetVerboseLevel(2);
    
    //HVeVPhononElectrode* electrode = new HVeVPhononElectrode();
    //electrode->SetVerboseLevel(2);
    surfProp->SetPhononElectrode(electrode);

    // Attach electrode object to handle KaplanQP interface
    //surfProp->SetPhononElectrode(new G4CMPPhononElectrode);
    // TEST if the electrode object is attached
    //G4CMPPhononElectrode* electrode1 = surfProp->GetPhononElectrode();
  //  G4CMPVElectrodePattern* electrode1 = surfProp->GetPhononElectrode();
}

void HVeVDetectorConstruction::
AttachGrid(G4CMPSurfaceProperty* surfProp)
{
    if (!surfProp) return;
    auto sensorProp = surfProp->GetPhononMaterialPropertiesTablePointer();
    sensorProp->AddConstProperty("filmAbsorption", 0.05);    // True sensor area
    //sensorProp->AddConstProperty("filmAbsorption", 0.0);    // True sensor area
    sensorProp->AddConstProperty("filmThickness", 30.*nm);
    sensorProp->AddConstProperty("gapEnergy", 173.715e-6*eV);
    sensorProp->AddConstProperty("lowQPLimit", 3.);
    sensorProp->AddConstProperty("highQPLimit", 10.);
    sensorProp->AddConstProperty("phononLifetime", 242.*ps);
    sensorProp->AddConstProperty("phononLifetimeSlope", 0.29);
    sensorProp->AddConstProperty("vSound", 3.26*km/s);
    sensorProp->AddConstProperty("subgapAbsorption", 0.1);

    // Attach electrode object to handle KaplanQP interface
    surfProp->SetPhononElectrode(new G4CMPPhononElectrode);
    // TEST if the electrode object is attached
    G4CMPVElectrodePattern* electrode = surfProp->GetPhononElectrode();
    electrode->SetVerboseLevel(2);

}

void HVeVDetectorConstruction::AttachField(G4LogicalVolume* lv)
{
    if (!fEMField) {
        //voltage = 0*volt;
        G4double fieldMag = -voltage / dp_siliconChipDimZ;
        //std::cout << "Voltage: "<<voltage << std::endl;
        fEMField = new G4UniformElectricField(G4ThreeVector(0., 0., fieldMag));
    }
    if (!lv->GetFieldManager()) {
        G4FieldManager* fFieldMgr = new G4CMPFieldManager(fEMField);
        lv->SetFieldManager(fFieldMgr, true);
    }
    lv->GetFieldManager()->SetDetectorField(fEMField);
    std::cout << "Applied bias voltage " << voltage << " volt across the silicon substrate" << std::endl;
}
