#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4GlobalMagFieldMessenger.hh"

#include "DetectorConstruction.hh"
#include "SteppingAction.hh"

void DetectorConstruction::ConstructHCAL()
{
    G4cout << "Construction of AHCAL begins now..." << G4endl;
    G4double ecal_length = 0.0 * mm;
    if (config->conf["Geometry"]["build_ECAL"].as<G4bool>())
        ecal_length = 300.0 * mm;
    G4cout << "AHCAL is constructed at z = " << ecal_length << " mm." << G4endl;

    // Parameters from YAML file
    G4int nLayer = config->conf["HCAL"]["nLayer"].as<G4int>();
    G4int nCellX = config->conf["HCAL"]["nCellX"].as<G4int>();
    G4int nCellY = config->conf["HCAL"]["nCellY"].as<G4int>();
    G4double crystalX = config->conf["HCAL"]["CellWidthX"].as<G4double>() * mm;
    G4double crystalY = config->conf["HCAL"]["CellWidthY"].as<G4double>() * mm;
    G4double gapX = config->conf["HCAL"]["GapX"].as<G4double>() * mm;
    G4double gapY = config->conf["HCAL"]["GapY"].as<G4double>() * mm;

    G4NistManager* nistManager = G4NistManager::Instance();

    // Elements and basic materials
    G4Element* elH = nistManager->FindOrBuildElement("H");
    G4Element* elC = nistManager->FindOrBuildElement("C");
    G4Element* elO = nistManager->FindOrBuildElement("O");
    G4Element* elP = nistManager->FindOrBuildElement("P");
    G4Element* elS = nistManager->FindOrBuildElement("S");
    G4Element* elMn = nistManager->FindOrBuildElement("Mn");
    G4Material* Si = nistManager->FindOrBuildMaterial("G4_Si");
    G4Material* Fe = nistManager->FindOrBuildMaterial("G4_Fe");

    // Absorber: steel
    G4Material* steel = new G4Material("steel", 7.85 * g / cm3, 6);
    steel->AddElement(elC, 0.22 * perCent);
    steel->AddElement(elMn, 1.4 * perCent);
    steel->AddMaterial(Si, 0.35 * perCent);
    steel->AddElement(elP, 0.045 * perCent);
    steel->AddElement(elS, 0.05 * perCent);
    steel->AddMaterial(Fe, 97.935 * perCent);

    // Wrapper: ESR
    G4Material* ESR = new G4Material("ESR", 0.9 * g / cm3, 2);
    ESR->AddElement(elC, 2);
    ESR->AddElement(elH, 4);

    // Plastic scintillator: polystyrene
    G4Material* plastic = nistManager->BuildMaterialWithNewDensity("plastic", "G4_POLYSTYRENE", 1.032 * g / cm3);
    plastic->GetIonisation()->SetBirksConstant(0.07943 * mm / MeV);

    // PCB: FR4
    G4Material* quartz = nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    G4Material* epoxy = new G4Material("epoxy", 1.3 * g / cm3, 3);
    epoxy->AddElement(elC, 15);
    epoxy->AddElement(elH, 44);
    epoxy->AddElement(elO, 7);
    G4Material* FR4 = new G4Material("FR4", 1.86 * g / cm3, 2);
    FR4->AddMaterial(quartz, 52.8 * perCent);
    FR4->AddMaterial(epoxy, 47.2 * perCent);

    /*
     * Standard structure:
     * First layer: 2-mm absorber  ---  1-mm gap  ---  0.25-mm wrapper  ---  3-mm scintillator  ---  0.25-mm wrapper  ---  2.5-mm PCB  ---  4-mm gap  ---  20-mm absorber.
     * Following layers: Without the 2-mm absorber and the 1-mm gap.
     * Scintillator size: 40 * 40 * 3 mm^3.
     * Thickness per layer: 30 mm.
     * Wrapper in x and y directions are in the gap; 0.15-mm thick.
     */

    // Parameters of the components
    G4double absorberZ0 = 2.0 * mm;
    G4double gap_psd_abs0 = 0.1 * mm;
    G4double crystalZ = 3.0 * mm;
    G4double ESRThick = 0.25 * mm;

    G4double ESRInX = crystalX;
    G4double ESRInY = crystalY;
    G4double ESRInZ = crystalZ;
    G4double ESROutX = ESRInX + gapX;
    G4double ESROutY = ESRInY + gapY;
    G4double ESROutZ = ESRInZ + 2 * ESRThick;

    G4double PCBX = nCellX * ESROutX;
    G4double PCBY = nCellY * ESROutY;
    G4double PCBZ = 2.5 * mm;
    G4double gapZ = 4.0 * mm;
    G4double absorberX = PCBX;
    G4double absorberY = PCBY;
    G4double absorberZ = 20.0 * mm;
    G4double thickness = ESROutZ + PCBZ + gapZ + absorberZ;

    // Positions
    G4double absorberPositionZ0 = ecal_length + 0.5 * absorberZ0;
    G4double crystalPositionZ = absorberPositionZ0 + 0.5 * absorberZ0 + gap_psd_abs0 + 0.5 * ESROutZ;
    G4double ESRPositionZ = crystalPositionZ;
    G4double PCBPositionZ = ESRPositionZ + 0.5 * (ESROutZ + PCBZ);
    G4double absorberPositionZ = PCBPositionZ + 0.5 * PCBZ + gapZ + 0.5 * absorberZ;

    G4bool checkOverlap = false;    // No overlap checking triggered

    // Absorber
    G4Box* solidAbsorber = new G4Box("hcal_absorber",                                       // Name
			                         0.5 * absorberX, 0.5 * absorberY, 0.5 * absorberZ);    // Size
    G4Box* solidAbsorber0 = new G4Box("hcal_absorber0",                                       // Name
                                      0.5 * absorberX, 0.5 * absorberY, 0.5 * absorberZ0);    // Size

    G4LogicalVolume* logicAbsorber = new G4LogicalVolume(solidAbsorber,       // Solid
					                                     steel,               // Material
                                                         "hcal_absorber");    // Name
    G4LogicalVolume* logicAbsorber0 = new G4LogicalVolume(solidAbsorber0,       // Solid
                                                          steel,                // Material
                                                          "hcal_absorber0");    // Name

    G4VPhysicalVolume* physiAbsorber = new G4PVPlacement(0,                   // No rotation
                                                         G4ThreeVector(0, 0, absorberPositionZ0),
                                                         logicAbsorber0,      // Logical volume
                                                         "hcal_absorber0",    // Name
                                                         logicWorld,          // Mother volume
                                                         false,               // No boolean operations
                                                         -1,                  // Copy number
                                                         checkOverlap);
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        physiAbsorber = new G4PVPlacement(0,                  // No rotation
                                          G4ThreeVector(0, 0, (absorberPositionZ + i_Layer * thickness)),
                                          logicAbsorber,      // Logical volume
                                          "hcal_absorber",    // Name
                                          logicWorld,         // Mother volume
                                          false,              // No boolean operations
                                          -1,                 // Copy number
                                          checkOverlap);
    }

    // Active layer & wrapper
    G4Box* solidCrystal = new G4Box("hcal_psd",                                         // Name
			                        0.5 * crystalX, 0.5 * crystalY, 0.5 * crystalZ);    // Size
    G4Box* solidESROut = new G4Box("ESR_out",                                       // Name
                                   0.5 * ESROutX, 0.5 * ESROutY, 0.5 * ESROutZ);    // Size
    G4Box* solidESRIn = new G4Box("ESR_in",                                      // Name
                                   0.5 * ESRInX, 0.5 * ESRInY, 0.5 * ESRInZ);    // Size
    // ESROut - ESRIn
    G4SubtractionSolid* solidESR = new G4SubtractionSolid("ESR",          // Name
                                                          solidESROut,    // Minuend
                                                          solidESRIn);    // Subtrahend

    G4LogicalVolume* logicCrystal = new G4LogicalVolume(solidCrystal,    // Solid
                                                        plastic,         // Material
                                                        "hcal_psd");     // Name
    G4LogicalVolume* logicESR = new G4LogicalVolume(solidESR,    // Solid
                                                    ESR,         // Material
                                                    "ESR");      // Name

    G4VPhysicalVolume* physiCrystal;
    G4VPhysicalVolume* physiESR;
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        for (G4int i_Y = 0; i_Y < nCellY; ++i_Y)
        {
            for (G4int i_X = 0; i_X < nCellX; ++i_X)
            {
                physiCrystal = new G4PVPlacement(0,                                     // No rotation
                                                 G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESROutX,
                                                               -0.5 * PCBY + (i_Y + 0.5) * ESROutY,
                                                               crystalPositionZ + i_Layer * thickness),
                                                 logicCrystal,                          // Logical volume
                                                 "hcal_psd",                            // Name
                                                 logicWorld,                            // Mother volume
                                                 false,                                 // No boolean operations
                                                 i_Layer * 100000 + i_X * 100 + i_Y,    // Copy number
                                                 checkOverlap);

                physiESR = new G4PVPlacement(0,             // No rotation
                                             G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESROutX,
                                                           -0.5 * PCBY + (i_Y + 0.5) * ESROutY,
                                                           ESRPositionZ + i_Layer * thickness),
                                             logicESR,      // Logical volume
                                             "ESR",         // Name
                                             logicWorld,    // Mother volume
                                             false,         // No boolean operations
                                             -1,            // Copy number
                                             checkOverlap);
            }
        }
    }

    // PCB
    G4Box* solidPCB = new G4Box("hcal_pcb",                             // Name
                                0.5 * PCBX, 0.5 * PCBY, 0.5 * PCBZ);    // Size

    G4LogicalVolume* logicPCB = new G4LogicalVolume(solidPCB,       // Solid
                                                    FR4,            // Material
                                                    "hcal_pcb");    // Name

    G4VPhysicalVolume* physiPCB;
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        physiPCB = new G4PVPlacement(0,             // No rotation
                                     G4ThreeVector(0, 0, (PCBPositionZ + i_Layer * thickness)),
                                     logicPCB,      // Logical volume
                                     "hcal_pcb",    // Name
                                     logicWorld,    // Mother volume
                                     false,         // No boolean operations
                                     -1,            // Copy number
                                     checkOverlap);
    }
}