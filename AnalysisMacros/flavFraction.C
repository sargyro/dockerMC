#include "LHEF.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iomanip>

// Function to calculate transverse momentum given px and py
float pt (const double px, const double py) {
    return std::sqrt(std::pow(px,2)+std::pow(py,2));
}

// Function to fill a histogram from a LHE file
void fillDistribution(TH1D &h, LHEF::Reader *reader) {
    long neve = 0;
    // Read each event
    while ( reader->readEvent() ) {
        ++neve;
        if ( reader->outsideBlock.length() ) std::cout << reader->outsideBlock;
        LHEF::HEPEUP hepeup = reader->hepeup;
        
        // Print event information
        //hepeup.print(std::cout);
        
        // Event weight
        double weight = hepeup.XWGTUP;
        
        // Retrireve particles in event
        std::vector<long>                 pid       = hepeup.IDUP;  // PDG ID of particle
        std::vector<int>                  status    = hepeup.ISTUP; // Particle status
        std::vector<std::vector<double> > momentum  = hepeup.PUP;   // Particle 4-momentum
        
        // Iterate over the particles
        // Fill histogram with pT of the outgoing (status = -1) particle that has the highest pT
        double pT = 0.;
        for (int i=0; i < pid.size(); ++i) {
            // Get the four-momentum of the particle
            std::vector<double> four_momentum = momentum.at(i);
            if (status.at(i) == 1) {
                pT = std::sqrt(std::pow(four_momentum.at(0),2)+std::pow(four_momentum.at(1),2));
                // We can exit the loop when the first particle with status 1 is found since particle 2 would have the same pT
                break;
            }
        }
        h.Fill(pT, weight);
    }
}

// Main function
void flavFraction() {
    
    // Do not print statistics on plot
    gStyle->SetOptStat(0);
    
    std::string fileName_qq = "unweighted_events_qq_qq.lhe";
    std::string fileName_qg = "unweighted_events_qg_qg.lhe";
    std::string fileName_gg = "unweighted_events_gg_gg.lhe";
    
    // Create a Reader for each input file
    LHEF::Reader reader_qq(fileName_qq);
    LHEF::Reader reader_qg(fileName_qg);
    LHEF::Reader reader_gg(fileName_gg);
            
    // Create a canvas to plot a histogram
    TCanvas canvas;
    
    // Create one histogram to hold transverse momentum distribution of each component
    TH1D h_pt_qq("h_pt_qq","Transverse momentum",100,0.,1000.); // 100 bins from 0 to 1000
    TH1D h_pt_qg("h_pt_qg","Transverse momentum",100,0.,1000.); // 100 bins from 0 to 1000
    TH1D h_pt_gg("h_pt_gg","Transverse momentum",100,0.,1000.); // 100 bins from 0 to 1000
    
    // Fill the histograms from the corresponding LHE files
    fillDistribution(h_pt_qq, &reader_qq);
    fillDistribution(h_pt_qg, &reader_qg);
    fillDistribution(h_pt_gg, &reader_gg);
        
    // Draw the histogram on the canvas
    canvas.cd();
    
    // Axis titles
    h_pt_qq.GetXaxis()->SetTitle("p_{T} [GeV]");
    h_pt_qq.GetYaxis()->SetTitle("Events");
    
    // Rebin histograms to reduce fluctiations (merge 2 bins)
    h_pt_qq.Rebin(2);
    h_pt_qg.Rebin(2);
    h_pt_gg.Rebin(2);

    // Normalise histograms to reproduce plot from lecture
    for (int i=1; i < h_pt_qq.GetNbinsX()+1; ++i) {
        // Total content in bin i
        double total_bin = h_pt_qq.GetBinContent(i)+h_pt_qg.GetBinContent(i)+h_pt_gg.GetBinContent(i);
        // Rescaling factor
        double factor = (total_bin != 0.) ? 1./total_bin : 1.;
        // Rescale each bin content so that the sum of the 3 histograms is equal to 1
        h_pt_qq.SetBinContent(i, h_pt_qq.GetBinContent(i)*factor);
        h_pt_qg.SetBinContent(i, h_pt_qg.GetBinContent(i)*factor);
        h_pt_gg.SetBinContent(i, h_pt_gg.GetBinContent(i)*factor);
    }
    
    // Color histograms
    h_pt_qq.SetFillColor(kSpring-9);
    h_pt_qg.SetFillColor(kYellow-9);
    h_pt_gg.SetFillColor(kCyan-9);
    
    // Plot histograms stacked
    h_pt_qq.Add(&h_pt_qg);
    h_pt_qq.Add(&h_pt_gg);
    h_pt_qq.Draw("hist");

    h_pt_qg.Add(&h_pt_gg);
    h_pt_qg.Draw("histsame");

    h_pt_gg.Draw("histsame");
    
    // Adjust y axis range
    h_pt_qq.GetYaxis()->SetRangeUser(0.,1.);
    h_pt_qq.GetXaxis()->SetRangeUser(20.,200.);
        
    // Set x-axis in log scale
    //canvas.SetLogx();
    canvas.RedrawAxis();
    
    // Save canvas in pdf file
    canvas.Print("pT_stacked.pdf");
        
    return 0;
}
