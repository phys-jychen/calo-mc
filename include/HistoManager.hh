#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"
#include "TSystem.h"
#include "TGeoManager.h"
#include <vector>
#include "g4root.hh"
#include <G4ThreeVector.hh>
#include <unordered_map>

class TTree;
class TFile;
//const G4int kMAXTrack=5000;//should be checked!!!

class ParticleInfo
{
	public:
		G4int fPrimaryPDG;
		G4double fPrimaryEnergy;
		std::vector<G4int> fecal_pdgid;
		std::vector<G4int> fecal_trackid;
		std::vector<G4double> fecal_x;
		std::vector<G4double> fecal_y;
		std::vector<G4double> fecal_z;
		std::vector<G4double> fecal_time;
		std::vector<G4int> fecal_psdid;
		std::vector<G4double> fecal_energy;
		std::vector<G4int> fecal_cellid;
		std::vector<G4double> fecal_celle;
		std::vector<G4double> fecal_cellx;
		std::vector<G4double> fecal_celly;
		std::vector<G4double> fecal_cellz;
		std::unordered_map<G4int,G4double> fecal_mape;
		std::vector<G4int> fhcal_pdgid;
		std::vector<G4int> fhcal_trackid;
		std::vector<G4double> fhcal_x;
		std::vector<G4double> fhcal_y;
		std::vector<G4double> fhcal_z;
		std::vector<G4double> fhcal_time;
		std::vector<G4int> fhcal_psdid;
		std::vector<G4double> fhcal_energy;
		std::vector<G4int> fhcal_cellid;
		std::vector<G4double> fhcal_celle;
		std::vector<G4double> fhcal_cellx;
		std::vector<G4double> fhcal_celly;
		std::vector<G4double> fhcal_cellz;
		std::unordered_map<G4int,G4double> fhcal_mape;
		void reset(){
			std::vector<G4int>().swap( fecal_pdgid);
			std::vector<G4int>().swap( fecal_trackid);
			std::vector<G4double>().swap( fecal_x);
			std::vector<G4double>().swap( fecal_y);
			std::vector<G4double>().swap( fecal_z);
			std::vector<G4double>().swap( fecal_time);
			std::vector<G4int>().swap( fecal_psdid);
			std::vector<G4double>().swap( fecal_energy);
			std::vector<G4int>().swap( fecal_cellid);
			std::vector<G4double>().swap( fecal_celle);
			std::vector<G4double>().swap( fecal_cellx);
			std::vector<G4double>().swap( fecal_celly);
			std::vector<G4double>().swap( fecal_cellz);
			std::vector<G4int>().swap( fhcal_pdgid);
			std::vector<G4int>().swap( fhcal_trackid);
			std::vector<G4double>().swap( fhcal_x);
			std::vector<G4double>().swap( fhcal_y);
			std::vector<G4double>().swap( fhcal_z);
			std::vector<G4double>().swap( fhcal_time);
			std::vector<G4int>().swap( fhcal_psdid);
			std::vector<G4double>().swap( fhcal_energy);
			std::vector<G4int>().swap( fhcal_cellid);
			std::vector<G4double>().swap( fhcal_celle);
			std::vector<G4double>().swap( fhcal_cellx);
			std::vector<G4double>().swap( fhcal_celly);
			std::vector<G4double>().swap( fhcal_cellz);
			fecal_mape.clear();
			fhcal_mape.clear();
		};

		ParticleInfo() {
			std::vector<G4int>().swap( fecal_pdgid);
			std::vector<G4int>().swap( fecal_trackid);
			std::vector<G4double>().swap( fecal_x);
			std::vector<G4double>().swap( fecal_y);
			std::vector<G4double>().swap( fecal_z);
			std::vector<G4double>().swap( fecal_time);
			std::vector<G4int>().swap( fecal_psdid);
			std::vector<G4double>().swap( fecal_energy);
			std::vector<G4int>().swap( fecal_cellid);
			std::vector<G4double>().swap( fecal_celle);
			std::vector<G4double>().swap( fecal_cellx);
			std::vector<G4double>().swap( fecal_celly);
			std::vector<G4double>().swap( fecal_cellz);
			std::vector<G4int>().swap( fhcal_pdgid);
			std::vector<G4int>().swap( fhcal_trackid);
			std::vector<G4double>().swap( fhcal_x);
			std::vector<G4double>().swap( fhcal_y);
			std::vector<G4double>().swap( fhcal_z);
			std::vector<G4double>().swap( fhcal_time);
			std::vector<G4int>().swap( fhcal_psdid);
			std::vector<G4double>().swap( fhcal_energy);
			std::vector<G4int>().swap( fhcal_cellid);
			std::vector<G4double>().swap( fhcal_celle);
			std::vector<G4double>().swap( fhcal_cellx);
			std::vector<G4double>().swap( fhcal_celly);
			std::vector<G4double>().swap( fhcal_cellz);
			fecal_mape.clear();
			fhcal_mape.clear();
		}
};

class HistoManager
{
	public:
		HistoManager(const char* foutname,const bool &savegeo);
		~HistoManager();
		void save();
		void book();
		ParticleInfo fParticleInfo;
	private:
		G4bool    fSaveGeo;
		G4String fOutName;
	public:  
		TFile* fRootFile;
		TTree* fNtuple;
};

#endif
