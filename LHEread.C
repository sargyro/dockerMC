#include "LHEF.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iomanip>

// Function to calculate transverse momentum given an input 4-momentum
float pt (const std::vector<double> & four_momentum) {
    double px = four_momentum.at(0);
    double py = four_momentum.at(1);
    return std::sqrt(std::pow(px,2)+std::pow(py,2));
}

// Function to calculate invariant mass given an input 4-momentum
float mass (const std::vector<double> & four_momentum) {
    double px = four_momentum.at(0);
    double py = four_momentum.at(1);
    double pz = four_momentum.at(2);
    double E  = four_momentum.at(3);
    double m2 = std::pow(E,2)-std::pow(px,2)-std::pow(py,2)-std::pow(pz,2);
    return (m2 > 0) ? std::sqrt(m2) : 0;
}

void LHEread() {
    
    std::string fileName = "unweighted_events.lhe";
    
    // Create Reader and Writer object
    LHEF::Reader reader(fileName);
        
    long neve = 0;
    
    // Create a canvas to plot a histogram
    TCanvas canvas;
    
    // Create a histogram to hold a given distribution
    TH1D h_E("h_E","Energy",100,0.,1000.); // 100 bins from 0 to 1000
    
    // Read each event and write them out again.
    while ( reader.readEvent() ) {
        ++neve;
        if ( reader.outsideBlock.length() ) std::cout << reader.outsideBlock;
        LHEF::HEPEUP hepeup = reader.hepeup;
        
        // Print event information
        hepeup.print(std::cout);
        
        // Event weight
        double weight = hepeup.XWGTUP;
        
        // Retrireve particles in event
        std::vector<long>                 pid       = hepeup.IDUP;  // PDG ID of particle
        std::vector<int>                  status    = hepeup.ISTUP; // Particle status
        std::vector<std::vector<double> > momentum  = hepeup.PUP;   // Particle 4-momentum
        
        for (int i=0; i < pid.size(); ++i) {
            // Get the four-momentum of the particle
            std::vector<double> four_momentum = momentum.at(i);
            
            // Print some information about the particle
            //std::cout << "id : " << pid.at(i) << " status = " << status.at(i) << " E = " << four_momentum.at(3) << std::endl << " m = " << mass(four_momentum) << std::endl;
            
            // Fill histogram with pT of outgoing particles (status = 1)
            if (status.at(i) == 1) {
                h_E.Fill(four_momentum.at(3), weight);
            }
        }
        
    } // Event loop
    
    // Draw the histogram on the canvas
    canvas.cd();
    h_E.GetXaxis()->SetTitle("E [GeV]");
    h_E.GetYaxis()->SetTitle("Events");
    h_E.Draw("hist");
    
    // Save canvas in pdf file
    canvas.Print("E.pdf");
    
    return 0;
}
