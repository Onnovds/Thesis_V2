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

        std::cout << "Energy: " << energyStr << std::endl;

        run.generatedParticles = 100000;
        run.generatorEnergy = energies[i];
        run.generatedParticleType = "Gamma";
        run.worldVolume = "G4_Galactic";
        run.shielding = "Aluminium 5.0mm";

        // Construct the filename
        run.title = std::string("100k") + run.generatedParticleType + energyStr + "MeV_Al_Slab_5mm_Vacuum_fStopandKill_afterSlab";
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
void plotKineticEnergy(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Disable all statistics
    std::string canvas_title = std::string("100k") + runs[0].generatedParticleType + "s_KineticEnergy_Combined";
    TCanvas* canvas = new TCanvas(canvas_title.c_str(), "Kinetic Energy for Different Runs", 1200, 800);
    std::vector<int> colours = {kRed, kBlue, kGreen, kMagenta, kCyan, kOrange};
    TLegend* legend = new TLegend(0.15, 0.7, 0.3, 0.9);

    // Define custom logarithmic bins
    const int nBins = 100;      // Number of bins
    const double xmin = 1e-3;  // Minimum range
    const double xmax = runs[runs.size()-1].generatorEnergy *1.3;   // Maximum range
    double binEdges[nBins + 1];

    //binEdges[1e-3, 5e-3, 1e-2, 5e-2, 1e-1, 5e-1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ,12, 13, 14, 15, 16, 17, 18, 19, 20, 24];

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
        Double_t Ek;

        // Set branch address locally
        runs[i].tree->SetBranchAddress("KineticEnergy", &Ek);
        

        // Create histogram
        std::string histName = "hist_" + std::to_string(i);
        std::string histTitle = std::string("Kinetic Energy of ") + runs[0].generatedParticleType + "s at detector";
        TH1D* hist = new TH1D(histName.c_str(), histTitle.c_str(), nBins, binEdges);
        
        // Fill histogram with data
        for (Long64_t j = 0; j < runs[i].nentries; ++j) {
            runs[i].tree->GetEntry(j);
            if (Ek > 1e-3) {  // Ensure valid kinetic energy values
                hist->Fill(Ek);
                //std::cout << Ek << std::endl;
            }
        }

        // Style and draw histogram
        hist->SetLineColor(colours[i % colours.size()]);
        hist->SetLineWidth(2);
        hist->SetMaximum(200000); // Set Y-axis max to 2000
        hist->GetXaxis()->SetTitle("Kinetic Energy [MeV]");
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
    textBox->AddText("Generated particle type = Gamma");
    textBox->AddText("World material = G4_Galactic");
    textBox->AddText("Shielding = Aluminium 5.0mm");
    textBox->AddText(("Binning method = Logarithmic spacing (" + std::to_string(nBins) + "bins)").c_str());
    textBox->Draw();

    canvas->SetTitle("Kinetic Energy Distribution for Different Runs");
    canvas->SetLogx();  // Optional
    canvas->SetLogy();  // Optional
    canvas->SetGrid();
    canvas->Update();
}

// Main function to load runs and plot histograms
void Plots_multiple_energies() {
    // Define specific energies for the runs
    std::vector<float> energies = {0.02, 0.05, 0.1, 0.5, 1.0};

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
    plotKineticEnergy(runs);
}

