{
    std::string title = "10kProton50MeV_Al_Slab_5mm_Vacuum_fStopandKill"; // Define your title here
    std::string filename = title + ".root";      // Use the title in the filename

    TFile *file = TFile::Open(filename.c_str());
    TTree *tree = (TTree*)file->Get("DetectorData");
    
    gStyle->SetOptStat(0); // Disable all statistics
    gStyle->SetOptStat("e"); // Enable displaying only the number of entries ("n" option)

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

    int bins = 14;
    int min = -7;
    int max = 7;
    
    Long64_t nentries = tree->GetEntries();

    // Variables to find the min and max values
    Double_t minEk = std::numeric_limits<Double_t>::max();
    Double_t maxEk = -std::numeric_limits<Double_t>::max();
    Double_t minEdep = std::numeric_limits<Double_t>::max();
    Double_t maxEdep = -std::numeric_limits<Double_t>::max();
    Double_t minLET = std::numeric_limits<Double_t>::max();
    Double_t maxLET = -std::numeric_limits<Double_t>::max();

    // Loop to find min and maxa values
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        //Ek *= 1000; // Convert MeV to keV
        //Edep *= 1000; // Convert MeV to keV
        if (Ek < minEk) minEk = Ek;
        if (Ek > maxEk) maxEk = Ek;
        if (Edep < minEdep) minEdep = Edep;
        if (Edep > maxEdep) maxEdep = Edep;
        if (LET < minLET) minLET = LET;
        if (LET > maxLET) maxLET = LET;
    }
    
    // Manual for loop for variable bin edges
    const int nBins = 28;
    double binEdgesManual[nBins + 1];
    double BinEdge = 0.001;
    for (int i = 0; i <= nBins; ++i) {
        binEdgesManual[i] = BinEdge;
        BinEdge = BinEdge + BinEdge * 0.05;
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
    std::vector<Double_t> binEdgesEdep = calculateBinEdges(minEdep, maxEdep, 100);
    std::vector<Double_t> binEdgesLET = calculateBinEdges(minLET, maxLET, 100);

    // Convert vector to array
    Double_t* binsEk = &binEdgesEk[0];
    Double_t* binsEdep = &binEdgesEdep[0];
    Double_t* binsLET = &binEdgesLET[0];
    

    // For Kinetic Energy
    TH1D *h1 = new TH1D("KineticEnergy", "...", binEdgesEk.size() - 1, 1, maxEk+maxEk/20);
    // For Deposited Energy
    TH1D *h2 = new TH1D("DepositedEnergy", "", binEdgesEdep.size() - 1, 1, maxEdep+maxEdep/20);
    // For LET
    TH1D *h3 = new TH1D("LET", "", binEdgesLET.size() - 1, minLET, maxLET);
    
    
    
    /*
    // For Kinetic Energy
    TH1D *h1 = new TH1D("KineticEnergy", "...", 80, 1e-3, maxEk+maxEk/20);
    // For Deposited Energy
    TH1D *h2 = new TH1D("DepositedEnergy", "", 80, 1e-3, maxEdep+maxEdep/20);
    // For LET
    TH1D *h3 = new TH1D("LET", "", 80, minLET, maxLET);
    */

    // Define histograms for hits on detector
    TH2F *hTotalHits = new TH2F("hTotalHits", "Total Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hProtons = new TH2F("hProtons", "Proton Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hElectrons = new TH2F("hElectrons", "Electron Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hGammas = new TH2F("hGammas", "Gamma Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hXrays = new TH2F("hXrays", "X-ray Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hNeutrons = new TH2F("hNeutrons", "Neutron Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hOtherParticles = new TH2F("hOtherParticles", "e+g+X Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hPrimaries = new TH2F("hPrimaries", "Hits of primaries;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaries = new TH2F("hSecondaries", "Hits of secondaries;X (mm); Y(mm)", bins, min, max, bins, min, max);


    // Define histograms for kinetic and deposited energy for protons and electrons
    TH1F *hKineticEnergyProtons = new TH1F("hKineticEnergyProtons", "Kinetic Energy;Kinetic Energy (MeV);Counts", binEdgesEk.size() - 1, 0.001, maxEk+maxEk/20);
    TH1F *hDepositedEnergyProtons = new TH1F("hDepositedEnergyProtons", "Deposited Energy;Deposited Energy (MeV);Counts", binEdgesEdep.size() - 1, 0.001, maxEdep+maxEdep/20);
    TH1F *hKineticEnergyElectrons = new TH1F("hKineticEnergyElectrons", "Kinetic Energy of Electrons;Kinetic Energy (MeV);Counts", binEdgesEk.size() - 1, 0.001, maxEk+maxEk/20);
    TH1F *hDepositedEnergyElectrons = new TH1F("hDepositedEnergyElectrons", "Deposited Energy by Electrons;Deposited Energy (MeV);Counts", binEdgesEdep.size() - 1, 0.001, maxEdep+maxEdep/20);

    
    
    
    TH1D *hLET = new TH1D("LET", "", nBins, binEdgesManual);

    // Loop over all entries in the tree to fill the histogram
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);
        h1->Fill(Ek);
        h2->Fill(Edep);
        h3->Fill(LET);
        hLET->Fill(LET);

        // Fill the total hits histogram
        hTotalHits->Fill(x, y);

        // Fill the histograms based on particle type
        if (Type == 'p') {
            hProtons->Fill(x, y);
            hKineticEnergyProtons->Fill(Ek);
            hDepositedEnergyProtons->Fill(Edep);
        } 
        else if (Type == 'e') {
            hElectrons->Fill(x, y);
            hOtherParticles->Fill(x, y);
            hKineticEnergyElectrons->Fill(Ek);
            hDepositedEnergyElectrons->Fill(Edep);
        } 
        else if (Type == 'g') {
            hGammas->Fill(x, y);
            hOtherParticles->Fill(x, y);
        } 
        else if (Type == 'x') { // assuming "x-ray" is used for X-rays
            hXrays->Fill(x, y);
            hOtherParticles->Fill(x, y);
        }
        
        if (ParentID == 0) {
            hPrimaries->Fill(x, y);
        }
        else if (ParentID > 0) {
            hSecondaries->Fill(x, y); 
        }
    }   

    // Draw the histograms with square aspect ratio
    std::string title_c1 = title + "_Energies";
    TCanvas *c1 = new TCanvas(title_c1.c_str(), title_c1.c_str(), 800, 800);
    //c1->Divide(3,1);
    
    //c1->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    h1->SetStats(1);
    gStyle->SetOptStat(110);
    h1->SetTitle("Kinetic Energy Spectrum at Detector; Kinetic Energy [MeV]; Count [#]");
    h1->Draw();
    /*
    c1->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetLogy();
    h2->SetStats(0);
    h2->SetTitle("Deposited Energy Spectrum; Deposited Energy [MeV]; Count [#]");
    h2->Draw();
    
    c1->cd(3);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetLogy();
    h3->SetStats(0);
    h3->SetTitle("LET in Si 500um detector; LET [keV/um]; Count [#]");
    h3->Draw();
    */
    // Create a box representing 14mm x 14mm in the center of the detector (140mm x 140mm)
    TBox *box = new TBox(-7, -7, 7, 7); 
    box->SetLineColor(kRed);
    box->SetLineWidth(2);
    box->SetFillStyle(0); // Transparent fill

    std::string title_c2 = title + "_Flux";
    TCanvas *c2 = new TCanvas(title_c2.c_str(), "Hits per pixel", 1800, 600);
    c2->Divide(3, 1);
    
    // Total Hits
    c2->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.15);
    hTotalHits->Draw("COLZ");
    box->Draw();

    // Proton Hits
    c2->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.15);
    hProtons->Draw("COLZ");
    box->Draw();

    // Create a legend to display entries for gamma, electron, and X-ray hits
    TLegend *legendEntries = new TLegend(0.2, 0.7, 0.3, 0.82);
    //legendEntries->SetHeader("Entries");
    legendEntries->AddEntry(hElectrons, Form("Electrons: %g", hElectrons->GetEntries()), "f");
    legendEntries->AddEntry(hGammas, Form("Gammas: %g", hGammas->GetEntries()), "f");
    legendEntries->AddEntry(hXrays, Form("X-rays: %g", hXrays->GetEntries()), "f");
    legendEntries->SetBorderSize(0);
    legendEntries->SetTextSize(0.03);
    // Other Hits
    c2->cd(3);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.15);
    hOtherParticles->Draw("COLZ");
    box->Draw();

    // Legend for the third plot


    legendEntries->Draw();
    /*TLegend *legend = new TLegend(0.1, 0.7, 0.3, 0.9);
    legend->AddEntry(hElectrons, "Electrons", "f");
    legend->AddEntry(hGammas, "Gammas", "f");
    legend->AddEntry(hXrays, "X-rays", "f");
    hElectrons->SetMarkerColor(kBlue);
    hGammas->SetMarkerColor(kGreen);
    hXrays->SetMarkerColor(kMagenta);
    legend->Draw();*/
    // Overlay the histograms for the other particles
  /*  c2->cd(3);
    hElectrons->Draw("COLZ SAME");
    hGammas->Draw("COLZ SAME");
    hXrays->Draw("COLZ SAME");
    legend->Draw();*/
    
    
    std::string title_c3 = title + "_Primaries";
    TCanvas *c3 = new TCanvas(title_c3.c_str(), "Hits per pixel", 1800, 600);
    c3->Divide(3, 1);
    
    // Total Hits
    c3->cd(1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.15);
    hTotalHits->Draw("COLZ");
    box->Draw();

    // Primary Particle Hits
    c3->cd(2);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.15);
    hPrimaries->Draw("COLZ");
    box->Draw();
    
    // Secondary Particle Hits
    c3->cd(3);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.15);
    hSecondaries->Draw("COLZ");
    box->Draw();
    
    std::string title_c4 = title + "LET";
    TCanvas *c4 = new TCanvas(title_c4.c_str(), "LET", 1800, 900);
    c4->Divide(1, 2);
    
    // Top plot with linear y-axis
    c4->cd(1);
    gPad->SetLogy(0);
    gPad->SetLogx();
    hLET->Draw();
    hLET->GetXaxis()->SetRangeUser(0.1, 200);  // Set x-axis range
    hLET->GetXaxis()->SetTitle("LET [keV/um]");
    hLET->GetYaxis()->SetTitle("N[-]");
    hLET->SetTitle("Histogram of LET (Linear Scale)");

    // Bottom plot with logarithmic y-axis
    c4->cd(2);
    gPad->SetLogy(1);
    gPad->SetLogx();
    hLET->Draw();
    hLET->GetXaxis()->SetRangeUser(0.1, 200);  // Set x-axis range
    hLET->GetXaxis()->SetTitle("LET [keV/um]");
    hLET->GetYaxis()->SetTitle("N[-]");
    hLET->SetTitle("Histogram of LET (Log Scale)");    



    // Create a new canvas for the kinetic and deposited energy histograms
    TCanvas *c5 = new TCanvas("c4", "Kinetic and Deposited Energy", 1200, 600);
    c5->Divide(2, 1);

    // Draw kinetic energy histograms
    c5->cd(1);
    hKineticEnergyProtons->SetLineColor(kRed);
    hKineticEnergyProtons->Draw();
    hKineticEnergyElectrons->SetLineColor(kBlue);
    hKineticEnergyElectrons->Draw("SAME");

    // Draw deposited energy histograms
    c5->cd(2);
    hDepositedEnergyProtons->SetLineColor(kRed);
    hDepositedEnergyProtons->Draw();
    hDepositedEnergyElectrons->SetLineColor(kBlue);
    hDepositedEnergyElectrons->Draw("SAME");

    // Create a legend for kinetic and deposited energy histograms
    TLegend *legendEnergy = new TLegend(0.7, 0.7, 0.9, 0.9);
    legendEnergy->AddEntry(hKineticEnergyProtons, Form("Protons: %g", hProtons->GetEntries()), "f");
    legendEnergy->AddEntry(hKineticEnergyElectrons, Form("Electrons: %g", hElectrons->GetEntries()), "f");
    legendEnergy->AddEntry(hKineticEnergyProtons, "Protons", "l");
    legendEnergy->AddEntry(hKineticEnergyElectrons, "Electrons", "l");
    legendEnergy->SetBorderSize(0);
    legendEnergy->SetTextSize(0.03);
    
    c5->cd(1);
    legendEnergy->Draw();
    
    c5->cd(2);
    legendEnergy->Draw();
    
}
