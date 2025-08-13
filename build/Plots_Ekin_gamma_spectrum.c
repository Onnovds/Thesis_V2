#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TROOT.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstring> 

// --- HELPER FUNCTIONS
std::string replaceDotWithUnderscore(const std::string& input) {
    std::string output = input;
    std::replace(output.begin(), output.end(), '.', '_');
    return output;
}

std::string formatFloat(float value) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    std::string result = stream.str();
    if (result.find('.') != std::string::npos) {
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
        if (result.back() == '.') {
            result.pop_back();
        }
    }
    return result;
}

// --- DATA STRUCTURES
struct RunData {
    std::string title;
    std::string filename;
    float generatedParticles;
    float generatorEnergy;
    TFile* file;
    TTree* tree;
};

// --- DATA LOADING 
std::vector<RunData> loadRuns(const std::vector<float>& energies) {
    std::vector<RunData> runs;
    for (const auto& energy : energies) {
        RunData run;
        std::string energyStr = replaceDotWithUnderscore(formatFloat(energy));
        run.generatorEnergy = energy;
        run.title = "100kProton" + energyStr + "MeV_Al_Slab_5mm_Vacuum_fStopandKill_Omnidirectional";
        run.filename = "/home/onno/satellite_test/build/Protons/" + run.title + ".root";
        run.file = TFile::Open(run.filename.c_str(), "READ");
        if (run.file && !run.file->IsZombie()) {
            run.tree = (TTree*)run.file->Get("DetectorData");
        } else {
            std::cerr << "Failed to open file: " << run.filename << std::endl;
        }
        runs.push_back(run);
    }
    return runs;
}

int numBins = 100;

// Create TProfiles for all particles from a list of runs 
std::map<double, TProfile*> createCombinedProfiles(const std::vector<RunData>& runs) {
    std::map<double, TProfile*> profiles;
    for (const auto& run : runs) {
        if (!run.tree) continue;

        Double_t Ekin, Edep;
        run.tree->SetBranchAddress("KineticEnergy", &Ekin);
        run.tree->SetBranchAddress("DepositedEnergy", &Edep);

        // Create a uniqu 2D histogram for each run
        TH2D* h2d = new TH2D(Form("h2d_all_%.1fMeV", run.generatorEnergy), "Ekin vs Edep", numBins, 0, 200, 100, 0, 2000);

        for (Long64_t j = 0; j < run.tree->GetEntries(); ++j) {
            run.tree->GetEntry(j);
            if (Ekin>0 && Edep>0){
            h2d->Fill(Ekin, Edep*1e3);}
        }

        // Create a uniquely named TProfile from the 2D hist
        TProfile* profile = h2d->ProfileX(Form("prof_all_%.1fMeV", run.generatorEnergy));
        profiles[run.generatorEnergy] = profile;
        h2d->Delete(); // We can delete the 2D hist as the profile now holds the info
    }
    return profiles;
}



// --- *** NEW FUNCTION TO PLOT GAMMA KINETIC ENERGY SPECTRUM *** ---
void plotGammaKineticEnergySpectrum(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Show statistics box

        // --- Binning Setup for Logarithmic Scale ---
    // We want to see details from the keV range up to the MeV range.
    const int nBins = 100;      // Number of bins. You can adjust this.
    const double minE = 0.001;  // Minimum energy in MeV (1 keV). Log scale cannot start at 0.
    const double maxE = 30.0;   // Maximum energy in MeV.

    // Create an array to hold the bin edges.
    // There are nBins + 1 edges for nBins.
    std::vector<double> bin_edges(nBins + 1);

    // Calculate the bin edges logarithmically.
    double logMinE = log10(minE);
    double logMaxE = log10(maxE);
    double logBinWidth = (logMaxE - logMinE) / nBins;

    for (int i = 0; i <= nBins; ++i) {
        bin_edges[i] = pow(10, logMinE + i * logBinWidth);
    }

    // Create one histogram to accumulate data from all runs.
    // Binning: 200 bins from 0 to 1 MeV (5 keV per bin). Adjust range if needed.
    TH1D* h_gamma_ekin = new TH1D("h_gamma_ekin", "Gamma Energy Spectrum (All Runs); Energy (MeV);Counts", nBins, bin_edges.data());

    // Loop through all the runs provided
    for (const auto& run : runs) {
        if (!run.tree) {
            std::cout << "Skipping run with no TTree: " << run.title << std::endl;
            continue;
        }

        // Set up variables and branches to read from the TTree
        Double_t Ekin;
        char particleType[256]; // Use a character array for C-style strings

        run.tree->SetBranchAddress("KineticEnergy", &Ekin);
        run.tree->SetBranchAddress("ParticleType", particleType); // Pass the array directly

        // Loop through all entries in the current run's TTree
        for (Long64_t j = 0; j < run.tree->GetEntries(); ++j) {
            run.tree->GetEntry(j);
            
            // Check if the particle type is "gamma"
            if (strcmp(particleType, "gamma") == 0) {
                h_gamma_ekin->Fill(Ekin); // Fill histogram with the gamma's kinetic energy
            }
        }
        std::cout << "Processed gammas from: " << run.title << std::endl;
    }

    // Create a canvas to draw the histogram
    TCanvas* canvas = new TCanvas("c_gamma_spectrum", "Gamma Kinetic Energy Spectrum", 1200, 800);
    h_gamma_ekin->SetLineColor(kBlue);
    h_gamma_ekin->SetFillColor(kBlue);
    h_gamma_ekin->SetFillStyle(3004);
    h_gamma_ekin->Draw();

    canvas->SetGrid();
    canvas->SetLogx();
    canvas->SetLogy(); // Set Y-axis to logarithmic scale to see the full range of counts
    canvas->SaveAs("gamma_kinetic_energy_spectrum.png");

    // Clean up memory
    // delete canvas;
    // delete h_gamma_ekin;
}



// --- Main function to run the analysis ---
void Plots_Ekin_gamma_spectrum() {

    std::vector<float> energies = {200, 100, 50, 40, 35, 32.5, 31.7};
    auto runs = loadRuns(energies);

    std::cout << "Creating gamma kinetic energy spectrum..." << std::endl;
        plotGammaKineticEnergySpectrum(runs);
        std::cout << "Done." << std::endl;
}