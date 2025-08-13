#include <vector>
#include <string>
#include <iostream>
#include <algorithm> // For std::replace
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TROOT.h>
#include <sstream>
#include <iomanip>
#include <TLatex.h>


// --- HELPER FUNCTIONS
// Replaces dots with underscores for file-safe names.
std::string replaceDotWithUnderscore(const std::string& input) {
    std::string output = input;
    std::replace(output.begin(), output.end(), '.', '_');
    return output;
}

// Formats a float to a string, trimming trailing zeros.
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
    float generatorEnergy;
    TFile* file;
    TTree* tree;
};

// --- DATA LOADING 
// Loads all ROOT files for the given energies.
std::vector<RunData> loadRuns(const std::vector<float>& energies) {
    std::vector<RunData> runs;
    for (const auto& energy : energies) {
        RunData run;
        std::string energyStr = replaceDotWithUnderscore(formatFloat(energy));
        run.generatorEnergy = energy;
        // Construct file title and name based on energy
        run.title = "100kProton" + energyStr + "MeV_NoSlab_5mm_Vacuum_fStopandKill_Omnidirectional";
        run.filename = "/home/onno/satellite_test/build/Protons/" + run.title + ".root";
        run.file = TFile::Open(run.filename.c_str(), "READ");
        
        if (run.file && !run.file->IsZombie()) {
            run.tree = (TTree*)run.file->Get("DetectorData");
            if (!run.tree) {
                std::cerr << "Failed to get TTree 'DetectorData' from file: " << run.filename << std::endl;
            }
        } else {
            std::cerr << "Failed to open file: " << run.filename << std::endl;
        }
        runs.push_back(run);
    }
    return runs;
}

// --- NEW PLOTTING FUNCTION ---
// Generates and saves hit map plots for each run.
void plotHitMaps(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0); // Turn off statistics box

    // Bins for flux plots
    const int bins = 140;
    const double min = -7.0;
    const double max = 7.0;

    for (const auto& run : runs) {
        if (!run.tree) {
            std::cout << "Skipping run with no TTree: " << run.title << std::endl;
            continue;
        }

        // --- Setup branch variables ---
        Double_t x, y;
        Int_t ParentID;
        char Type[20]; // Assuming 'Type' is a char array in the TTree

        run.tree->SetBranchAddress("fX", &x);
        run.tree->SetBranchAddress("fY", &y);
        run.tree->SetBranchAddress("ParentID", &ParentID);
        run.tree->SetBranchAddress("ParticleType", Type);

        // --- Create uniquely named histograms for this run ---
        // Using Form() ensures each run gets its own set of histograms in memory.
        std::string suffix = replaceDotWithUnderscore(formatFloat(run.generatorEnergy));
        TH2F *hTotalHits = new TH2F(Form("hTotalHits_%s", suffix.c_str()), "All Particles;X (mm);Y (mm)", bins, min, max, bins, min, max);
        TH2F *hPrimaryProtons = new TH2F(Form("hPrimaryProtons_%s", suffix.c_str()), "Primary Particles;X (mm);Y (mm)", bins, min, max, bins, min, max);
        TH2F *hSecondaryProtons = new TH2F(Form("hSecondaryProtons_%s", suffix.c_str()), "Secondary Proton Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
        TH2F *hSecondaryElectrons = new TH2F(Form("hSecondaryElectrons_%s", suffix.c_str()), "Secondary Electron Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
        TH2F *hSecondaryGammas = new TH2F(Form("hSecondaryGammas_%s", suffix.c_str()), "Secondary Particles;X (mm);Y (mm)", bins, min, max, bins, min, max); //Title should actually be Gamma but gammas have highest count so this works better for COLZ legend
        TH2F *hSecondaryNeutrons = new TH2F(Form("hSecondaryNeutrons_%s", suffix.c_str()), "Secondary Neutron Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
        TH2F *hSecondaryOthers = new TH2F(Form("hSecondaryOthers_%s", suffix.c_str()), "Other Secondary Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);

        // --- Loop over tree entries to fill histograms ---
        for (Long64_t i = 0; i < run.tree->GetEntries(); i++) {
            run.tree->GetEntry(i);
            hTotalHits->Fill(x, y);

            if (ParentID == 0) { // Primary Particles
                if (strcmp(Type, "proton") == 0) hPrimaryProtons->Fill(x, y);
                // Note: You didn't have primary electrons/gammas in your proton beam, so these will be empty.
            } else { // Secondary Particles
                if (strcmp(Type, "proton") == 0) hSecondaryProtons->Fill(x, y);
                else if (strcmp(Type, "e-") == 0) hSecondaryElectrons->Fill(x, y);
                else if (strcmp(Type, "gamma") == 0) hSecondaryGammas->Fill(x, y);
                else if (strcmp(Type, "neutron") == 0) hSecondaryNeutrons->Fill(x, y);
                else hSecondaryOthers->Fill(x, y);
            }
        }

        // --- Create canvas and draw plots ---
        TCanvas *c = new TCanvas(Form("c_hitmap_%s", suffix.c_str()), Form("Hit Maps for %.1f MeV Protons", run.generatorEnergy), 1800, 600);
        c->Divide(3, 1);

        // Pad 1: Total Hits
        c->cd(1);
        gPad->SetMargin(0.1, 0.15, 0.1, 0.1); // L, R, B, T
        hTotalHits->Draw("COLZ");
        TPaveText *legend_total = new TPaveText(0.4, 0.8, 0.85, 0.9, "NDC");  //Left, bottom, right, top
        legend_total->SetFillColor(0);
        legend_total->SetBorderSize(1);
        legend_total->SetTextFont(42);
        legend_total->SetTextSize(0.045);
        legend_total->SetTextAlign(12);
        legend_total->AddText(Form("Total Particles: %.0f", hTotalHits->GetEntries()));
        legend_total->Draw();

        // Pad 2: Primary Particle Hits
        c->cd(2);
        gPad->SetMargin(0.1, 0.15, 0.1, 0.1);
        hPrimaryProtons->Draw("COLZ");
        // Step 1: Draw an empty box with TPaveText
        TPaveText *legend_primary = new TPaveText(0.37, 0.8, 0.85, 0.9, "NDC");
        legend_primary->SetFillColor(0); // Transparent fill
        legend_primary->SetBorderSize(1);
        legend_primary->Draw();

        // Step 2: Draw the text on top with TLatex
        TLatex *latex_primary = new TLatex();
        latex_primary->SetNDC(); // Use normalised coordinates
        latex_primary->SetTextFont(42);
        latex_primary->SetTextSize(0.045);
        latex_primary->SetTextAlign(12); // Left-aligned, bottom
        // Adjust coordinates to place text inside the box
        latex_primary->DrawLatex(0.39, 0.85, Form("Primary Protons: %.0f", hPrimaryProtons->GetEntries()));
        //TPaveText *legend_primary = new TPaveText(0.4, 0.8, 0.85, 0.9, "NDC");
        //legend_primary->SetFillColor(0);
        //legend_primary->SetBorderSize(1);
        //// --- NEW TLatex method for precise control ---
        //TLatex *latex_primary = new TLatex();
        //latex_primary->SetNDC(); // Use normalised coordinates (0 to 1)
        //latex_primary->SetTextFont(42);
        //latex_primary->SetTextSize(0.035); // This will now be respected
        //latex_primary->SetTextAlign(12); // 1 = Left, 2 = Bottom alignment
        //// Draw the text at position (x, y)
        //latex_primary->DrawLatex(0.4, 0.83, Form("Primary Protons: %.0f", hPrimaryProtons->GetEntries()));
        //legend_primary->SetFillColor(0);
        //legend_primary->SetBorderSize(1);
        //legend_primary->SetTextFont(42);
        //legend_primary->SetTextSize(0.035);
        //legend_primary->SetTextAlign(12);
       //legend_primary->AddText(Form("Primary Protons: %.0f", hPrimaryProtons->GetEntries()));
        //legend_primary->Draw();

        // Pad 3: Secondary Particle Hits
        c->cd(3);
        gPad->SetMargin(0.1, 0.15, 0.1, 0.1);
        // Draw the one with the most entries first to set the Z-axis scale
        hSecondaryGammas->Draw("SAME COL");  //--> gammas always have highest count in secondaries
        // Then draw others on top (without "COLZ" to avoid repainting the axis)
        hSecondaryProtons->Draw("SAME COL");
        hSecondaryElectrons->Draw("SAME COLZ"); 
        hSecondaryNeutrons->Draw("SAME COL");
        hSecondaryOthers->Draw("SAME COL");
        TPaveText *legend_secondary = new TPaveText(0.42, 0.65, 0.85, 0.9, "NDC");
        legend_secondary->SetFillColor(0);
        legend_secondary->SetBorderSize(1);
        legend_secondary->SetTextFont(42);
        legend_secondary->SetTextSize(0.045);
        legend_secondary->SetTextAlign(12);
        legend_secondary->AddText(Form("Sec. Protons: %.0f", hSecondaryProtons->GetEntries()));
        legend_secondary->AddText(Form("Sec. Electrons: %.0f", hSecondaryElectrons->GetEntries()));
        legend_secondary->AddText(Form("Sec. Gammas: %.0f", hSecondaryGammas->GetEntries()));
        legend_secondary->AddText(Form("Sec. Neutrons: %.0f", hSecondaryNeutrons->GetEntries()));
        legend_secondary->AddText(Form("Sec. Others: %.0f", hSecondaryOthers->GetEntries()));
        legend_secondary->Draw();

        
        
        // --- Save the canvas ---
        std::string outputDir = "/home/onno/Documents/Thesis/Results figures and plots/Satellite simulation results/";
        std::string outputFile = outputDir + run.title + "_SpatialDistribution.pdf";
        c->SaveAs(outputFile.c_str());

        // --- Clean up memory for this loop iteration ---
        delete c;
        delete hTotalHits;
        delete hPrimaryProtons;
        delete hSecondaryProtons;
        delete hSecondaryElectrons;
        delete hSecondaryGammas;
        delete hSecondaryNeutrons;
        delete hSecondaryOthers;
        delete legend_total;
        delete legend_primary;
        delete legend_secondary;
    }
}


// --- MAIN FUNCTION TO RUN THE ANALYSIS ---
// To run in ROOT: root -l -q Plots_HitMaps.c
void Plots_HitMaps() {
    // Define the list of energies you want to process
    std::vector<float> energies = {31.7, 32.5, 35.0, 40, 50, 100, 200}; // Add all your energies here

    // Load the data
    std::vector<RunData> runs = loadRuns(energies);

    // Generate and save the hit map plots for all runs
    plotHitMaps(runs);
}