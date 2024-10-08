#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include <TTree.h>
#include <TFile.h>

HistoManager::HistoManager(const char* foutname, const G4bool& savegeo)
  : fRootFile(0), fNtuple(0), fSaveGeo(savegeo)
{
    fOutName = foutname;
}

HistoManager::~HistoManager()
{
    delete G4AnalysisManager::Instance();
}

void HistoManager::book()
{
    G4cout << "----------> Creating ROOT file < ----------" << G4endl << G4endl;
    fRootFile = new TFile(fOutName.c_str(), "RECREATE");
    fNtuple = new TTree("Calib_Hit", "MC events");
//    fNtuple = new TTree("Calib_Hit", "MC events of " + G4BestUnit(config->conf["Source"]["energy"].as<G4double>(), "Energy") + " " + config->conf["Source"]["particle"].as<G4String>());

    /*
    // ECAL
    fNtuple->Branch("ecal_psdid",          &fParticleInfo.fecal_psdid);
    fNtuple->Branch("ecal_energy",         &fParticleInfo.fecal_energy);
    fNtuple->Branch("ecal_pdgid",          &fParticleInfo.fecal_pdgid);
    fNtuple->Branch("ecal_trackid",        &fParticleInfo.fecal_trackid);
    fNtuple->Branch("ecal_x",              &fParticleInfo.fecal_x);
    fNtuple->Branch("ecal_y",              &fParticleInfo.fecal_y);
    fNtuple->Branch("ecal_z",              &fParticleInfo.fecal_z);
    fNtuple->Branch("ecal_time",           &fParticleInfo.fecal_time);
    fNtuple->Branch("ecal_cellid",         &fParticleInfo.fecal_cellid);
    fNtuple->Branch("ecal_celle",          &fParticleInfo.fecal_celle);
    fNtuple->Branch("ecal_cellx",          &fParticleInfo.fecal_cellx);
    fNtuple->Branch("ecal_celly",          &fParticleInfo.fecal_celly);
    fNtuple->Branch("ecal_cellz",          &fParticleInfo.fecal_cellz);

    // HCAL
    fNtuple->Branch("hcal_psdid",          &fParticleInfo.fhcal_psdid);
    fNtuple->Branch("hcal_energy",         &fParticleInfo.fhcal_energy);
    fNtuple->Branch("hcal_pdgid",          &fParticleInfo.fhcal_pdgid);
    fNtuple->Branch("hcal_trackid",        &fParticleInfo.fhcal_trackid);
    fNtuple->Branch("hcal_x",              &fParticleInfo.fhcal_x);
    fNtuple->Branch("hcal_y",              &fParticleInfo.fhcal_y);
    fNtuple->Branch("hcal_z",              &fParticleInfo.fhcal_z);
    fNtuple->Branch("hcal_time",           &fParticleInfo.fhcal_time);
    fNtuple->Branch("hcal_cellid",         &fParticleInfo.fhcal_cellid);
    fNtuple->Branch("hcal_celle",          &fParticleInfo.fhcal_celle);
    fNtuple->Branch("hcal_cellx",          &fParticleInfo.fhcal_cellx);
    fNtuple->Branch("hcal_celly",          &fParticleInfo.fhcal_celly);
    fNtuple->Branch("hcal_cellz",          &fParticleInfo.fhcal_cellz);
    */
    fNtuple->Branch("CellID",              &fParticleInfo.fhcal_cellid);
//    fNtuple->Branch("Hit_Energy_nodigi",   &fParticleInfo.fhcal_celle_nodigi);
    fNtuple->Branch("Hit_Energy",          &fParticleInfo.fhcal_celle);
    fNtuple->Branch("Hit_X",               &fParticleInfo.fhcal_cellx);
    fNtuple->Branch("Hit_Y",               &fParticleInfo.fhcal_celly);
    fNtuple->Branch("Hit_Z",               &fParticleInfo.fhcal_cellz);
//    fNtuple->Branch("Energy",              &fParticleInfo.fhcal_energy);
//    fNtuple->Branch("X",                   &fParticleInfo.fhcal_x);
//    fNtuple->Branch("Y",                   &fParticleInfo.fhcal_y);
//    fNtuple->Branch("Z",                   &fParticleInfo.fhcal_z);
//    fNtuple->Branch("Time",                &fParticleInfo.fhcal_time);
}

void HistoManager::save()
{
    if (fSaveGeo)
    {
        gSystem->Load("libGeom");
        TGeoManager::Import("cepc-calo.gdml");
        gGeoManager->Write("cepc_calo");
        std::remove("cepc-calo.gdml");
    }

    fNtuple->Write("", TObject::kOverwrite);
    fRootFile->Close();
    G4cout << "----------> Closing ROOT file <----------" << G4endl << G4endl;
}
