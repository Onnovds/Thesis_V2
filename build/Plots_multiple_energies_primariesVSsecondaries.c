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
    int ParentID;
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

        std::cout << "Energy: " << energyStr << std::endl;

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
void plotKineticEnergy2(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Disable statistics
    std::string canvas_title = std::string("100k") + runs[0].generatedParticleType + "s_KineticEnergy_Primary_Secondary";
    
    // Ensure that ROOT doesn't overwrite the previous canvas
    TCanvas* canvas = new TCanvas(canvas_title.c_str(), "Kinetic Energy for Different Runs", 1400, 1000);
    
    std::vector<int> colours = {kRed, kBlue, kGreen, kMagenta+2, kCyan, kOrange, kYellow, kViolet};
    std::vector<TGraph*> dummyMarkers; // Store to avoid garbage collection
    TLegend* legend = new TLegend(0.132, 0.6, 0.5, 0.9);
    // Global legend entry for line styles 
    TLegendEntry* entryPrimary = legend->AddEntry((TObject*)0, "Primary Particles", "l");
    entryPrimary->SetLineColor(kBlack);
    entryPrimary->SetLineWidth(2);
    
    TLegendEntry* entrySecondary = legend->AddEntry((TObject*)0, "Secondary Particles", "l");
    entrySecondary->SetLineColor(kBlack);
    entrySecondary->SetLineWidth(2);
    entrySecondary->SetLineStyle(2);  // Dashed line

    // Define logarithmic binning
    const int nBins = 100;
    const double xmin = 1e-3;
    const double xmax = runs[runs.size() - 1].generatorEnergy * 1.3;
    double binEdges[nBins + 1];

    double logMin = log(xmin);
    double logMax = log(xmax);
    double delta = (logMax - logMin) / nBins;
    for (int j = 0; j <= nBins; ++j) {
        binEdges[j] = exp(logMin + j * delta);
    }
    
    std::vector<TH1D*> primaryHistograms;
    std::vector<TH1D*> secondaryHistograms;
    
    for (size_t i = 0; i < runs.size(); ++i) {
        if (!runs[i].tree) {
            std::cerr << "Tree not available for run: " << runs[i].generatorEnergy << " MeV." << std::endl;
            continue;
        }

        // Variables for kinetic energy and parent ID
        Double_t Ek;
        Int_t ParentID;

        runs[i].tree->SetBranchAddress("KineticEnergy", &Ek);
        runs[i].tree->SetBranchAddress("ParentID", &ParentID);

        // Create histograms
        std::string histNamePrimary = "hist_primary_" + std::to_string(i);
        std::string histNameSecondary = "hist_secondary_" + std::to_string(i);
        
        TH1D* histPrimary = new TH1D(histNamePrimary.c_str(), "Kinetic Energy Spectrum at Detector", nBins, binEdges);
        TH1D* histSecondary = new TH1D(histNameSecondary.c_str(), "Secondary Particles", nBins, binEdges);
        
        // Fill histograms
        for (Long64_t j = 0; j < runs[i].nentries; ++j) {
            runs[i].tree->GetEntry(j);
            if (Ek > 1e-6) {  
                if (ParentID == 0) {
                    histPrimary->Fill(Ek);
                } else {
                    histSecondary->Fill(Ek);
                }
            }
        }

        std::cout << "amount of primaries " << histPrimary->GetEntries() << std::endl;


        // Store histograms to prevent garbage collection
        primaryHistograms.push_back(histPrimary);
        secondaryHistograms.push_back(histSecondary);

        // Style histograms
        histPrimary->SetLineColor(colours[i % colours.size()]);
        histPrimary->SetLineWidth(4);
        histPrimary->SetMaximum(200000);
        histPrimary->SetMinimum(1);
        
        histSecondary->SetLineColor(colours[i % colours.size()]);
        histSecondary->SetLineWidth(4);
        histSecondary->SetLineStyle(2); // Dashed line for secondary particles
        
        // Set title size (axis labels)
        histPrimary->GetXaxis()->SetTitle("Kinetic Energy [MeV]");
        histPrimary->GetXaxis()->SetTitleSize(0.05);  // Increase title size
        histPrimary->GetXaxis()->SetLabelSize(0.04);  // Increase number size
        histPrimary->GetXaxis()->SetTitleOffset(1.); // Increase offset for more space

        histPrimary->GetYaxis()->SetTitle("Counts [#]");
        histPrimary->GetYaxis()->SetTitleSize(0.05);
        histPrimary->GetYaxis()->SetLabelSize(0.04);
        histPrimary->GetYaxis()->SetTitleOffset(0.9);


        // Draw histograms
        if (i == 0) {
            histPrimary->Draw();
            histSecondary->Draw("SAME");
        } else {
            histPrimary->Draw("SAME");
            histSecondary->Draw("SAME");
        }

        // Add to legend
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

    // Ensure canvas persists
    canvas->SetLeftMargin(0.11);    // Adjust left margin (default ~0.1)
    canvas->SetRightMargin(0.03);   // Reduce right margin (default ~0.1)
    canvas->SetBottomMargin(0.11);  // Increase bottom margin to fit X-axis title
    canvas->SetTopMargin(0.1);     // Reduce top margin slightly
    canvas->SetLogx();
    canvas->SetLogy();
    canvas->SetGrid();
    canvas->Update();
    //canvas->Draw(); // **Prevents deletion of the first canvas**
}



// Main function to load runs and plot histograms
void Plots_multiple_energies_primariesVSsecondaries() {
    // Define specific energies for the runs
    std::vector<float> energies = {31.7, 32.5, 35, 40, 50, 100, 200};

    auto runs = loadRuns(energies);

    // Access data for each run
    for (size_t i = 0; i < runs.size(); ++i) {
        if (runs[i].tree) {
            std::cout << "Run with " << runs[i].generatorEnergy << " MeV has " 
                      << runs[i].nentries << " entriestd::to_string(s." << std::endl;
            runs[i].tree->Print();
        } else {
            std::cout << "Run with " << runs[i].generatorEnergy << " MeV could not load data." << std::endl;
        }
    }

    // Plot kinetic energy
    plotKineticEnergy2(runs);
}


Plots_multiple_energies_primariesVSsecondaries();