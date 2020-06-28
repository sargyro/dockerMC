#include "Pythia8/Pythia.h"

// ROOT, for histogramming.
#include "TH1.h"
#include "TCanvas.h"

// ROOT, for interactive graphics.
#include "TVirtualPad.h"
#include "TApplication.h"

using namespace Pythia8;

int main() {
 
  // Declare histograms
  TH1F *h_jet_N = new TH1F("h_jet_N", "Jet multiplicity", 16, -0.5, 15.5);
  TH1F *h_jet1_pt = new TH1F("h_jet1_pt", "Leading jet transverse momentum", 50, 0., 100.);
  TH1F *h_jet2_pt = new TH1F("h_jet2_pt", "Subleading jet transverse momentum", 50, 0., 100.);
    
  // Settings
  int  nEvent = 3000;

  // Generator
  Pythia pythia;

  // Multi-jet production
  pythia.readString("HardQCD:all = on");
  
  // Turn off ISR/FSR/MPI
  bool partonOnly = true;
  bool includeIFSR = false;
  if (!includeIFSR) {
    pythia.readString("PartonLevel:MPI = off");
    pythia.readString("PartonLevel:ISR = off");
  }
  if (partonOnly) {
    pythia.readString("PartonLevel:FSR = off");
    pythia.readString("PartonLevel:Remnants = off");
    pythia.readString("Check:event = off");
    pythia.readString("HadronLevel:all = off");
  }
                     
  // Minimum pt cut for 2->2
  //pythia.readString("PhaseSpace:pTHatMin = 20.0");
      
  // Initialisation, pp @ 13 TeV
  pythia.readString("Beams:eCM = 13000.");
  pythia.init();

  // Select common parameters for SlowJet and FastJet analyses.
  int    power   = -1;     // -1 = anti-kT; 0 = C/A; 1 = kT.
  double R       = 0.4;    // Jet radius.
  double pTMin   = 0.0;    // Min jet pT in GeV.
  double etaMax  = 25.0;   // Pseudorapidity range of detector (number above 20 means full coverage)
  
  // Set up SlowJet jet finder in native mode.
  SlowJet slowJet( power, R, pTMin, etaMax);

  // Begin event loop. Generate event. Skip if error.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    // Begin SlowJet analysis of jet properties
    slowJet.analyze( pythia.event );
          
    // Fill histograms with jet properties
    h_jet_N->Fill(slowJet.sizeJet());
    if (slowJet.sizeJet() > 0) {
      h_jet1_pt->Fill(slowJet.pT(0));
      if (slowJet.sizeJet() > 1) {
        h_jet2_pt->Fill(slowJet.pT(1));
      }
    }
      
  // End of event loop.
  }

  // Statistics
  pythia.stat();
  
  // Print histograms in a Canvas and save them as pdf files
  TCanvas c;
  
  // Jet multiplicity
  h_jet_N->Draw("hist");
  h_jet_N->GetXaxis()->SetTitle("N jets");
  h_jet_N->GetYaxis()->SetTitle("Events");
  c.Print("Njets.pdf");

  // Leading jet transverse momentum
  h_jet1_pt->Draw("hist");
  h_jet1_pt->GetXaxis()->SetTitle("p_{T} leading jet [GeV]");
  h_jet1_pt->GetYaxis()->SetTitle("Events");
  c.Print("jet1pt.pdf");

  // Subleading jet transverse momentum
  h_jet2_pt->Draw("hist");
  h_jet2_pt->GetXaxis()->SetTitle("p_{T} subleading jet [GeV]");
  h_jet2_pt->GetYaxis()->SetTitle("Events");
  c.Print("jet2pt.pdf");

    // Done.
  return 0;
}
