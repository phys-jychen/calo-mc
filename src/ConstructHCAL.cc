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
    G4cout << "Constructing AHCAL..." << G4endl;
    G4double ecal_length = 300.0 * mm;
    if (!config->conf["Geometry"]["build_ECAL"].as<G4bool>())
        ecal_length = 0.0 * mm;
    G4cout << "Building AHCAL at " << ecal_length << " mm." << G4endl;

    G4Material* iron = G4NistManager::Instance()->FindOrBuildMaterial("G4_Fe");
    G4Material* PSD = G4NistManager::Instance()->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    PSD->GetIonisation()->SetBirksConstant(0.07943 * mm / MeV); 
//    G4cout << "GetNuclearInterLength " << PSD->GetNuclearInterLength() << G4endl;
    G4Material* kevlar = G4NistManager::Instance()->FindOrBuildMaterial("G4_KAPTON");

    G4int nLayer = config->conf["HCAL"]["nLayer"].as<G4int>();
    G4int nCellX = config->conf["HCAL"]["nCellX"].as<G4int>();
    G4int nCellY = config->conf["HCAL"]["nCellY"].as<G4int>();
    G4double CellWidthX = config->conf["HCAL"]["CellWidthX"].as<G4double>();
    G4double CellWidthY = config->conf["HCAL"]["CellWidthY"].as<G4double>();

    G4double absorberZ0 = 2.0 * mm;
    G4double crystalX = CellWidthX * mm;
    G4double crystalY = CellWidthY * mm;
    G4double crystalZ = 3.0 * mm;
    G4double gap_psd_abs0 = 0.1 * mm;
    G4double gapX = 0.3 * mm;
    G4double gapY = 0.3 * mm;
    G4double gapZ = 5.0 * mm;
    G4double crystalPositionZ = (ecal_length + absorberZ0 + gap_psd_abs0 + 0.5 * crystalZ) * mm;
    G4double PCBX = nCellX * (CellWidthX + gapX) * mm;
    G4double PCBY = nCellY * (CellWidthY + gapY) * mm;
    G4double PCBZ = 2.0 * mm;
    G4double absorberX = PCBX;
    G4double absorberY = PCBY;
    G4double absorberZ = 20.0 * mm;
    G4double absorberPositionZ0 = (ecal_length + 0.5 * absorberZ0) * mm;
    G4double absorberPositionZ = (ecal_length + absorberZ0 + crystalZ + gapZ + 0.5 * absorberZ) * mm;
//    G4double absorberPositionZ = (ecal_length + absorberZ0 + crystalZ + PCBZ + 0.5 * absorberZ) * mm;
    G4double absorberGapZ = (absorberZ + crystalZ + gapZ) * mm;
//    G4double absorberGapZ = (absorberZ + crystalZ + PCBZ) * mm;
    G4double crystalGapZ = absorberGapZ;
    G4double PCBGapZ = absorberGapZ;
    G4double PCBPositionZ = (ecal_length + absorberZ0 + crystalZ + 0.5 * PCBZ) * mm;

    G4bool checkOverlap = false;

    // Absorber layer
    G4Box* solidAbsorber = new G4Box("hcal_absorber",                                              // name
			                         0.5 * absorberX, 0.5 * absorberY, 0.5 * absorberZ);           // size
    G4Box* solidAbsorber0 = new G4Box("hcal_absorber0",                                            // name
                                      0.5 * absorberX, 0.5 * absorberY, 0.5 * absorberZ0);         // size
    G4LogicalVolume* logicAbsorber = new G4LogicalVolume(solidAbsorber,                            // solid
					                                     iron,                                     // material
                                                         "hcal_absorber");                         // name 
    G4LogicalVolume* logicAbsorber0 = new G4LogicalVolume(solidAbsorber0,                          // solid
                                                          iron,                                    // material
                                                          "hcal_absorber0"); 
    G4VPhysicalVolume* physiAbsorber;
    physiAbsorber = new G4PVPlacement(0,
                                      G4ThreeVector(0, 0, absorberPositionZ0),
                                      logicAbsorber0,
                                      "hcal_absorber0",
                                      logicWorld,
                                      false,
                                      -1,
                                      checkOverlap);
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        physiAbsorber = new G4PVPlacement(0,                                                       // no rotation
                                          G4ThreeVector(0, 0, (absorberPositionZ + i_Layer * absorberGapZ)),
                                          logicAbsorber,                                           // logical volume
                                          "hcal_absorber",                                         // name
                                          logicWorld,                                              // mother volume
                                          false,                                                   // no boolean operation
                                          -1,
                                          checkOverlap);                                           // copy number
    }

    // Active layer
    G4Box* solidCrystal = new G4Box("hcal_psd",                                                    // name
			                        0.5 * crystalX, 0.5 * crystalY, 0.5 * crystalZ);               // size

    G4LogicalVolume* logicCrystal = new G4LogicalVolume(solidCrystal,                              // solid
				                                        //ScinGlass4,                                // material
                                                        PSD,                                       // material
                                                        "hcal_psd");                               // name 

    G4VPhysicalVolume* physiCrystal;

    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        for (G4int i_X = 0; i_X < nCellX; ++i_X)
        {
            for (G4int i_Y = 0; i_Y < nCellY; ++i_Y)
            {
                physiCrystal = new G4PVPlacement(0,                                                // no rotation
                                                 G4ThreeVector(-0.5 * PCBX + (i_X + 0.5) * (crystalX + gapX),
                                                               -0.5 * PCBY + (i_Y + 0.5) * (crystalY + gapY),
                                                               crystalPositionZ + i_Layer * crystalGapZ),
                                                 logicCrystal,                                     // its logical volume
                                                 "hcal_psd",                                       // its name
                                                 logicWorld,                                       // its mother  volume
                                                 false,                                            // no boolean operation
                                                 i_Layer * 100000 + i_X * 100 + i_Y,
                                                 checkOverlap);                                    // copy number
            }
        }
    }

    G4Box* solidPCB = new G4Box("hcal_pcb",                                                        // its name
                                0.5 * PCBX, 0.5 * PCBY, 0.5 * PCBZ);                               // its size

    G4LogicalVolume* logicPCB = new G4LogicalVolume(solidPCB,                                      // its solid
				                                    kevlar,                                        // its material//should be check.............
                                                    "hcal_pcb");                                   // its name 

    G4VPhysicalVolume* physiPCB;
    for (G4int i_Layer = 0; i_Layer < nLayer; ++i_Layer)
    {
        physiPCB = new G4PVPlacement(0,                                                            // no rotation
                                     G4ThreeVector(0, 0, (PCBPositionZ + i_Layer * PCBGapZ)),
                                     logicPCB,                                                     // its logical volume
                                     "hcal_pcb",                                                   // its name
                                     logicWorld,                                                   // its mother  volume
                                     false,                                                        // no boolean operation
                                     -1,
                                     checkOverlap);                                                // copy number
    }
}
