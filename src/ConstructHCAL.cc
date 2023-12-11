#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
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

    // ESR
    G4Material* ESR = new G4Material("ESR", 0.9 * g / cm3, 2);
    ESR->AddElement(elC, 2);
    ESR->AddElement(elH, 4);

    // Plastic scintillator: polystyrene
//    G4Material* PSD = nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
//    PSD->GetIonisation()->SetBirksConstant(0.07943 * mm / MeV);
//    G4cout << "GetNuclearInterLength " << PSD->GetNuclearInterLength() << G4endl;
    G4Material* plastic = nistManager->BuildMaterialWithNewDensity("plastic", "G4_POLYSTYRENE", 1.032 * g / cm3);
    plastic->GetIonisation()->SetBirksConstant(0.07943 * mm / MeV);

    // PCB: FR4
//    G4Material* kevlar = nistManager->FindOrBuildMaterial("G4_KAPTON");
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
     * First layer: 2-mm absorber  ---  1-mm gap  ---  0.25-mm ESR  ---  3-mm scintillator  ---  0.25-mm ESR  ---  2.5-mm PCB  ---  4-mm gap  ---  20-mm absorber.
     * Following layers: Without the 2-mm absorber and the 1-mm gap.
     * Scintillator size: 40 * 40 * 3 mm^3.
     * Thickness per layer: 30 mm.
     * ESR in x and y directions are in the gap; 0.15-mm thick.
     */

    // Parameters of the components
    G4double absorberZ0 = 2.0 * mm;
    G4double gap_psd_abs0 = 0.1 * mm;
    G4double crystalZ = 3.0 * mm;

    G4double ESRFrontBackX = crystalX + gapX;
    G4double ESRFrontBackY = crystalY + gapY;
    G4double ESRFrontBackZ = 0.25 * mm;
    G4double ESRLeftRightX = 0.5 * gapX;
    G4double ESRLeftRightY = ESRFrontBackY;
    G4double ESRLeftRightZ = crystalZ;
    G4double ESRUpDownX = crystalX;
    G4double ESRUpDownY = 0.5 * gapY;
    G4double ESRUpDownZ = crystalZ;

    G4double PCBX = nCellX * ESRFrontBackX;
    G4double PCBY = nCellY * ESRFrontBackY;
    G4double PCBZ = 2.5 * mm;
    G4double gapZ = 4.0 * mm;
    G4double absorberX = PCBX;
    G4double absorberY = PCBY;
    G4double absorberZ = 20.0 * mm;
    G4double thickness = ESRFrontBackZ + crystalZ + ESRFrontBackZ + PCBZ + gapZ + absorberZ;

    // Positions
    G4double absorberPositionZ0 = ecal_length + 0.5 * absorberZ0;
    G4double ESRFrontPositionZ = absorberPositionZ0 + 0.5 * absorberZ0 + gap_psd_abs0 + 0.5 * ESRFrontBackZ;
    G4double crystalPositionZ = ESRFrontPositionZ + 0.5 * (ESRFrontBackZ + crystalZ);
    G4double ESRBackPositionZ = crystalPositionZ + 0.5 * (crystalZ + ESRFrontBackZ);
    G4double PCBPositionZ = ESRBackPositionZ + 0.5 * (ESRFrontBackZ + PCBZ);
    G4double absorberPositionZ = PCBPositionZ + 0.5 * PCBZ + gapZ + 0.5 * absorberZ;

    G4bool checkOverlap = false;    // no overlap checking triggered

    // Absorber
    G4Box* solidAbsorber = new G4Box("hcal_absorber",                                       // name
			                         0.5 * absorberX, 0.5 * absorberY, 0.5 * absorberZ);    // size
    G4Box* solidAbsorber0 = new G4Box("hcal_absorber0",                                       // name
                                      0.5 * absorberX, 0.5 * absorberY, 0.5 * absorberZ0);    // size

    G4LogicalVolume* logicAbsorber = new G4LogicalVolume(solidAbsorber,       // solid
					                                     steel,               // material
                                                         "hcal_absorber");    // name
    G4LogicalVolume* logicAbsorber0 = new G4LogicalVolume(solidAbsorber0,       // solid
                                                          steel,                // material
                                                          "hcal_absorber0");    // name

    G4VPhysicalVolume* physiAbsorber = new G4PVPlacement(0,                   // no rotation
                                                         G4ThreeVector(0, 0, absorberPositionZ0),
                                                         logicAbsorber0,      // logical volume
                                                         "hcal_absorber0",    // name
                                                         logicWorld,          // mother volume
                                                         false,               // no boolean operations
                                                         -1,                  // copy number
                                                         checkOverlap);
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        physiAbsorber = new G4PVPlacement(0,                  // no rotation
                                          G4ThreeVector(0, 0, (absorberPositionZ + i_Layer * thickness)),
                                          logicAbsorber,      // logical volume
                                          "hcal_absorber",    // name
                                          logicWorld,         // mother volume
                                          false,              // no boolean operations
                                          -1,                 // copy number
                                          checkOverlap);
    }

    // Active layer & ESR
    G4Box* solidCrystal = new G4Box("hcal_psd",                                         // name
			                        0.5 * crystalX, 0.5 * crystalY, 0.5 * crystalZ);    // size
    G4Box* solidESRFrontBack = new G4Box("ESR_frontback",                                                   // name
                                         0.5 * ESRFrontBackX, 0.5 * ESRFrontBackY, 0.5 * ESRFrontBackZ);    // size
    G4Box* solidESRLeftRight = new G4Box("ESR_leftright",                                                   // name
                                         0.5 * ESRLeftRightX, 0.5 * ESRLeftRightY, 0.5 * ESRLeftRightZ);    // size
    G4Box* solidESRUpDown = new G4Box("ESR_updown",                                             // name
                                      0.5 * ESRUpDownX, 0.5 * ESRUpDownY, 0.5 * ESRUpDownZ);    // size

    G4LogicalVolume* logicCrystal = new G4LogicalVolume(solidCrystal,    // solid
                                                        plastic,         // material
//                                                        PSD,             // material
                                                        "hcal_psd");     // name
    G4LogicalVolume* logicESRFrontBack = new G4LogicalVolume(solidESRFrontBack,    //solid
                                                             ESR,                  // material
                                                             "ESR_frontback");     // name
    G4LogicalVolume* logicESRLeftRight = new G4LogicalVolume(solidESRLeftRight,    //solid
                                                             ESR,                  // material
                                                             "ESR_leftright");     // name
    G4LogicalVolume* logicESRUpDown = new G4LogicalVolume(solidESRUpDown,    //solid
                                                          ESR,               // material
                                                          "ESR_updown");     // name

    G4VPhysicalVolume* physiCrystal;
    G4VPhysicalVolume* physiESRFront;
    G4VPhysicalVolume* physiESRBack;
    G4VPhysicalVolume* physiESRLeft;
    G4VPhysicalVolume* physiESRRight;
    G4VPhysicalVolume* physiESRDown;
    G4VPhysicalVolume* physiESRUp;
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        for (G4int i_Y = 0; i_Y < nCellY; ++i_Y)
        {
            for (G4int i_X = 0; i_X < nCellX; ++i_X)
            {
                physiCrystal = new G4PVPlacement(0,                                     // no rotation
                                                 G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESRFrontBackX,
                                                               -0.5 * PCBY + (i_Y + 0.5) * ESRFrontBackY,
                                                               crystalPositionZ + i_Layer * thickness),
                                                 logicCrystal,                          // logical volume
                                                 "hcal_psd",                            // name
                                                 logicWorld,                            // mother volume
                                                 false,                                 // no boolean operations
                                                 i_Layer * 100000 + i_X * 100 + i_Y,    // copy number
                                                 checkOverlap);
                physiESRFront = new G4PVPlacement(0,                    // no rotation
                                                  G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESRFrontBackX,
                                                                -0.5 * PCBY + (i_Y + 0.5) * ESRFrontBackY,
                                                                ESRFrontPositionZ),
                                                  logicESRFrontBack,    // logical volume
                                                  "ESR_frontback",      // name
                                                  logicWorld,           // mother volume
                                                  false,                // no boolean operations
                                                  -1,                   // copy number
                                                  checkOverlap);
                physiESRBack = new G4PVPlacement(0,                    // no rotation
                                                 G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESRFrontBackX,
                                                               -0.5 * PCBY + (i_Y + 0.5) * ESRFrontBackY,
                                                               ESRBackPositionZ),
                                                 logicESRFrontBack,    // logical volume
                                                 "ESR_frontback",      // name
                                                 logicWorld,           // mother volume
                                                 false,                // no boolean operations
                                                 -1,                   // copy number
                                                 checkOverlap);
                physiESRLeft = new G4PVPlacement(0,                    // no rotation
                                                 G4ThreeVector(-0.5 * PCBX + i_X * ESRFrontBackX + 0.5 * ESRLeftRightX,
                                                               -0.5 * PCBY + (i_Y + 0.5) * ESRFrontBackY,
                                                               crystalPositionZ),
                                                 logicESRLeftRight,    // logical volume
                                                 "ESR_leftright",      // name
                                                 logicWorld,           // mother volume
                                                 false,                // no boolean operations
                                                 -1,                   // copy number
                                                 checkOverlap);
                physiESRRight = new G4PVPlacement(0,                    // no rotation
                                                  G4ThreeVector(-0.5 * PCBX + (i_X + 1) * ESRFrontBackX - 0.5 * ESRLeftRightX,
                                                                -0.5 * PCBY + (i_Y + 0.5) * ESRFrontBackY,
                                                                crystalPositionZ),
                                                  logicESRLeftRight,    // logical volume
                                                  "ESR_leftright",      // name
                                                  logicWorld,           // mother volume
                                                  false,                // no boolean operations
                                                  -1,                   // copy number
                                                  checkOverlap);
                physiESRDown = new G4PVPlacement(0,                 // no rotation
                                                 G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESRFrontBackX,
                                                               -0.5 * PCBY + i_Y * ESRFrontBackY + 0.5 * ESRUpDownY,
                                                               crystalPositionZ),
                                                 logicESRUpDown,    // logical volume
                                                 "ESR_updown",      // name
                                                 logicWorld,        // mother volume
                                                 false,             // no boolean operations
                                                 -1,                // copy number
                                                 checkOverlap);
                physiESRUp = new G4PVPlacement(0,                 // no rotation
                                               G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * ESRFrontBackX,
                                                             -0.5 * PCBY + (i_Y + 1) * ESRFrontBackY - 0.5 * ESRUpDownY,
                                                             crystalPositionZ),
                                               logicESRUpDown,    // logical volume
                                               "ESR_updown",      // name
                                               logicWorld,        // mother volume
                                               false,             // no boolean operations
                                               -1,                // copy number
                                               checkOverlap);
            }
        }
    }

    // PCB
    G4Box* solidPCB = new G4Box("hcal_pcb",                             // name
                                0.5 * PCBX, 0.5 * PCBY, 0.5 * PCBZ);    // size

    G4LogicalVolume* logicPCB = new G4LogicalVolume(solidPCB,       // solid
                                                    FR4,            // material
//                                                    kevlar,         // material // Should be checked...
                                                    "hcal_pcb");    // name

    G4VPhysicalVolume* physiPCB;
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        physiPCB = new G4PVPlacement(0,                // no rotation
                                     G4ThreeVector(0, 0, (PCBPositionZ + i_Layer * thickness)),
                                     logicPCB,         // logical volume
                                     "hcal_pcb",       // name
                                     logicWorld,       // mother volume
                                     false,            // no boolean operations
                                     -1,               // copy number
                                     checkOverlap);
    }
}