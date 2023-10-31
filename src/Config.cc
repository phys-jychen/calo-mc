#include "Config.hh"
using namespace std;

Config::Config() {}

Config::~Config() {}

void Config::Parse(const string& config_file)
{
    UI = G4UImanager::GetUIpointer();
    conf = YAML::LoadFile(config_file);

    if (conf["Project"].IsDefined())
        G4cout << "Configuration file loaded successfully" << G4endl;
    else
        throw config_file;
}

G4bool Config::IsLoad()
{
    return conf["Project"].IsDefined();
}

G4int Config::Run()
{
    // Choose the Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    if (conf["Global"]["useseed"].as<G4bool>())
        CLHEP::HepRandom::setTheSeed(conf["Global"]["seed"].as<G4long>());
    else
        CLHEP::HepRandom::setTheSeed(this->GetTimeNs());
    CLHEP::HepRandom::showEngineStatus();
    G4cout << "seed: " << CLHEP::HepRandom::getTheSeed() << G4endl;

    // Construct the default run manager
    // Verbose output class
    G4VSteppingVerbose::SetInstance(new SteppingVerbose);
    G4RunManager* runManager = new G4RunManager;

    // Set mandatory initialisation classes
    DetectorConstruction* detector = new DetectorConstruction(this);
    if (conf["Global"]["savegeo"].as<G4bool>())
    {
    	G4GDMLParser parser;
    	parser.Write("cepc-calo.gdml",detector->Construct());
    }
    runManager->SetUserInitialization(detector);

    G4VUserPhysicsList* physics = new QGSP_BERT();
    runManager->SetUserInitialization(physics);

    HistoManager* histo = new HistoManager(conf["Global"]["output"].as<string>().c_str(), conf["Global"]["savegeo"].as<G4bool>());
//    SteppingVerbose* stepV = new SteppingVerbose();

    PrimaryGeneratorAction* primary = new PrimaryGeneratorAction(detector, histo, this);
    runManager->SetUserAction(primary);

    RunAction* runAction = new RunAction(primary, histo, this);
    runManager->SetUserAction(runAction);

    EventAction* eventAction = new EventAction(histo, this);
    runManager->SetUserAction(eventAction);

    TrackingAction* trackingAction = new TrackingAction(runAction, eventAction, this);
    runManager->SetUserAction(trackingAction);

    SteppingAction* steppingAction = new SteppingAction(detector, eventAction);
    runManager->SetUserAction(steppingAction);

    runManager->SetVerboseLevel(conf["Verbose"]["run"].as<G4int>());
    G4String command = "/control/execute ";

    UI->ApplyCommand(G4String("/control/verbose ") + G4String(conf["Verbose"]["control"].as<string>()));
    UI->ApplyCommand(G4String("/tracking/verbose ") + G4String(conf["Verbose"]["tracking"].as<string>()));
    UI->ApplyCommand(G4String("/event/verbose ") + G4String(conf["Verbose"]["event"].as<string>()));

    for (auto subconf : conf["Source"])
        UI->ApplyCommand("/gps/" + subconf.first.as<string>() + " " + subconf.second.as<string>());

    // Initialise G4 kernel
    runManager->Initialize();
    runManager->BeamOn(conf["Global"]["beamon"].as<G4int>());

    // Job termination
    delete runManager;
    if (access("cepc-calo.gdml", F_OK) == 0)
        remove("cepc-calo.gdml");

    return 1;
}

void Config::Print()
{
    ofstream fout("./default.yaml");

    fout << "# Project information" << endl;
    fout << "Project: CALO-MC" << endl;
    fout << "Contact: Ji-Yuan CHEN (SJTU; < jy_chen@sjtu.edu.cn >)" << endl;
    fout << endl << endl;
    fout << "Global:" << endl;
    fout << "    useseed: true    # True: Use user-specified seed; False: Use system time in ns" << endl;
    fout << "    seed: 2022" << endl;
    fout << endl;
    fout << "    usemac: false    # Currently not applicable" << endl;
    fout << "    mac: ./vis.mac" << endl;
    fout << endl;
    fout << "    output: ./test.root    # Output ROOT file name" << endl;
    fout << "    beamon: 100" << endl;
    fout << "    savegeo: false" << endl;
    fout << endl << endl;
    fout << "# Calorimeter construction" << endl;
    fout << "Geometry:" << endl;
    fout << "    build_ECAL: false" << endl;
    fout << "    build_HCAL: true" << endl;
    fout << endl << endl;
    fout << "# Structure of HCAL" << endl;
    fout << "HCAL:" << endl;
    fout << "    nCellX: 18" << endl;
    fout << "    nCellY: 18" << endl;
    fout << "    nLayer: 40" << endl;
    fout << endl;
    fout << "    CellWidthX: 40    # In mm" << endl;
    fout << "    CellWidthY: 40    # In mm" << endl;
    fout << endl;
    fout << "    GapX: 0.3    # In mm" << endl;
    fout << "    GapY: 0.3    # In mm" << endl;
    fout <<  endl << endl;
    fout << "# Particle source set-up" << endl;
    fout << "Source:" << endl;
    fout << "    particle: \"mu-\"" << endl;
    fout << endl;
    fout << "    pos/type: \"Beam\"" << endl;
    fout << "    pos/shape: \"Circle\"" << endl;
    fout << "    pos/centre: \"0 0 -1 cm\"" << endl;
    fout << "    pos/radius: \"0 mm\"" << endl;
    fout << "    pos/sigma_r: \"7 mm\"" << endl;
    fout << endl;
    fout << "    ang/type: \"beam1d\"" << endl;
    fout << "    ang/rot1: \"0 1 0\"" << endl;
    fout << "    ang/rot2: \"1 0 0\"" << endl;
    fout << endl;
    fout << "    ene/type: \"Mono\"" << endl;
    fout << "    ene/mono: \"100 GeV\"" << endl;
    /*
    fout << "    particle: mu+" << endl;
    fout << "    ene_type: Mono" << endl;
    fout << "    energy: 100.0    # In GeV" << endl;
    fout << "    pos_type: Point" << endl;
    fout << "    position: [ 0.0, 0.0, -1.0 ]    # In cm" << endl;
    fout << "    ang_type: planar" << endl;
    fout << "    direction: [ 0.0, 0.0, 1.0 ]" << endl;
    */
    fout << endl << endl;
    fout << "# Verbose" << endl;
    fout << "Verbose:" << endl;
    fout << "    run: 0" << endl;
    fout << "    control: 0" << endl;
    fout << "    event: 0" << endl;
    fout << "    tracking: 0" << endl;

    fout.close();
}