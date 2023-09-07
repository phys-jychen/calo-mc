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

DetectorConstruction::DetectorConstruction(Config* c)
 : G4VUserDetectorConstruction(),
   config(c)
{}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    visAttributes = new G4VisAttributes(G4Colour(0.9, 0.0, 0.0));
    visAttributes -> SetVisibility(false);

    physiWorld = ConstructWorld();
    if (config->conf["Geometry"]["build_ECAL"].as<G4bool>())
    	ConstructECAL();
    if (config->conf["Geometry"]["build_HCAL"].as<G4bool>())
    	ConstructHCAL();

    //logicAbsorber ->SetVisAttributes(visAttributes);
    //(0.9, 0.9, 0.0)yellow

    //visAttributes = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));//green
    //logicPCB ->SetVisAttributes(visAttributes);

    //visAttributes = new G4VisAttributes(G4Colour(0.9, 0.9, 0.9));//white
    //logicCrystal ->SetVisAttributes(visAttributes);

    //SteppingAction* steppingAction = SteppingAction::Instance();

    return physiWorld;
}

G4VPhysicalVolume* DetectorConstruction::ConstructWorld()
{
    G4Material* Vacuum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    G4bool checkOverlaps = false;

    // Full sphere shape
    G4double solidWorld_rmax = 200 * cm;
    G4Orb* solidWorld = new G4Orb("World",                                    // name
                                  solidWorld_rmax);                           // size 

//    G4LogicalVolume*
    logicWorld = new G4LogicalVolume(solidWorld,                              // solid
                                     Vacuum,                                  // material
                                     "World");                                // name
    G4VPhysicalVolume* physiWorld = new G4PVPlacement(0,                      // no rotation
                                                      G4ThreeVector(),        // at (0,0,0)
                                                      logicWorld,             // logical volume
                                                      "World",                // name
                                                      0,                      // mother  volume
                                                      false,                  // no boolean operation
                                                      0,
                                                      checkOverlaps);         //copy number
    logicWorld ->SetVisAttributes(visAttributes);

    return physiWorld;
}
