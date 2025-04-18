{
    std::string title = "100kProton2000MeV_Al_Slab_5mm_Vacuum_fStopandKill_afterSlab_V4"; // Define your title here
    std::string filename = title + ".root";      // Use the title in the filename

    float GeneratedParticles = 100000;
    float GeneratorEnergy = 200;
    const char* GeneratedParticleType = "Proton";
    const char* WorldVolume = "G4_Air";
    const char* Shielding = "Aluminium 5.0mm";

    TFile *file = TFile::Open(filename.c_str());
    TTree *tree = (TTree*)file->Get("DetectorData");

    Long64_t nentries = tree->GetEntries();
    
    gStyle->SetOptStat(0); // Disable all statistics
    //gStyle->SetOptStat("e"); // Enable displaying only the number of entries ("n" option)

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

    // Loop over all entries in the tree to fill the histograms
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        hTotalHits->Fill(x, y);

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
        else {
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


    } }

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
    TPaveText *legend_total = new TPaveText(0.59, 0.8, 0.89, 0.9, "NDC"); // Using TPaveText instead of Tlegend because of whitespace
    legend_total->SetFillColor(0);  // Transparent background
    legend_total->SetBorderSize(1); // Thin black border
    legend_total->SetTextFont(42);  // ROOT's default font
    legend_total->SetTextSize(0.033);
    legend_total->SetTextAlign(12); // Left-aligned text

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
    //hPrimaryElectrons->Draw("COLZ");
    //hPrimaryGammas->Draw("COLZ");
    TPaveText *legend_primary = new TPaveText(0.48, 0.75, 0.89, 0.9, "NDC"); // Using TPaveText instead of Tlegend because of whitespace
    legend_primary->SetFillColor(0);  // Transparent background
    legend_primary->SetBorderSize(1); // Thin black border
    legend_primary->SetTextFont(42);  // ROOT's default font
    legend_primary->SetTextSize(0.033);
    legend_primary->SetTextAlign(12); // Left-aligned text

    legend_primary->AddText(Form("Primary Protons: %g", hPrimaryProtons->GetEntries()));
    //legend_primary->AddText(Form("Primary Electrons: %g", hPrimaryElectrons->GetEntries()));
    //legend_primary->AddText(Form("Primary Gammas: %g", hPrimaryGammas->GetEntries()));
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
    TPaveText *legend_secondary = new TPaveText(0.5, 0.65, 0.91, 0.9, "NDC"); // Using TPaveText instead of Tlegend because of whitespace
    legend_secondary->SetFillColor(0);  // Transparent background
    legend_secondary->SetBorderSize(1); // Thin black border
    legend_secondary->SetTextFont(42);  // ROOT's default font
    legend_secondary->SetTextSize(0.033);
    legend_secondary->SetTextAlign(12); // Left-aligned text

    legend_secondary->AddText(Form("Secondary Protons: %g", hSecondaryProtons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Electrons: %g", hSecondaryElectrons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Gammas: %g", hSecondaryGammas->GetEntries()));
    legend_secondary->AddText(Form("Secondary Neutrons: %g", hSecondaryNeutrons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Others: %g", hSecondaryOthers->GetEntries()));

    // Draw the text box
    legend_secondary->Draw();    
    //box->Draw();

}