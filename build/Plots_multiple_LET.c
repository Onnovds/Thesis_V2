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
        run.filename = run.title + ".root";


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
void plotLET(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Disable statistics box
    std::string canvas_title = std::string("100k") + runs[0].generatedParticleType + "s_LET_Combined";
    TCanvas* canvas = new TCanvas(canvas_title.c_str(), "LET for Different Runs", 1400, 1000);

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
        Double_t LET;
        Int_t ParentID;

        // Set branch addresses
        runs[i].tree->SetBranchAddress("LET", &LET);
        runs[i].tree->SetBranchAddress("ParentID", &ParentID);
        

        // Create histograms
        std::string histPrimaryName = "hLET_primary_" + std::to_string(i);
        std::string histSecondaryName = "hLET_secondary_" + std::to_string(i);
        
        TH1D* hLET_primary = new TH1D(histPrimaryName.c_str(), "LET Spectrum at Detector", nBins, binEdges);
        TH1D* hLET_secondary = new TH1D(histSecondaryName.c_str(), "Primary vs Secondary LET", nBins, binEdges);

        // Fill histograms
        for (Long64_t j = 0; j < runs[i].nentries; ++j) {
            runs[i].tree->GetEntry(j);

            if (ParentID == 0) {
                    hLET_primary->Fill(LET);
                } else {
                    hLET_secondary->Fill(LET);
                }
        }

        std::cout << "Mean LET for primaries " << hLET_primary->GetEntries() << std::endl;


        // Style histograms
        hLET_primary->SetLineColor(colours[i % colours.size()]);
        hLET_primary->SetLineWidth(4);
        hLET_secondary->SetLineColor(colours[i % colours.size()]);
        hLET_secondary->SetLineWidth(4);
        hLET_secondary->SetLineStyle(2); // Dashed line for secondary

        hLET_primary->SetMaximum(200000);
        hLET_primary->SetMinimum(1);
        

        // Set title size (axis labels)
        hLET_primary->GetXaxis()->SetTitle("LET [keV/um]");
        hLET_primary->GetXaxis()->SetTitleSize(0.05);  // Increase title size
        hLET_primary->GetXaxis()->SetLabelSize(0.04);  // Increase number size
        hLET_primary->GetXaxis()->SetTitleOffset(1.); // Increase offset for more space

        hLET_primary->GetYaxis()->SetTitle("Counts [#]");
        hLET_primary->GetYaxis()->SetTitleSize(0.05);
        hLET_primary->GetYaxis()->SetLabelSize(0.04);
        hLET_primary->GetYaxis()->SetTitleOffset(0.9);
        
        if (i == 0) {
            hLET_primary->Draw();
            hLET_secondary->Draw("SAME");
        } else {
            hLET_primary->Draw("SAME");
            hLET_secondary->Draw("SAME");
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


/*
void plotLET(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Disable all statistics
    std::string canvas_title = std::string("100k") + runs[0].generatedParticleType + "s_LET_Combined";
    TCanvas* canvas = new TCanvas(canvas_title.c_str(), "LET for Different Runs", 1200, 800);
    std::vector<int> colours = {kRed, kBlue, kGreen, kMagenta, kCyan, kOrange};
    TLegend* legend = new TLegend(0.15, 0.7, 0.3, 0.9);

    // Define custom logarithmic bins
    const int nBins = 70;      // Number of bins
    const double xmin = 1e-3;  // Minimum range
    const double xmax = 300;   // Maximum range
    // const double xmax = runs[runs.size()-1].generatorEnergy *1.3;   // Maximum range    ---> AUTOMISE THIS FOR LET
    double binEdges[nBins + 1];


    // Calculate logarithmic bin edges
    double logMin = log(xmin);
    double logMax = log(xmax);
    double delta = (logMax - logMin) / nBins;

    for (int j = 0; j <= nBins; ++j) {
        binEdges[j] = exp(logMin + j * delta);
        // std::cout << binEdges[j] << std::endl; // Print bin edges for checking
    }
    
    
    for (size_t i = 0; i < runs.size(); ++i) {
        if (!runs[i].tree) {
            std::cerr << "Tree not available for run: " << runs[i].generatorEnergy << " MeV." << std::endl;
            continue;
        }

    // Unique variable for KineticEnergy
        Double_t LET;

        // Set branch address locally
        runs[i].tree->SetBranchAddress("LET", &LET);
        

        // Create histogram
        std::string histName = "hist_" + std::to_string(i);
        std::string histTitle = std::string("LET of ") + runs[0].generatedParticleType + "s at detector";
        TH1D* hist = new TH1D(histName.c_str(), histTitle.c_str(), nBins, binEdges);
        
        // Fill histogram with data
        for (Long64_t j = 0; j < runs[i].nentries; ++j) {
            runs[i].tree->GetEntry(j);
            if (LET > 1e-3) {  // Ensure valid kinetic energy values
                hist->Fill(LET);
                //std::cout << Ek << std::endl;
            }
        }

        // Style and draw histogram
        hist->SetLineColor(colours[i % colours.size()]);
        hist->SetLineWidth(2);
        hist->SetMaximum(200000); // Set Y-axis max to 2000
        hist->GetXaxis()->SetTitle("LET [keV/um]");
        hist->GetYaxis()->SetTitle("Counts [#]");
        hist->GetXaxis()->SetLabelOffset(-0.005); // Increase offset for more space
        if (i == 0) {
            hist->Draw();
            legend->AddEntry((TObject*)0, "Energy at Generator", ""); // Add custom entry
        } else {
            hist->Draw("SAME");
        }
        std::ostringstream LegendIdentifier;
        LegendIdentifier << std::fixed << std::setprecision(2) << runs[i].generatorEnergy;  // Fixed with 1 or 2 decimal places
        legend->AddEntry(hist, (LegendIdentifier.str() + " MeV").c_str(), "l");
    }
    
    legend->Draw();

    // Create a TPaveText box to match the legend style
    TPaveText *textBox = new TPaveText(0.3, 0.7, 0.6, 0.9, "NDC"); // Normalized coordinates
    textBox->SetFillColor(0);       // White background
    textBox->SetLineColor(kBlack);  // Black border
    textBox->SetLineWidth(1);       // Thin black border
    textBox->SetTextFont(42);       // Font type to match ROOT legend
    textBox->SetTextSize(0.02);     // Font size
    textBox->SetTextAlign(12);      // Align text to the left
    textBox->SetShadowColor(0);     // No shadow


    // Add text lines
    textBox->AddText("Generated particles = 100k");
    textBox->AddText("Generated particle type = Proton");
    textBox->AddText("World material = G4_Galactic");
    textBox->AddText("Shielding = Aluminium 5.0mm");
    textBox->AddText(("Binning method = Logarithmic spacing (" + std::to_string(nBins) + "bins)").c_str());
    textBox->Draw();

    canvas->SetTitle("LET Distribution for Different Runs");
    canvas->SetLogx();  // Optional
    canvas->SetLogy();  // Optional
    canvas->SetGrid();
    canvas->Update();
}
*/
// Main function to load runs and plot histograms
void Plots_multiple_LET() {
    // Define specific energies for the runs
    std::vector<float> energies = {31.7, 32.5, 35, 40, 50, 70, 100, 200};

    auto runs = loadRuns(energies);

    // Access data for each run
    for (size_t i = 0; i < runs.size(); ++i) {
        if (runs[i].tree) {
            std::cout << "Run with " << runs[i].generatorEnergy << " MeV has " 
                      << runs[i].nentries << " entries" << std::endl;
            // runs[i].tree->Print();
            // Calculate mean LET using a histogram
            runs[i].tree->Draw("LET>>hLET"); // Create a histogram "hLET" with the LET data
            TH1* hLET = (TH1*)gDirectory->Get("hLET");

            if (hLET) {
                double mean_LET = hLET->GetMean();
                std::cout << "Mean LET: " << mean_LET << " keV/um" << std::endl;
                delete hLET; // Clean up to avoid memory leaks
            } else {
                std::cerr << "Error: Histogram 'hLET' not found for run with " 
                          << runs[i].generatorEnergy << " MeV." << std::endl;
            }
        } else {
            std::cout << "Run with " << runs[i].generatorEnergy << " MeV could not load data." << std::endl;
        }
        
    }

    // Plot kinetic energy
    plotLET(runs);

   
}






