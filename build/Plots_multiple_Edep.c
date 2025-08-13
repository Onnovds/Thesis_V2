#include <vector>
#include <string>
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TROOT.h>
#include <sstream>
#include <iomanip>

// Helper function to replace '.' with '_' in a string
std::string replaceDotWithUnderscore(const std::string& input) {
    std::string output = input;
    std::replace(output.begin(), output.end(), '.', '_');
    return output;
}

// Helper function to format floats without unnecessary trailing zeroes
std::string formatFloat(float value) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << value;
    std::string result = stream.str();

    // Remove trailing ".0" for integers
    if (result.find('.') != std::string::npos) {
        result.erase(result.find_last_not_of('0') + 1, std::string::npos);
        if (result.back() == '.') {
            result.pop_back();
        }        
    }
    return result;
}

// Struct to store metadata and pointers to files/trees
struct RunData {
    std::string title;
    std::string filename;
    float generatedParticles;
    float generatorEnergy;
    const char* generatedParticleType;
    const char* worldVolume;
    const char* shielding;
    TFile* file;     // Pointer to ROOT file
    TTree* tree;     // Pointer to TTree
    Long64_t nentries; // Number of entries
};

// Function to load runs into RunData
std::vector<RunData> loadRuns(const std::vector<float>& energies) {
    std::vector<RunData> runs;

    for (size_t i = 0; i < energies.size(); ++i) {
        RunData run;

        // Format energy into a string
        std::string energyStr = replaceDotWithUnderscore(formatFloat(energies[i]));

        // std::cout << "Energy: " << energyStr << std::endl;

        run.generatedParticles = 100000;
        run.generatorEnergy = energies[i];
        run.generatedParticleType = "Proton";
        run.worldVolume = "G4_Galactic";
        run.shielding = "Aluminium 5.0mm";

        // Construct the filename
        run.title = std::string("100k") + run.generatedParticleType + energyStr + "MeV_Al_Slab_5mm_Vacuum_fStopandKill_Omnidirectional";
        run.filename = std::string("/home/onno/satellite_test/build/") + run.generatedParticleType + "s/" + run.title + ".root";


        // Open the ROOT file
        run.file = TFile::Open(run.filename.c_str(), "READ");
        if (run.file && !run.file->IsZombie()) {
            run.tree = (TTree*)run.file->Get("DetectorData");
            if (run.tree) {
                run.nentries = run.tree->GetEntries();
            } else {
                std::cerr << "Failed to load tree from file: " << run.filename << std::endl;
            }
        } else {
            std::cerr << "Failed to open file: " << run.filename << std::endl;
        }

        runs.push_back(run);
    }

    return runs;
}

// Function to create histograms and plot
void plotEdep(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Disable statistics box
    std::string canvas_title = std::string("100k") + runs[0].generatedParticleType + "s_Edep_Combined";
    TCanvas* canvas = new TCanvas(canvas_title.c_str(), "Deposited Energy for Different Runs", 1400, 1000);

    std::vector<int> colours = {kRed, kBlue, kGreen, kMagenta+2, kCyan, kOrange, kYellow, kViolet}; // For primary particles
    std::vector<TGraph*> dummyMarkers; // Store to avoid garbage collection

    TLegend* legend = new TLegend(0.12, 0.6, 0.4, 0.9);
    // Global legend entry for line styles 
    TLegendEntry* entryPrimary = legend->AddEntry((TObject*)0, "Primary Particles", "l");
    entryPrimary->SetLineColor(kBlack);
    entryPrimary->SetLineWidth(2);
    
    TLegendEntry* entrySecondary = legend->AddEntry((TObject*)0, "Secondary Particles", "l");
    entrySecondary->SetLineColor(kBlack);
    entrySecondary->SetLineWidth(2);
    entrySecondary->SetLineStyle(2);  // Dashed line

    // Define custom logarithmic bins
    const int nBins = 100;
    const double xmin = 1e-3;
    const double xmax = 10000;
    double binEdges[nBins + 1];

    double logMin = log(xmin);
    double logMax = log(xmax);
    double delta = (logMax - logMin) / nBins;

    for (int j = 0; j <= nBins; ++j) {
        binEdges[j] = exp(logMin + j * delta);
    }

    for (size_t i = 0; i < runs.size(); ++i) {
        if (!runs[i].tree) {
            std::cerr << "Tree not available for run: " << runs[i].generatorEnergy << " MeV." << std::endl;
            continue;
        }

        // Variables to store values
        Double_t Edep;
        Int_t ParentID;

        // Set branch addresses
        runs[i].tree->SetBranchAddress("DepositedEnergy", &Edep);
        runs[i].tree->SetBranchAddress("ParentID", &ParentID);
        

        // Create histograms
        std::string histPrimaryName = "hEdep_primary_" + std::to_string(i);
        std::string histSecondaryName = "hEdep_secondary_" + std::to_string(i);
        
        TH1D* hEdep_primary = new TH1D(histPrimaryName.c_str(), "Deposited Energy Spectrum at Detector", nBins, binEdges);
        TH1D* hEdep_secondary = new TH1D(histSecondaryName.c_str(), "Primary vs Secondary Deposited Energy", nBins, binEdges);

        // Fill histograms
        for (Long64_t j = 0; j < runs[i].nentries; ++j) {
            runs[i].tree->GetEntry(j);
            if (Edep>0){
            Edep *= 1000.0; // Convert from MeV to keV

            if (ParentID == 0) {
                    hEdep_primary->Fill(Edep);
                } else {
                    hEdep_secondary->Fill(Edep);
                }}
        }


        // Style histograms
        hEdep_primary->SetLineColor(colours[i % colours.size()]);
        hEdep_primary->SetLineWidth(4);
        hEdep_secondary->SetLineColor(colours[i % colours.size()]);
        hEdep_secondary->SetLineWidth(4);
        hEdep_secondary->SetLineStyle(2); // Dashed line for secondary

        hEdep_primary->SetMaximum(20000);
        hEdep_primary->SetMinimum(1);
        

        // Set title size (axis labels)
        hEdep_primary->GetXaxis()->SetTitle("Edep [keV]");
        hEdep_primary->GetXaxis()->SetTitleSize(0.05);  // Increase title size
        hEdep_primary->GetXaxis()->SetLabelSize(0.04);  // Increase number size
        hEdep_primary->GetXaxis()->SetTitleOffset(1.); // Increase offset for more space

        hEdep_primary->GetYaxis()->SetTitle("Counts [-]");
        hEdep_primary->GetYaxis()->SetTitleSize(0.05);
        hEdep_primary->GetYaxis()->SetLabelSize(0.04);
        hEdep_primary->GetYaxis()->SetTitleOffset(0.9);
        
        if (i == 0) {
            hEdep_primary->Draw();
            hEdep_secondary->Draw("SAME");
        } else {
            hEdep_primary->Draw("SAME");
            hEdep_secondary->Draw("SAME");
        }

        

        std::ostringstream LegendIdentifier;
        LegendIdentifier << std::fixed << std::setprecision(2) << runs[i].generatorEnergy << " MeV";
    
        // Create a dummy graph with a single point
        TGraph* markerGraph = new TGraph(1);
        markerGraph->SetPoint(0, 0, 0);  // Place it at (0,0) just as a placeholder
         // Set marker properties
        markerGraph->SetMarkerColor(colours[i % colours.size()]);
        markerGraph->SetMarkerStyle(8);  // Large dot
        markerGraph->SetMarkerSize(2.5);  // Make it visible
         // Store to prevent garbage collection
        dummyMarkers.push_back(markerGraph);
         // Add marker to the legend
        legend->AddEntry(markerGraph, LegendIdentifier.str().c_str(), "p");
    }

    legend->Draw();
    canvas->SetLeftMargin(0.11);    // Adjust left margin (default ~0.1)
    canvas->SetRightMargin(0.03);   // Reduce right margin (default ~0.1)
    canvas->SetBottomMargin(0.11);  // Increase bottom margin to fit X-axis title
    canvas->SetTopMargin(0.1);     // Reduce top margin slightly
    canvas->SetLogx();
    canvas->SetLogy();
    canvas->SetGrid();
    canvas->Update();
}


// Main function to load runs and plot histograms
void Plots_multiple_Edep() {
    // Define specific energies for the runs
    std::vector<float> energies = {31.7, 32.5, 35, 40, 50, 100, 200};

    auto runs = loadRuns(energies);

    // Access data for each run
    for (size_t i = 0; i < runs.size(); ++i) {
        if (runs[i].tree) {
            std::cout << "Run with " << runs[i].generatorEnergy << " MeV has " 
                      << runs[i].nentries << " entries" << std::endl;
            // runs[i].tree->Print();
            // Calculate mean Edep using a histogram
            runs[i].tree->Draw("DepositedEnergy>>hEdep"); // Create a histogram "hEdep" with the Edep data
            TH1* hEdep = (TH1*)gDirectory->Get("hEdep");

            if (hEdep) {
                int lastBin = hEdep->FindLastBinAbove(0); 
                double mean_Edep = hEdep->GetMean();
                double max_Edep = hEdep->GetXaxis()->GetBinCenter(lastBin);;
                std::cout << "Mean Edep: " << mean_Edep << " keV" << std::endl;
                std::cout << "Max Edep: " << max_Edep*1e3 << " keV" << std::endl;
                delete hEdep; // Clean up to avoid memory leaks
            } else {
                std::cerr << "Error: Histogram 'hEdep' not found for run with " 
                          << runs[i].generatorEnergy << " MeV." << std::endl;
            }
        } else {
            std::cout << "Run with " << runs[i].generatorEnergy << " MeV could not load data." << std::endl;
        }
        
    }

    // Plot kinetic energy
    plotEdep(runs);

   
}
