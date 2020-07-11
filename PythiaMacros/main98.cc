#include "Pythia8/Pythia.h"

// ROOT, for histogramming.
#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLorentzVector.h"

// ROOT, for interactive graphics.
#include "TVirtualPad.h"
#include "TApplication.h"

using namespace Pythia8;

class MyUserHooks : public UserHooks {

public:

  // Constructor can set helicity definition. Destructor does nothing.
  MyUserHooks(Pythia8::Info* infoPtrIn) : infoPtr(infoPtrIn) { }
  ~MyUserHooks() { }

  // Allow a veto for the process level, to gain access to decays.
  bool canVetoProcessLevel() {return true;}

  // Access the event after resonance decays.
  bool doVetoProcessLevel(Event& process) {
    for (int i = 0; i < process.size(); ++i) {
      if (process[i].idAbs() == 22) {
        if (process[i].pT() < 20. || fabs(process[i].eta()) > 2.5) return true;
      }
    }
    return false;
  }

private:
  Pythia8::Info* infoPtr;
};


int main() {
 
  // Declare histograms
  TH1F *h_diphoton_m = new TH1F("h_diphoton_m", "Diphoton mass", 300, 10., 310.);
    
  // Settings - 10k events will take about 30 min to generate
  int  nEvent = 10000;

  // Generator
  Pythia pythia;
  Event& event = pythia.event;

  // Hook to kill events with low pT photons
  MyUserHooks* myUserHooks = new MyUserHooks(&pythia.info);
  pythia.setUserHooksPtr( myUserHooks);

  // Prompt diphoton production
  pythia.readString("PromptPhoton:ffbar2gammagamma = on");
  pythia.readString("PromptPhoton:gg2gammagamma = on");
                     
  // Initialisation, pp @ 13 TeV
  pythia.readString("Beams:eCM = 13000.");
  pythia.init();

  // Begin event loop. Generate event. Skip if error.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;
            
    // Declare a vector to hold the 4-momentum particles we are interested in
    vector<TLorentzVector> photons;
      
    // Loop over final state particles
    for (int i = 0; i < event.size(); ++i){
      // Particle short notation
      Particle& p = event[i];
        
      // Apply cuts
      if (p.isFinal() && abs(p. id())==22 && fabs(p.eta()) < 2.5 && p.pT() > 10) {
        TLorentzVector v;
        v.SetPxPyPzE(p.px(), p.py(), p.pz(), p.e());
        photons.push_back(v);
      }
    }
    
    // If there are less than two photons skip the event
    if (photons.size() < 2) continue;
      
    // Find the two highest pT photons and calculate their mass
    unsigned int index_lead = -1;
    unsigned int index_sublead = -1;
    for (unsigned int i=0; i < photons.size(); ++i) {
      if (index_lead == -1) { index_lead=i; continue; }
      if ( photons.at(i).Pt() > photons.at(index_lead).Pt()) {
        index_sublead=index_lead;
        index_lead=i;
        continue;
      }
      if (index_sublead == -1) { index_sublead=i; continue; }
    }
    
    // Calculate the diphoton mass
    float mass = (photons.at(index_lead)+photons.at(index_sublead)).M();
    
    // Fill histogram
    h_diphoton_m->Fill(mass, pythia.info.weight());
      
  // End of event loop.
  }

  // Statistics
  pythia.stat();
  
  // Print histograms in a Canvas and save them as pdf files
  TCanvas c;
  
  // Jet multiplicity
  h_diphoton_m->Draw("hist");
  h_diphoton_m->GetXaxis()->SetTitle("m_{#gamma#gamma} [GeV]");
  h_diphoton_m->GetYaxis()->SetTitle("Events");
  c.Print("diphoton_mass.pdf");
  
  // Save the histogram in a root file
  TFile *f = TFile::Open("diphoton.root","RECREATE");
  h_diphoton_m->Write();
  f->Close();
    
  // Done.
  return 0;
}
