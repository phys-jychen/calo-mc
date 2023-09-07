#include "G4Event.hh"
#include <iomanip>
#include <HistoManager.hh>
#include <TTree.h>
#include "RunAction.hh"

#include "EventAction.hh"
//#include "EventMessenger.hh"

EventAction::EventAction(HistoManager* histo, Config* c)
 : G4UserEventAction(),
   fEventEdep(0), fPrintModulo(10000), fDecayChain(), fHistoManager_Event(histo), config(c)
{
    fGParticleSource = new G4GeneralParticleSource();
    //eventmanager->SetVerboseLevel(config->conf["Verbose"]["event"].as<int>());
    //fHistoManager_Event = new HistoManager();
    //fEventMessenger = new EventMessenger(this);
}

EventAction::~EventAction()
{
    delete fGParticleSource;
    //delete fHistoManager_Event;
    //delete fEventMessenger;
}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fStepTag = 0;
//    G4cout << "..................6666666666666........................" << G4endl;
    fDecayChain = " ";
//    fHistoManager_Event->fParticleInfo.reset();
//    G4cout << "Begin of event" << G4endl;
}

void EventAction::EndOfEventAction(const G4Event* evt)
{
//    G4cout << " >>>>>>>>>>>>>>>>> " << fHistoManager_Event->fParticleInfo.fPrimaryEnergy << " <<<<<<<<<<<<<<<" << G4endl;
//    G4cout << "..................777777777777777........................" << G4endl;
    G4int evtNb = evt->GetEventID();

    G4int nLayer = config->conf["HCAL"]["nLayer"].as<G4int>();
    G4int nCellX = config->conf["HCAL"]["nCellX"].as<G4int>();
    G4int nCellY = config->conf["HCAL"]["nCellY"].as<G4int>();
    G4double CellWidthX = config->conf["HCAL"]["CellWidthX"].as<G4double>();
    G4double CellWidthY = config->conf["HCAL"]["CellWidthY"].as<G4double>();
    G4double gapX = 0.3;
    G4double gapY = 0.3;

    // Printing survey
    if (evtNb % 1000 == 0) 
        G4cout << G4endl << "End of event " << std::setw(6) << evtNb << " :" + fDecayChain << G4endl;
    for (auto i : fHistoManager_Event->fParticleInfo.fecal_mape)
    {
        if (i.second < 0.1)
            continue;
        fHistoManager_Event->fParticleInfo.fecal_cellid.emplace_back(i.first);
        fHistoManager_Event->fParticleInfo.fecal_celle.emplace_back(SiPMDigi(i.second));
        G4int layer = i.first / 210;
        G4int m = (i.first % 210) / 42;
        G4int n = (i.first % 210) % 42;
        G4double x = 0.0;
        G4double y = 0.0;
        G4double z = 0.0;
        if (layer % 2 == 0)
        {
            x = -41.0 * 5.0 / 2.0 + n * 5.0;
            y = -4.0 * 45.0 / 2.0 + m * 45.0;
        }
        if (layer % 2 == 1)
        {
            x = -4.0 * 45.0 / 2.0 + m * 45.0;
            y = -41.0 * 5.0 / 2.0 + n * 5.0;
        }
        z = 1.0 + layer * (2.0 + 2.0 + 2.8);
        fHistoManager_Event->fParticleInfo.fecal_cellx.emplace_back(x);
        fHistoManager_Event->fParticleInfo.fecal_celly.emplace_back(y);
        fHistoManager_Event->fParticleInfo.fecal_cellz.emplace_back(z);
    }

    for (auto i : fHistoManager_Event->fParticleInfo.fhcal_mape)
    {
        if (i.second < 0.1)
        	continue;
        fHistoManager_Event->fParticleInfo.fhcal_cellid.emplace_back(i.first);
        fHistoManager_Event->fParticleInfo.fhcal_celle.emplace_back(SiPMDigi(i.second));
        G4double x = (i.first % 100000) / 100;
        G4double y = (i.first % 100);
        G4double layer = i.first / 100000;
        fHistoManager_Event->fParticleInfo.fhcal_cellx.emplace_back((x + 0.5 - 0.5 * nCellX) * (CellWidthX + gapX));
        fHistoManager_Event->fParticleInfo.fhcal_celly.emplace_back((y + 0.5 - 0.5 * nCellY) * (CellWidthY + gapY));
        fHistoManager_Event->fParticleInfo.fhcal_cellz.emplace_back(30.0 * layer);
        /*
        fHistoManager_Event->fParticleInfo.fhcal_cellx.emplace_back(-360.0 + (x + 0.5) * 40.0);
        fHistoManager_Event->fParticleInfo.fhcal_celly.emplace_back(-360.0 + (y + 0.5) * 40.0);
        fHistoManager_Event->fParticleInfo.fhcal_cellz.emplace_back(1.5 + layer * 25.0);
        */
    }
//    G4cout << "End of event " << fHistoManager_Event->fParticleInfo.nTrack << " " << fHistoManager_Event->fParticleInfo.fTrackTime[0] << G4endl;
 
    fHistoManager_Event->fNtuple->Fill();
}

void EventAction::AddEcalHit(const G4int& copyNo, const G4double& edep, const G4double& time, const G4int& pdgid, const G4int& trackid)
{
    //fHistoManager_Event->fParticleInfo.fecal_psdid.emplace_back(copyNo);
    //fHistoManager_Event->fParticleInfo.fecal_energy.emplace_back(edep);
    //fHistoManager_Event->fParticleInfo.fecal_pdgid.emplace_back(pdgid);
    //fHistoManager_Event->fParticleInfo.fecal_trackid.emplace_back(trackid);
    //fHistoManager_Event->fParticleInfo.fecal_time.emplace_back(time);

    G4int layer=copyNo / 210;
    //G4cout << copyNo << " " << layer << G4endl;
    G4int m = (copyNo % 210) / 42;
    G4int n = (copyNo % 210) % 42;
    G4double x = 0.0;
    G4double y = 0.0;
    G4double z = 0.0;
    if (layer % 2 == 0)
    {
        x = -41.0 * 5.0 / 2.0 + n * 5.0;
        y = -4.0 * 45.0 / 2.0 + m * 45.0;
    }
    if (layer % 2 == 1)
    {
        x = -4.0 * 45.0 / 2.0 + m * 45.0;
        y = -41.0 * 5.0 / 2.0 + n * 5.0;
    }
    z = 1.0 + layer * (2.0 + 2.0 + 2.8);
    //fHistoManager_Event->fParticleInfo.fecal_x.emplace_back(x);
    //fHistoManager_Event->fParticleInfo.fecal_y.emplace_back(y);
    //fHistoManager_Event->fParticleInfo.fecal_z.emplace_back(z);
    fHistoManager_Event->fParticleInfo.fecal_mape[copyNo] += edep;
}

void EventAction::AddHcalHit(const G4int& copyNo, const G4double& edep, const G4double& time, const G4int& pdgid, const G4int& trackid)
{
    //fHistoManager_Event->fParticleInfo.fhcal_psdid.emplace_back(copyNo);
    //fHistoManager_Event->fParticleInfo.fhcal_energy.emplace_back(edep);
    //fHistoManager_Event->fParticleInfo.fhcal_pdgid.emplace_back(pdgid);
    //fHistoManager_Event->fParticleInfo.fhcal_trackid.emplace_back(trackid);
    //fHistoManager_Event->fParticleInfo.fhcal_time.emplace_back(time);
    G4double x = (copyNo % 10000) / 100;
    G4double y = (copyNo % 100);
    G4double layer = copyNo / 10000;
    //fHistoManager_Event->fParticleInfo.fhcal_x.push_back(-360.0 + (x + 0.5) * 40.0);
    //fHistoManager_Event->fParticleInfo.fhcal_y.push_back(-360.0 + (y + 0.5) * 40.0);
    //fHistoManager_Event->fParticleInfo.fhcal_z.push_back(1.5 + layer * 25.0);
    fHistoManager_Event->fParticleInfo.fhcal_mape[copyNo] += edep;
}

Double_t EventAction::SiPMDigi(const Double_t& edep) const
{
    Int_t sPix = 0;
    sPix = gRandom->Poisson(edep / 0.466 * 20);
    sPix = 7396.0 * (1 - TMath::Exp(-sPix / 7284.0));
    Double_t sChargeOutMean = sPix * 29.4;
    Double_t sChargeOutSigma = sqrt(sPix * 5 * 5 + 3 * 3);
    Double_t sChargeOut = -1;
    while (sChargeOut < 0)
        sChargeOut = gRandom->Gaus(sChargeOutMean, sChargeOutSigma);
    Double_t sAdc = -1;
    while (sAdc < 0)
        sAdc = gRandom->Gaus(sChargeOut, 0.0002 * sChargeOut);
    Double_t sMIP = sAdc / 29.4 / 20;
    if (sMIP < 0.5)
        return 0;
    return sMIP * 0.466;
}