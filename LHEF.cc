#include "LHEF.h"
#include <iomanip>

int main(int argc, char ** argv) {
    
    if (argc < 2) {
        std::cerr << "./LHEF input.LHE" << std::endl;
        return 1;
    }
    
    // Create Reader and Writer object
    LHEF::Reader reader(argv[1]);
        
    long neve = 0;

    // Read each event and write them out again.
    while ( reader.readEvent() ) {
        ++neve;
        if ( reader.outsideBlock.length() ) std::cout << reader.outsideBlock;
        LHEF::HEPEUP hepeup = reader.hepeup;
        hepeup.print(std::cout);
        std::vector<long>                 particles = hepeup.IDUP;
        std::vector<std::vector<double> > momentum  = hepeup.PUP;
        for (int i=0; i < particles.size(); ++i) {
            std::vector<double> four_momentum = momentum.at(i);
            double E  = four_momentum.at(3);
            double px = four_momentum.at(0);
            double py = four_momentum.at(1);
            double pz = four_momentum.at(2);
            std::cout << "particle : " << particles.at(i) << " E = " << E << std::endl;
        }
    }
    
    return 0;
}
