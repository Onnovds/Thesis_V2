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

int numBins = 200;

// Create TProfiles for all particles from a list of runs 
std::map<double, TProfile*> createCombinedProfiles(const std::vector<RunData>& runs) {
    std::map<double, TProfile*> profiles;
    for (const auto& run : runs) {
        if (!run.tree) continue;

        Double_t Ekin, Edep;
        run.tree->SetBranchAddress("KineticEnergy", &Ekin);
        run.tree->SetBranchAddress("DepositedEnergy", &Edep);

        // Create a uniqu 2D histogram for each run
        TH2D* h2d = new TH2D(Form("h2d_all_%.1fMeV", run.generatorEnergy), "Ekin vs Edep", numBins, 0, 200, 200, 0, 3000);

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

//Plot the combined profiles on one canvas ---
void plotCombinedProfiles(const std::map<double, TProfile*>& profiles) {
    gStyle->SetOptStat(0);
    std::string title = "100kProtons_Edep_vs_Ekin_Profiles_Omnidirectional";
    TCanvas* canvas = new TCanvas(title.c_str(), "Average Edep vs Ekin", 1200, 800);
    TLegend* legend = new TLegend(0.6, 0.6, 0.88, 0.88);
    legend->SetHeader("Generator Energy", "C");

    std::vector<int> colors = {kRed, kBlue, kGreen, kMagenta+2, kCyan, kOrange, kYellow, kViolet};
    int color_index = 0;

    bool is_first_plot = true;
    for (auto const& [energy, profile_ptr] : profiles) {
        profile_ptr->SetLineColor(colors[color_index % colors.size()]);
        profile_ptr->SetLineWidth(3);

        if (is_first_plot) {
            profile_ptr->SetTitle("Average Deposited Energy vs. Kinetic Energy");
            profile_ptr->GetXaxis()->SetTitle("Kinetic Energy (Ekin) [MeV]");
            profile_ptr->GetYaxis()->SetTitle("Average Deposited Energy (Edep) [keV]");
            profile_ptr->GetYaxis()->SetRangeUser(0, 2000); // Set a common Y-axis range
            profile_ptr->Draw();
            is_first_plot = false;
        } else {
            profile_ptr->Draw("SAME");
        }
        legend->AddEntry(profile_ptr, Form("%.1f MeV", energy), "l");
        color_index++;
    }

    legend->Draw();
    canvas->SetGrid();
    canvas->SaveAs("profiles_combined.png");
}

// --- NEW FUNCTION: Create and plot a separate 3-panel canvas for each run ---
void plotSeparateProfiles(const std::vector<RunData>& runs) {
    gStyle->SetOptStat(0);
    for (const auto& run : runs) {
        if (!run.tree) continue;

        TCanvas* canvas = new TCanvas(Form("c_separate_%.1f", run.generatorEnergy), Form("Profile for %.1f MeV", run.generatorEnergy), 1800, 600);
        canvas->Divide(3, 1);

        Double_t Ekin, Edep;
        Int_t ParentID;
        run.tree->SetBranchAddress("KineticEnergy", &Ekin);
        run.tree->SetBranchAddress("DepositedEnergy", &Edep);
        run.tree->SetBranchAddress("ParentID", &ParentID);

        // Create three uniquely named 2D histograms
        TH2D* h2d_all   = new TH2D(Form("h2d_all_%.1f", run.generatorEnergy), "All", numBins, 0, run.generatorEnergy, 100, 0, 5000);
        TH2D* h2d_prims = new TH2D(Form("h2d_prims_%.1f", run.generatorEnergy), "Primaries", numBins, 0, run.generatorEnergy, 100, 0, 5000);
        TH2D* h2d_secs  = new TH2D(Form("h2d_secs_%.1f", run.generatorEnergy), "Secondaries", numBins, 0, run.generatorEnergy, 100, 0, 5000);

        for (Long64_t j = 0; j < run.tree->GetEntries(); ++j) {
            run.tree->GetEntry(j);
            if (Ekin>0 && Edep>0) {
            h2d_all->Fill(Ekin, Edep*1e3);
            if (ParentID == 0) {h2d_prims->Fill(Ekin, Edep*1e3);}
            else {h2d_secs->Fill(Ekin, Edep*1e3);}}
        }
        
        TProfile* prof_all   = h2d_all->ProfileX(Form("prof_all_%.1f", run.generatorEnergy));
        TProfile* prof_prims = h2d_prims->ProfileX(Form("prof_prims_%.1f", run.generatorEnergy));
        TProfile* prof_secs  = h2d_secs->ProfileX(Form("prof_secs_%.1f", run.generatorEnergy));

        // Plotting on the 3 pads
        canvas->cd(1);
        gPad->SetLeftMargin(0.13);
        prof_all->SetTitle("All Particles");
        prof_all->GetXaxis()->SetTitle("Kinetic Energy (Ekin) [MeV]");
        prof_all->GetYaxis()->SetTitle("Average Deposited Energy (Edep) [keV]");
        prof_all->Draw();

        canvas->cd(2);
        gPad->SetLeftMargin(0.13);
        prof_prims->SetTitle("Primary Particles");
        prof_prims->GetXaxis()->SetTitle("Kinetic Energy (Ekin) [MeV]");
        prof_prims->GetYaxis()->SetTitle("Average Deposited Energy (Edep) [keV]");
        prof_prims->Draw();

        canvas->cd(3);
        gPad->SetLeftMargin(0.13);
        prof_secs->SetTitle("Secondary Particles");
        prof_secs->GetXaxis()->SetTitle("Kinetic Energy (Ekin) [MeV]");
        prof_secs->GetYaxis()->SetTitle("Average Deposited Energy (Edep) [keV]");        
        prof_secs->Draw();

        canvas->SaveAs(Form("profile_separate_%.1fMeV.png", run.generatorEnergy));
        
        // Clean up to prevent memory issues in a long script
        // delete canvas; delete h2d_all; delete h2d_prims; delete h2d_secs;
    }
}


// --- UPDATED FUNCTION: Exports TProfile data to a CSV file instead of std::cout
void exportProfileData(const std::map<double, TProfile*>& profiles) {
    // 1. Define the list of energies you want to export
    std::vector<double> targetEnergies = {32.5, 35.0, 40.0, 50.0, 100.0, 200.0};

    // 2. Create and open the output file ONCE
    std::ofstream dataFile("profile_data_all.csv");
    if (!dataFile.is_open()) {
        std::cerr << "Error: Could not open file for writing." << std::endl;
        return;
    }

    std::cout << "Exporting profile data to profile_data_all.csv..." << std::endl;

    // 3. Write the header line ONCE
    dataFile << "Ekin_MeV,Avg_Edep_keV\n";

    // 4. Loop through each target energy
    for (double energy : targetEnergies) {
        // Check if the profile for this energy exists in the map
        if (profiles.count(energy)) {
            TProfile* profile = profiles.at(energy);

            // Loop through the bins of the current profile and append data
            for (int i = 1; i <= profile->GetNbinsX(); ++i) {
                if (profile->GetBinEntries(i) > 0) {
                    double ekin = profile->GetBinCenter(i);
                    double avg_edep = profile->GetBinContent(i);
                    dataFile << ekin << "," << avg_edep << "\n";
                }
            }
        } else {
            // Warn if a specific energy's profile is missing, but continue
            std::cerr << "Warning: Could not find profile for " << energy << " MeV run. Skipping." << std::endl;
        }
    }
    
    // 5. Close the file ONCE at the very end
    dataFile.close();
    std::cout << "Data export complete." << std::endl;
}



// --- Main function to run the analysis ---
void Plots_EkinVsEdep_profiles() {
    // --- Main Control Flags ---
    bool plotCombined = true; // Set to true to get one plot with all profiles
    bool plotSeparate = true; // Set to true to get a separate 3-panel plot for each energy

    // --- Workflow ---
    std::vector<float> energies = {200, 100, 50, 40, 35, 32.5, 31.7};
    auto runs = loadRuns(energies);

    if (plotCombined) {
        std::cout << "Creating combined profile plot..." << std::endl;
        auto profiles = createCombinedProfiles(runs);
        exportProfileData(profiles); // Export the relation between Edep and Ekin
        plotCombinedProfiles(profiles);
        std::cout << "Done." << std::endl;
    }

    if (plotSeparate) {
        std::cout << "Creating separate profile plots for each run..." << std::endl;
        plotSeparateProfiles(runs);
        std::cout << "Done." << std::endl;
    }
}