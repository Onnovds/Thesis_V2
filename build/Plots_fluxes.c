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

{   float GeneratedParticles = 100000;
    float GeneratorEnergy = 35;
    const char* GeneratedParticleType = "Proton";
    const char* WorldVolume = "G4_Air";
    const char* Shielding = "Aluminium 5.0mm";

    std::string identifier = std::string("100k") + GeneratedParticleType;
    std::string energyStr = replaceDotWithUnderscore(formatFloat(GeneratorEnergy));
    std::string title = identifier + energyStr + "MeV_Al_Slab_5mm_Vacuum_fStopandKill_Omnidirectional";  // _Omnidirectional   or  _afterSlab_V4  or _afterSlab
    std::string filename = std::string("/home/onno/satellite_test/build/") + GeneratedParticleType + "s/" + title + ".root";   // Use the title in the filename

    TFile *file = TFile::Open(filename.c_str());
    TTree *tree = (TTree*)file->Get("DetectorData");

    Long64_t nentries = tree->GetEntries();

    gStyle->SetOptStat(0); // Disable all statistics

    Double_t x, y, z, Ek, Edep, LET;
    Int_t events, ParentID;
    char Type[20];

    tree->SetBranchAddress("fEvent", &events);
    tree->SetBranchAddress("fX", &x);
    tree->SetBranchAddress("fY", &y);
    tree->SetBranchAddress("fZ", &z);
    tree->SetBranchAddress("KineticEnergy", &Ek);
    tree->SetBranchAddress("DepositedEnergy", &Edep);
    tree->SetBranchAddress("LET", &LET);
    tree->SetBranchAddress("ParticleType", &Type);
    tree->SetBranchAddress("ParentID", &ParentID);

    Double_t minLET = std::numeric_limits<Double_t>::max();
    Double_t maxLET = -std::numeric_limits<Double_t>::max();
    Double_t minEdep = std::numeric_limits<Double_t>::max();
    Double_t maxEdep = -std::numeric_limits<Double_t>::max();

      // Loop to find min and maxa values
    for (Long64_t j = 0; j < nentries; j++) {
        tree->GetEntry(j);
        Edep*=1000; 
        if (LET < minLET) minLET = LET;
        if (LET > maxLET) maxLET = LET;
        if (Edep < minEdep) minEdep = Edep;
        if (Edep > maxEdep) maxEdep = Edep;
    }

    std::cout << "Min LET: " << minLET << " keV/#mum" << std::endl;
    std::cout << "Max LET: " << maxLET << " keV/#mum" << std::endl;
    std::cout << "Max Edep: " << maxEdep << " keV" << std::endl;

    // Bins for flux plots
    int bins = 140;
    int min = -7;
    int max = 7;

    TH2F *hTotalHits = new TH2F("hTotalHits", "Total Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hPrimaryProtons = new TH2F("hPrimaryProtons", "Hits of primary particles;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryProtons = new TH2F("hSecondaryProtons", "Hits of secondary particles;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hPrimaryElectrons = new TH2F("hPrimaryElectrons", "Hits of primary electrons;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryElectrons = new TH2F("hSecondaryElectrons", "Hits of secondary electrons;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hPrimaryGammas = new TH2F("hPrimaryGammas", "Hits of primary gammas;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryGammas = new TH2F("hSecondaryGammas", "Hits of secondary gammas;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryNeutrons = new TH2F("hSecondaryNeutrons", "Hits of neutrons;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryOthers = new TH2F("hSecondaryOthers", "Hits of secondary others;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hEdepVsLET = new TH2F("hEdepVsLET", "Deposited Energy vs. LET;LET (keV/#mum);Deposited Energy (MeV)", 100, 0, 120, 100, 0, 2.5);
    TH2F *hEkinVsLET = new TH2F("hEkinVsLET", "Kinetic Energy vs. LET;LET (keV/#mum);Kinetic Energy (MeV)", 100, 0, 110, 100, 0, GeneratorEnergy);
    TH2F *hEkinVsLET_prims = new TH2F("hEkinVsLET", "Kinetic Energy vs. LET (primaries);LET (keV/#mum);Kinetic Energy (MeV)", 100, 0, 110, 100, 0, GeneratorEnergy);
    TH2F *hEkinVsLET_secs = new TH2F("hEkinVsLET", "Kinetic Energy vs. LET (secondaries);LET (keV/#mum);Kinetic Energy (MeV)", 100, 0, maxLET+maxLET/50, 100, 0, GeneratorEnergy);
    TH2F *hEkinVsEdep = new TH2F("hEkinVsEdep", "Kinetic vs. Deposited Energy;Deposited Energy (keV);Kinetic Energy (MeV)", 100, 0, GeneratorEnergy, 100, 0, 1200);
    TH2F *hEkinVsEdep_prims = new TH2F("hEkinVsEdep", "Kinetic vs. Deposited Energy;Deposited Energy (keV);Kinetic Energy (MeV)", 100, 0, GeneratorEnergy, 100, 0, 1200);
    TH2F *hEkinVsEdep_secs = new TH2F("hEkinVsEdep", "Kinetic vs. Deposited Energy;Deposited Energy (keV);Kinetic Energy (MeV)", 100, 0, GeneratorEnergy, 100, 0, maxEdep);
 
    // Loop over all entries in the tree to fill the histograms
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        hTotalHits->Fill(x, y);

        if (Edep > 0 && LET > 0) { // Only fill physically meaningful values
             hEdepVsLET->Fill(LET, Edep);
        }

        if (Ek > 0 && LET > 0) { // Only fill physically meaningful values
            hEkinVsLET->Fill(LET, Ek);
            if (ParentID == 0) {
                hEkinVsLET_prims->Fill(LET, Ek);}
            else {
                hEkinVsLET_secs->Fill(LET, Ek);
           }
        }

        if (Ek > 0 && Edep > 0) { // Only fill physically meaningful values
            hEkinVsEdep->Fill(Ek, Edep*1e3);
            if (ParentID == 0) {
                hEkinVsEdep_prims->Fill(Ek, Edep*1e3);}
            else {
                hEkinVsEdep_secs->Fill(Ek, Edep*1e3);
           }
        }

         // Primary Particles (ParentID == 0)
        if (ParentID == 0) {
            if (strcmp(Type, "proton") == 0) {
                hPrimaryProtons->Fill(x, y);}
            else if (strcmp(Type, "e-") == 0) {
                hPrimaryElectrons->Fill(x, y);}
            else if (strcmp(Type, "gamma") == 0) {
                hPrimaryGammas->Fill(x, y);}
        }
        // Secondary Particles (ParentID > 0)
        //else if (Ek>0 && Edep>0) {
            if (strcmp(Type, "proton") == 0) {
              hSecondaryProtons->Fill(x, y);}
            else if (strcmp(Type, "e-") == 0) {
              hSecondaryElectrons->Fill(x, y);}
            else if (strcmp(Type, "gamma") == 0) {
              hSecondaryGammas->Fill(x, y);
}           else if (strcmp(Type, "neutron") == 0) {
              hSecondaryNeutrons->Fill(x, y);}
            else {
              hSecondaryOthers->Fill(x, y);
}
     }

// FLUX Plots
/*
    // Create a box representing 14mm x 14mm in the center of the detector (140mm x 140mm)
    TBox *box = new TBox(-7, -7, 7, 7);
    box->SetLineColor(kRed);
    box->SetLineWidth(2);
    box->SetFillStyle(0); // Transparent fill
*/

    std::string title_c3 = title + "_Flux";
    TCanvas *c3 = new TCanvas(title_c3.c_str(), "Hits per pixel", 1800, 600);
    c3->Divide(3, 1);

    // Total Hits
    c3->cd(1);
    gPad->SetLeftMargin(0.09);
    gPad->SetRightMargin(0.11);
    gPad->SetBottomMargin(0.1);
    gPad->SetTopMargin(0.1);
    hTotalHits->Draw("COLZ");
    TPaveText *legend_total = new TPaveText(0.59, 0.8, 0.89, 0.9, "NDC");
    legend_total->SetFillColor(0);
    legend_total->SetBorderSize(1);
    legend_total->SetTextFont(42);
    legend_total->SetTextSize(0.033);
    legend_total->SetTextAlign(12);
    legend_total->AddText(Form("Total Hits: %g", hTotalHits->GetEntries()));
    legend_total->Draw();
    //box->Draw();

    // Primary Particle Hits
    c3->cd(2);
    gPad->SetLeftMargin(0.08);
    gPad->SetRightMargin(0.11);
    gPad->SetBottomMargin(0.1);
    gPad->SetTopMargin(0.1);
    hPrimaryProtons->Draw("COLZ");
    //hPrimaryElectrons->Draw("COLZ");              # Uncomment for electron plots
    TPaveText *legend_primary = new TPaveText(0.48, 0.75, 0.89, 0.9, "NDC");
    legend_primary->SetFillColor(0);
    legend_primary->SetBorderSize(1);
    legend_primary->SetTextFont(42);
    legend_primary->SetTextSize(0.033);
    legend_primary->SetTextAlign(12);
    legend_primary->AddText(Form("Primary Protons: %g", hPrimaryProtons->GetEntries()));
    //legend_primary->AddText(Form("Primary Electrons: %g", hPrimaryElectrons->GetEntries()));      # Uncomment for electron plots
    legend_primary->Draw();
    //box->Draw();

    // Secondary Particle Hits
    c3->cd(3);
    gPad->SetLeftMargin(0.085);
    gPad->SetRightMargin(0.09);
    gPad->SetBottomMargin(0.1);
    gPad->SetTopMargin(0.1);
    hSecondaryProtons->Draw("COLZ");
    hSecondaryElectrons->Draw("SAME");
    hSecondaryGammas->Draw("SAME");
    hSecondaryOthers->Draw("SAME");
    TPaveText *legend_secondary = new TPaveText(0.5, 0.65, 0.91, 0.9, "NDC");
    legend_secondary->SetFillColor(0);
    legend_secondary->SetBorderSize(1);
    legend_secondary->SetTextFont(42);
    legend_secondary->SetTextSize(0.033);
    legend_secondary->SetTextAlign(12);
    legend_secondary->AddText(Form("Secondary Protons: %g", hSecondaryProtons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Electrons: %g", hSecondaryElectrons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Gammas: %g", hSecondaryGammas->GetEntries()));
    legend_secondary->AddText(Form("Secondary Neutrons: %g", hSecondaryNeutrons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Others: %g", hSecondaryOthers->GetEntries()));
    legend_secondary->Draw();
    //box->Draw();


    // NEW CODE: Step 3 - Create a new canvas and draw the Edep vs. LET plot
    std::string title_c4 = title + "_Edep_vs_LET";
    TCanvas *c4 = new TCanvas(title_c4.c_str(), "Edep vs LET", 900, 700);
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.12);

    // Use a logarithmic Z-axis to better visualize low-statistic bins.
    gPad->SetLogz();

    hEdepVsLET->Draw("COLZ"); // "COLZ" draws a 2D histogram with a color map and a color scale bar.

    std::string title_c6 = title + "_Ekin_vs_LET";
    TCanvas *c6 = new TCanvas(title_c6.c_str(), "Ekin vs LET", 900, 700);
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.12);

    // Use a logarithmic Z-axis to better visualize low-statistic bins.
    gPad->SetLogz();

    hEkinVsLET->Draw("COLZ"); // "COLZ" draws a 2D histogram with a color map and a color scale bar.
    hEkinVsLET_prims->SetLineColor(kRed);
    hEkinVsLET_prims->SetLineWidth(2);
    hEkinVsLET_prims->Draw("SAME");
    hEkinVsLET_secs->SetLineColor(kGreen);
    hEkinVsLET_secs->SetLineWidth(2);
    hEkinVsLET_secs->Draw("SAME");

    // Create a canvas
    std::string title_c1 = title + "_Ekin_vs_LET_comparison";
    TCanvas *c1 = new TCanvas(title_c1.c_str(), "Kinetic Energy vs LET comparison", 1800, 600);
    // Divide the canvas into 2 pads, side-by-side
    c1->Divide(3, 1);

    // All particles
    c1->cd(1);
    gPad->SetLogz();
    hEkinVsLET->SetTitle("All Particles");
    hEkinVsLET->Draw("COLZ"); // "COLZ" shows the color map and Z-axis scale

    c1->cd(2);
    gPad->SetLogz();
    hEkinVsLET_prims->SetTitle("Primary Particles");
    hEkinVsLET_prims->Draw("COLZ"); // "COLZ" shows the color map and Z-axis scale

    // Go to the third pad and draw the secondary histogram
    c1->cd(3);
    gPad->SetLogz();
    hEkinVsLET_secs->SetTitle("Secondary Particles");
    hEkinVsLET_secs->Draw("COLZ");

    // Legend
    TPaveText *legend_EkinVsLET = new TPaveText(0.38, 0.65, 0.9, 0.9, "NDC");
    legend_EkinVsLET->SetFillColor(0);
    legend_EkinVsLET->SetBorderSize(1);
    legend_EkinVsLET->SetTextFont(42);
    legend_EkinVsLET->SetTextSize(0.033);
    legend_EkinVsLET->SetTextAlign(12);
    legend_EkinVsLET->AddText(Form("Kinetic energy generator: %.1f MeV", GeneratorEnergy));
    legend_EkinVsLET->AddText(Form("All Particle Hits: %g", hEkinVsLET->GetEntries()));
    legend_EkinVsLET->AddText(Form("Primary Particle Hits: %g", hEkinVsLET_prims->GetEntries()));
    legend_EkinVsLET->AddText(Form("Secondary Particle Hits: %g", hEkinVsLET_secs->GetEntries()));
    legend_EkinVsLET->Draw();




   // Create a canvas
    std::string title_c5 = title + "_Ekin_vs_Edep_comparison";
    TCanvas *c5 = new TCanvas(title_c5.c_str(), "Kinetic Energy vs Edep comparison", 1800, 600);
    // Divide the canvas into 2 pads, side-by-side
    c5->Divide(3, 1);

    // All particles
    c5->cd(1);
    gPad->SetLogz();
    gPad->SetRightMargin(0.11);
    hEkinVsEdep->GetXaxis()->SetNdivisions(505); // Fewer ticks on the X-axis
    hEkinVsEdep->SetTitle("All Particles");
    hEkinVsEdep->Draw("COLZ"); // "COLZ" shows the color map and Z-axis scale

    c5->cd(2);
    gPad->SetLogz();
    gPad->SetRightMargin(0.11);
    hEkinVsEdep_prims->GetXaxis()->SetNdivisions(505); // Fewer ticks on the X-axis
    hEkinVsEdep_prims->SetTitle("Primary Particles");
    hEkinVsEdep_prims->Draw("COLZ"); // "COLZ" shows the color map and Z-axis scale

    // Go to the third pad and draw the secondary histogram
    c5->cd(3);
    gPad->SetLogz();
    gPad->SetRightMargin(0.12);
    hEkinVsEdep_secs->GetXaxis()->SetNdivisions(505); // Fewer ticks on the X-axis
    hEkinVsEdep_secs->SetTitle("Secondary Particles");
    hEkinVsEdep_secs->Draw("COLZ");

    // Legend
    TPaveText *legend_EkinVsEdep = new TPaveText(0.36, 0.65, 0.88, 0.9, "NDC");
    legend_EkinVsEdep->SetFillColor(0);
    legend_EkinVsEdep->SetBorderSize(1);
    legend_EkinVsEdep->SetTextFont(42);
    legend_EkinVsEdep->SetTextSize(0.033);
    legend_EkinVsEdep->SetTextAlign(12);
    legend_EkinVsEdep->AddText(Form("Kinetic energy generator: %.1f MeV", GeneratorEnergy));
    legend_EkinVsEdep->AddText(Form("All Particle Hits: %g", hEkinVsEdep->GetEntries()));
    legend_EkinVsEdep->AddText(Form("Primary Particle Hits: %g", hEkinVsEdep_prims->GetEntries()));
    legend_EkinVsEdep->AddText(Form("Secondary Particle Hits: %g", hEkinVsEdep_secs->GetEntries()));
    legend_EkinVsEdep->Draw();


    // The new TProfile will have the same X-axis binning as your TH2D.
    TProfile* prof_EdepVsEkin = hEkinVsEdep->ProfileX("prof_all");
    TProfile* prof_EdepVsEkin_prims = hEkinVsEdep_prims->ProfileX("prof_prim");
    TProfile* prof_EdepVsEkin_secs = hEkinVsEdep_secs->ProfileX("prof_sec");

    prof_EdepVsEkin_prims->SetLineColor(kBlue);  // Solid blue line for primaries
    prof_EdepVsEkin_prims->SetLineWidth(2);
    prof_EdepVsEkin_secs->SetLineColor(kRed);  // Solid blue line for primaries
    prof_EdepVsEkin_secs->SetLineWidth(2);


    // 3. Draw the profile
    TCanvas* c_profile = new TCanvas("c_profile", "Profile Canvas", 800, 600);

    c_profile->Divide(3, 1);

    // All particles
    c_profile->cd(1);
    gPad->SetLogz();
    gPad->SetRightMargin(0.11);
    prof_EdepVsEkin->SetTitle("All Particles");
    prof_EdepVsEkin->GetXaxis()->SetTitle("Kinetic Energy (Ekin) [MeV]");
    prof_EdepVsEkin->GetYaxis()->SetTitle("Average Deposited Energy (Edep) [keV]");
    prof_EdepVsEkin->Draw(); // Draw the plot

    c_profile->cd(2);
    gPad->SetLogz();
    gPad->SetRightMargin(0.11);
    prof_EdepVsEkin_prims->GetXaxis()->SetNdivisions(505); // Fewer ticks on the X-axis
    prof_EdepVsEkin_prims->SetTitle("Primary Particles");
    prof_EdepVsEkin_prims->Draw();

    // Go to the third pad and draw the secondary histogram
    c_profile->cd(3);
    gPad->SetLogz();
    gPad->SetRightMargin(0.12);
    prof_EdepVsEkin_secs->GetXaxis()->SetNdivisions(505); // Fewer ticks on the X-axis
    prof_EdepVsEkin_secs->SetTitle("Secondary Particles");
    prof_EdepVsEkin_secs->Draw();

    // Legend
    TPaveText *legend_prof_EkinVsEdep = new TPaveText(0.36, 0.65, 0.88, 0.9, "NDC");
    legend_prof_EkinVsEdep->SetFillColor(0);
    legend_prof_EkinVsEdep->SetBorderSize(1);
    legend_prof_EkinVsEdep->SetTextFont(42);
    legend_prof_EkinVsEdep->SetTextSize(0.033);
    legend_prof_EkinVsEdep->SetTextAlign(12);
    legend_prof_EkinVsEdep->AddText(Form("Kinetic energy generator: %.1f MeV", GeneratorEnergy));
    legend_prof_EkinVsEdep->AddText(Form("All Particle Hits: %g", hEkinVsEdep->GetEntries()));
    legend_prof_EkinVsEdep->AddText(Form("Primary Particle Hits: %g", hEkinVsEdep_prims->GetEntries()));
    legend_prof_EkinVsEdep->AddText(Form("Secondary Particle Hits: %g", hEkinVsEdep_secs->GetEntries()));
    legend_prof_EkinVsEdep->Draw();

    // Optional: To make it look like a continuous line instead of data points with errors
    // prof_EdepVsEkin->Draw("HIST"); 

}