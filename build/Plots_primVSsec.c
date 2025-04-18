{
    std::string title = "500kProton50MeV_Al_Slab_5mm_Vacuum_fStopandKill_Omnidirectional"; // Define your title here
    std::string filename = title + ".root";      // Use the title in the filename

    float GeneratedParticles = 10000;
    float GeneratorEnergy = 100;
    const char* GeneratedParticleType = "Proton";
    const char* WorldVolume = "G4_Air";
    const char* Shielding = "-";

    TFile *file = TFile::Open(filename.c_str());
    TTree *tree = (TTree*)file->Get("DetectorData");
    
    gStyle->SetOptStat(0); // Disable all statistics
    //gStyle->SetOptStat("e"); // Enable displaying only the number of entries ("n" option)

    Double_t x, y, z, Ek, Edep, LET;
    Int_t events, ParentID;
    Char_t Type;

    tree->SetBranchAddress("fEvent", &events);
    tree->SetBranchAddress("fX", &x);
    tree->SetBranchAddress("fY", &y);
    tree->SetBranchAddress("fZ", &z);
    tree->SetBranchAddress("KineticEnergy", &Ek);
    tree->SetBranchAddress("DepositedEnergy", &Edep);
    tree->SetBranchAddress("LET", &LET);
    tree->SetBranchAddress("ParticleType", &Type);
    tree->SetBranchAddress("ParentID", &ParentID);

        // Manual for loop for variable bin edges
    const int nBins = 280;
    double binEdgesManual[nBins + 1];
    double BinEdge = 0.001;
    for (int i = 0; i <= nBins; ++i) {
        binEdgesManual[i] = BinEdge;
        BinEdge = BinEdge + BinEdge * 0.05;
    }

        // Variables to find the min and max values
    Double_t minEk = std::numeric_limits<Double_t>::max();
    Double_t maxEk = -std::numeric_limits<Double_t>::max();
    Double_t minEdep = std::numeric_limits<Double_t>::max();
    Double_t maxEdep = -std::numeric_limits<Double_t>::max();
    Double_t minLET = std::numeric_limits<Double_t>::max();
    Double_t maxLET = -std::numeric_limits<Double_t>::max();

    // Loop to find min and maxa values
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        //Ek *= 1000; // Convert MeV to keV
        //Edep *= 1000; // Convert MeV to keV
        if (Ek < minEk) minEk = Ek;
        if (Ek > maxEk) maxEk = Ek;
        if (minEdep <= 0) minEdep = 1e-3;
        if (Edep > maxEdep) maxEdep = Edep;
        if (LET < minLET) minLET = LET;
        if (LET > maxLET) maxLET = LET;
    }

    // Function to calculate bin edges for relative binning
    auto calculateBinEdges = [](Double_t min, Double_t max, Int_t numBins) {
        std::vector<Double_t> binEdges;
        Double_t logMin = log10(min);
        Double_t logMax = log10(max);
        Double_t binWidth = (logMax - logMin) / numBins;

        for (int i = 0; i <= numBins; ++i) {
            binEdges.push_back(pow(10, logMin + i * binWidth));
        }
        return binEdges;
    };

    // Calculate bin edges
    std::vector<Double_t> binEdgesEk = calculateBinEdges(minEk, maxEk, 100);
    std::vector<Double_t> binEdgesEdep = calculateBinEdges(1, 4e3, 110);   //Self chosen because it compares better with TPX data
    std::vector<Double_t> binEdgesLET = calculateBinEdges(minLET, maxLET, 100);

    // Convert vector to array
    Double_t* binsEk = &binEdgesEk[0];
    Double_t* binsEdep = &binEdgesEdep[0];
    Double_t* binsLET = &binEdgesLET[0];

    // Bins for flux plots
    int bins = 140;
    int min = -7;
    int max = 7;
   



    // Define histograms separately for primary and secondary particles
    TH1D *h1_total = new TH1D("KineticEnergy_Total", "Total Kinetic Energy; Kinetic Energy [MeV]; Count [#]", binEdgesEk.size() - 1, 1e-3, maxEk+maxEk/20);
    TH1D *h1_primary = new TH1D("KineticEnergy_Primary", "Primary Kinetic Energy; Kinetic Energy [MeV]; Count [#]", binEdgesEk.size() - 1, 1e-3, maxEk+maxEk/20);
    TH1D *h1_secondary = new TH1D("KineticEnergy_Secondary", "Secondary Kinetic Energy; Kinetic Energy [MeV]; Count [#]", binEdgesEk.size() - 1, 1e-3, maxEk+maxEk/20);

    TH1D *h2_total = new TH1D("DepositedEnergy_Total", "Total Deposited Energy; Deposited Energy [keV]; Count [#]", binEdgesEdep.size() - 1, binEdgesEdep.data());
    TH1D *h2_primary = new TH1D("DepositedEnergy_Primary", "Deposited Energy at detector", binEdgesEdep.size() - 1, binEdgesEdep.data());
    TH1D *h2_secondary = new TH1D("DepositedEnergy_Secondary", "Secondary Deposited Energy; Deposited Energy [keV]; Count [#]", binEdgesEdep.size() - 1, binEdgesEdep.data());

    TH1D *hLET_total = new TH1D("LET_Total", "LET of All Particles; LET [keV/um]; Count [#]", nBins, binEdgesManual);
    TH1D *hLET_primary_linear = new TH1D("LET_Primary", "LET in detector (linear scale); LET [keV/um]; Count [#]", nBins, binEdgesManual);
    TH1D *hLET_primary_log = new TH1D("LET_Primary", "LET in detector (log scale); LET [keV/um]; Count [#]", nBins, binEdgesManual);
    TH1D *hLET_secondary = new TH1D("LET_Secondary", "LET of Secondary Particles; LET [keV/um]; Count [#]", nBins, binEdgesManual);

    TH2F *hTotalHits = new TH2F("hTotalHits", "Total Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hPrimaryProtons = new TH2F("hPrimaryProtons", "Hits of primary particles;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryProtons = new TH2F("hSecondaryProtons", "Hits of secondary particles;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hPrimaryElectrons = new TH2F("hPrimaryElectrons", "Hits of primary electrons;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryElectrons = new TH2F("hSecondaryElectrons", "Hits of secondary electrons;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryGammas = new TH2F("hSecondaryGammas", "Hits of secondary gammas;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaryOthers = new TH2F("hSecondaryOthers", "Hits of secondary others;X (mm); Y(mm)", bins, min, max, bins, min, max);

    // Loop over all entries in the tree to fill the histograms
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        
        h1_total->Fill(Ek);
        h2_total->Fill(Edep*1000); // *1000 to get in keV 
        hLET_total->Fill(LET);
        hTotalHits->Fill(x, y);

        // Primary Particles (ParentID == 0)
        if (ParentID == 0) {
            h1_primary->Fill(Ek);
            h2_primary->Fill(Edep*1000); // *1000 to get in keV
            hLET_primary_linear->Fill(LET);
            hLET_primary_log->Fill(LET);
            if (Type == 'p'){
                hPrimaryProtons->Fill(x, y);}
            else if (Type == 'e'){
                hPrimaryElectrons->Fill(x, y);}
        } 
        // Secondary Particles (ParentID > 0)
        else {
            h1_secondary->Fill(Ek);
            h2_secondary->Fill(Edep*1000); // *1000 to get in keV
            hLET_secondary->Fill(LET);
            if (Type == 'p'){
                hSecondaryProtons->Fill(x, y);}
            else if (Type == 'e'){
                hSecondaryElectrons->Fill(x, y);}
            else if (Type == 'g'){
                hSecondaryGammas->Fill(x, y);}
            else {
                hSecondaryOthers->Fill(x, y);}
        }
    }

    // Set colors and styles
    h1_primary->SetLineColor(kBlue);  // Solid blue line for primaries
    h1_primary->SetLineWidth(2);
    h1_secondary->SetLineColor(kRed);  // Dashed red line for secondaries
    h1_secondary->SetLineWidth(2);
    h1_secondary->SetLineStyle(2);  // Dashed line

    h2_total->SetLineColor(kBlack);
    h2_total->SetLineWidth(2);
    h2_primary->SetLineColor(kBlue);
    h2_primary->SetLineWidth(2);
    h2_secondary->SetLineColor(kRed);
    h2_secondary->SetLineWidth(2);
    h2_secondary->SetLineStyle(2);  // Dashed line

    hLET_primary_linear->SetLineColor(kBlue);
    hLET_primary_linear->SetLineWidth(3);
    hLET_primary_log->SetLineColor(kBlue);
    hLET_primary_log->SetLineWidth(3);
    hLET_secondary->SetLineColor(kRed);
    hLET_secondary->SetLineWidth(4);
    hLET_secondary->SetLineStyle(2);  // Dashed line

    // Create canvas for Kinetic Energy
    std::string title_Ekin = title + "_KineticEnergy";
    TCanvas *c1 = new TCanvas(title_Ekin.c_str(), "Kinetic Energy Spectrum", 800, 800);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.08);
    h1_primary->SetTitle("Kinetic Energy Spectrum at Detector; Kinetic Energy [MeV]; Count [#]");
    h1_primary->Draw();  // Draw primaries first
    h1_secondary->Draw("SAME");  // Overlay secondaries

    double mean_Ekin_all = h1_total->GetMean();
    double mean_Ekin_primary = h1_primary->GetMean();
    double mean_Ekin_secondary = h1_secondary->GetMean();

    // Create legend
    TLegend *legend = new TLegend(0.18, 0.45, 0.8, 0.9);
    legend->SetTextSize(0.025);
    legend->SetMargin(0.08);
    legend->AddEntry(h1_primary, "Primary Particles", "l");
    legend->AddEntry(h1_secondary, "Secondary Particles", "l");
    legend->AddEntry((TObject*)0, Form("Generated particles = %.0f", GeneratedParticles), "");
    legend->AddEntry((TObject*)0, Form("Generated particle type = %s", GeneratedParticleType), "");
    legend->AddEntry((TObject*)0, Form("Energy @ generator = %.1f MeV", GeneratorEnergy), "");
    legend->AddEntry((TObject*)0, Form("Simulation Environment = %s", WorldVolume), "");
    legend->AddEntry((TObject*)0, Form("Shielding? = %s", Shielding), "");
    legend->AddEntry((TObject*)0, Form("Mean energy @ detector = %.2f MeV", mean_Ekin_all), "");    
    legend->AddEntry((TObject*)0, Form("Mean energy primaries @ detector = %.2f MeV", mean_Ekin_primary), ""); // "l" is for line style
    legend->AddEntry((TObject*)0, Form("Mean energy secondaries @ detector = %.3f MeV", mean_Ekin_secondary), "");
    legend->AddEntry((TObject*)0, Form("Hits = %.2lld ", nentries), "");
    legend->Draw();



    // Create canvas for Deposited Energy

    std::string title_Edep = title + "_DepositedEnergy";
    TCanvas *c2 = new TCanvas(title_Edep.c_str(), "Deposited Energy Spectrum", 1200, 800);

    // Define two pads without whitespace
    TPad *pad1 = new TPad("pad1", "Top pad", 0, 0.5, 1, 1);
    TPad *pad2 = new TPad("pad2", "Bottom pad", 0, 0, 1, 0.5);

    // Remove spacing
    pad1->SetBottomMargin(0.02); // No X-axis labels
    pad2->SetTopMargin(0.02);    // No gap

    // Set margins
    pad1->SetLeftMargin(0.12);
    pad1->SetRightMargin(0.05);
    pad2->SetLeftMargin(0.12);
    pad2->SetRightMargin(0.05);
    pad2->SetBottomMargin(0.15);

    pad1->Draw();
    pad2->Draw();

    double mean_Edep_all = h2_total->GetMean();
    double mean_Edep_primary = h2_primary->GetMean();
    double mean_Edep_secondary = h2_secondary->GetMean();
    int maxBin = h2_total->GetMaximumBin();
    double peakValue = h2_total->GetXaxis()->GetBinCenter(maxBin);


    /// ----- TOP PLOT -----
    pad1->cd();
    gPad->SetLogx();
    h2_total->Draw();
    h2_total->GetXaxis()->SetLabelSize(0); // Remove x-axis labels
    h2_total->SetTitle("");
    TPaveText *titleText = new TPaveText(0.15, 0.92, 0.85, 0.98, "NDC");
    titleText->AddText("Deposited Energy Spectrum at Detector");
    titleText->SetFillColor(0);
    titleText->SetBorderSize(0);
    titleText->SetTextFont(42);
    titleText->SetTextSize(0.08);
    titleText->Draw();
    // Create legend
    TLegend *legend_Edep = new TLegend(0.6, 0.6, 0.95, 0.9);
    legend_Edep->SetTextSize(0.06);
    legend_Edep->SetMargin(0.08);
    //legend_Edep->AddEntry(h2_total, "All Particles", "l");
    //legend_Edep->AddEntry(h2_primary, "Primary Particles", "l");
    //legend_Edep->AddEntry(h2_secondary, "Secondary Particles", "l");
    //legend_Edep->AddEntry((TObject*)0, Form("Generated particles = %.0f", GeneratedParticles), "");
    //legend_Edep->AddEntry((TObject*)0, Form("Generated particle type = %s", GeneratedParticleType), "");
    //legend_Edep->AddEntry((TObject*)0, Form("Energy @ generator = %.1f MeV", GeneratorEnergy), "");
    //legend_Edep->AddEntry((TObject*)0, Form("Simulation Environment = %s", WorldVolume), "");
    //legend_Edep->AddEntry((TObject*)0, Form("Shielding? = %s", Shielding), "");
    legend_Edep->AddEntry((TObject*)0, Form("Mean Energy @ detector = %.2f keV", mean_Edep_all), "");    
    //legend_Edep->AddEntry((TObject*)0, Form("Mean primaries @ detector = %.2f keV", mean_Edep_primary), ""); // "l" is for line style
    //legend_Edep->AddEntry((TObject*)0, Form("Mean secondaries @ detector = %.2f keV", mean_Edep_secondary), "");
    legend_Edep->AddEntry((TObject*)0, Form("Hits in detector = %.2lld ", nentries), "");
    legend_Edep->AddEntry((TObject*)0, Form("x(y_max) = %.2f keV", peakValue), "");
    legend_Edep->AddEntry((TObject*)0, "Logarithmic binning: 110 bins", "");
    legend_Edep->Draw();


    /// ----- BOTTOM PLOT -----
    pad2->cd();
    gPad->SetLogx();
    gPad->SetLogy();
    h2_total->SetTitle(""); // Remove bottom plot title
    h2_total->Draw();
    h2_total->GetXaxis()->SetTitle("Deposited Energy [keV]");
    h2_total->GetXaxis()->SetTitleSize(0.07);   // Increase X label size
    h2_total->GetXaxis()->SetLabelSize(0.06);   // Increase X tick label size
    h2_total->GetYaxis()->SetTitle("N [-]");
    h2_total->GetYaxis()->SetTitleSize(0.07);   // Increase X label size
    h2_total->GetYaxis()->SetLabelSize(0.06);   // Increase X tick label size

    c2->Update();



    // Create canvas for LET
    std::string title_LET = title + "_LET";
    TCanvas *c4 = new TCanvas(title_LET.c_str(), "LET Histogram", 1400, 900);
    c4->Divide(1, 2);

    double mean_LET_total = hLET_total->GetMean();
    double mean_LET_primary = hLET_primary_linear->GetMean();
    double mean_LET_secondary = hLET_secondary->GetMean();

    // Top plot (Linear scale)
    c4->cd(1);
    gPad->SetLogx();
    gPad->SetLeftMargin(0.07);   // Default is around 0.1, reduce for less left space
    gPad->SetRightMargin(0.02);  // Default is around 0.1, reduce for less right space
    //gPad->SetBottomMargin(0.09);    // Default is around 0.1, reduce for less top space

    hLET_primary_linear->Draw();
    hLET_secondary->Draw("SAME");
    hLET_primary_linear->GetXaxis()->SetTitleSize(0.05);  // Increase title size
    hLET_primary_linear->GetXaxis()->SetLabelSize(0.04);  // Increase number size
    hLET_primary_linear->GetXaxis()->SetTitleOffset(1.); // Increase offset for more space
    hLET_primary_linear->GetYaxis()->SetTitleSize(0.05);  // Increase title size
    hLET_primary_linear->GetYaxis()->SetLabelSize(0.04);  // Increase number size
    hLET_primary_linear->GetYaxis()->SetTitleOffset(0.7); // 
    hLET_primary_log->GetXaxis()->SetTitleSize(0.05);  // Increase title size
    hLET_primary_log->GetXaxis()->SetLabelSize(0.04);  // Increase number size
    hLET_primary_log->GetXaxis()->SetTitleOffset(1.); // Increase offset for more space
    hLET_primary_log->GetYaxis()->SetTitleSize(0.05);  // Increase title size
    hLET_primary_log->GetYaxis()->SetLabelSize(0.04);  // Increase number size
    hLET_primary_log->GetYaxis()->SetTitleOffset(0.7); // 


    TLegend *legend_LET = new TLegend(0.65, 0.5, 0.98, 0.9);
    legend_LET->AddEntry((TObject*)0, Form("Mean LET all particles @ detector = %.2f keV/um", mean_LET_total), "");
    legend_LET->AddEntry(hLET_primary_linear, "Primary Particles", "l");
    legend_LET->AddEntry((TObject*)gPad, Form("Mean LET primaries @ detector = %.2f keV/um", mean_LET_primary), "");
    legend_LET->AddEntry(hLET_secondary, "Secondary Particles", "l");
    legend_LET->AddEntry((TObject*)0, Form("Mean LET secondaries @ detector = %.2f keV/um", mean_LET_secondary), "");
    legend_LET->SetTextAlign(12);   // Align text to the left
    legend_LET->SetMargin(0.15);    // Reduce left margin so text moves left

    legend_LET->SetTextSize(0.04);  //standard is 0.03
    legend_LET->Draw();

    // Bottom plot (Log scale)
    c4->cd(2);
    gPad->SetLogy(1);
    gPad->SetLogx();
    gPad->SetLeftMargin(0.07);   // Default is around 0.15, reduce for less left space
    gPad->SetRightMargin(0.02);  // Default is around 0.1, reduce for less right space
    gPad->SetTopMargin(0.08);    // Default is around 0.1, reduce for less top space
    hLET_primary_log->Draw();
    hLET_secondary->Draw("SAME");




    // FLUX Plots

    // Create a box representing 14mm x 14mm in the center of the detector (140mm x 140mm)
    TBox *box = new TBox(-7, -7, 7, 7); 
    box->SetLineColor(kRed);
    box->SetLineWidth(2);
    box->SetFillStyle(0); // Transparent fill

        
    std::string title_c3 = title + "_Flux";
    TCanvas *c3 = new TCanvas(title_c3.c_str(), "Hits per pixel", 1800, 600);
    c3->Divide(3, 1);
    
    // Total Hits
    c3->cd(1);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.09);
    gPad->SetBottomMargin(0.1);
    gPad->SetTopMargin(0.08);
    hTotalHits->Draw("COLZ");
    TPaveText *legend_total = new TPaveText(0.59, 0.8, 0.91, 0.92, "NDC"); // Using TPaveText instead of Tlegend because of whitespace
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
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.09);
    gPad->SetBottomMargin(0.1);
    gPad->SetTopMargin(0.08);
    hPrimaryProtons->Draw("COLZ");
    hPrimaryElectrons->Draw("SAME");
    TPaveText *legend_primary = new TPaveText(0.48, 0.75, 0.91, 0.92, "NDC"); // Using TPaveText instead of Tlegend because of whitespace
    legend_primary->SetFillColor(0);  // Transparent background
    legend_primary->SetBorderSize(1); // Thin black border
    legend_primary->SetTextFont(42);  // ROOT's default font
    legend_primary->SetTextSize(0.033);
    legend_primary->SetTextAlign(12); // Left-aligned text

    legend_primary->AddText(Form("Primary Protons: %g", hPrimaryProtons->GetEntries()));
    legend_primary->AddText(Form("Primary Electrons: %g", hPrimaryElectrons->GetEntries()));
    legend_primary->Draw();
    //box->Draw();
    
    // Secondary Particle Hits
    c3->cd(3);
    gPad->SetLeftMargin(0.11);
    gPad->SetRightMargin(0.09);
    gPad->SetBottomMargin(0.1);
    gPad->SetTopMargin(0.08);
    hSecondaryProtons->Draw("COLZ");
    hSecondaryElectrons->Draw("SAME");
    hSecondaryGammas->Draw("SAME");
    hSecondaryOthers->Draw("SAME");
    TPaveText *legend_secondary = new TPaveText(0.5, 0.65, 0.91, 0.92, "NDC"); // Using TPaveText instead of Tlegend because of whitespace
    legend_secondary->SetFillColor(0);  // Transparent background
    legend_secondary->SetBorderSize(1); // Thin black border
    legend_secondary->SetTextFont(42);  // ROOT's default font
    legend_secondary->SetTextSize(0.033);
    legend_secondary->SetTextAlign(12); // Left-aligned text

    legend_secondary->AddText(Form("Secondary Protons: %g", hSecondaryProtons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Electrons: %g", hSecondaryElectrons->GetEntries()));
    legend_secondary->AddText(Form("Secondary Gammas: %g", hSecondaryGammas->GetEntries()));
    legend_secondary->AddText(Form("Secondary Others: %g", hSecondaryOthers->GetEntries()));

    // Draw the text box
    legend_secondary->Draw();    
    //box->Draw();






}