{
    std::string title = "100kGamma1MeV_Al_Slab_5mm_Vacuum_fStopandKill_afterSlab"; // Define your title here
    std::string filename = title + ".root";      // Use the title in the filename

    // Top level variables for legend
    float GeneratedParticles = 100000;
    float GeneratorEnergy = 1;
    const char* GeneratedParticleType = "Gamma";
    const char* WorldVolume = "G4_Galactic";
    const char* Shielding = "Aluminium 5.0mm";

    // Open the file and get the tree with data
    TFile *file = TFile::Open(filename.c_str());
    TTree *tree = (TTree*)file->Get("DetectorData");
    
    gStyle->SetOptStat(0); // Disable all statistics

    // Define variables to store the data from the tree
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

    Long64_t nentries = tree->GetEntries(); // Get the number of hits in the detector

    // Variables to find the min and max values
    Double_t minEk = std::numeric_limits<Double_t>::max();
    Double_t maxEk = -std::numeric_limits<Double_t>::max();
    Double_t minEdep = std::numeric_limits<Double_t>::max();
    Double_t maxEdep = -std::numeric_limits<Double_t>::max();
    Double_t minLET = std::numeric_limits<Double_t>::max();
    Double_t maxLET = -std::numeric_limits<Double_t>::max();

    // Loop to find min and max values
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
    TH1D *h1 = new TH1D("KineticEnergy", "...", binEdgesEk.size() - 1, 0, maxEk+maxEk/20);  


    // Variables for Flux plots
    int bins = 128;
    int min = -7;
    int max = 7;

    // Define histograms for hits on detector
    TH2F *hTotalHits = new TH2F("hTotalHits", "Total Hits;X (mm);Y (mm)", bins, min, max, bins, min, max); 
    TH2F *hPrimaries = new TH2F("hPrimaries", "Hits of primaries;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hSecondaries = new TH2F("hSecondaries", "Hits of secondaries;X (mm); Y(mm)", bins, min, max, bins, min, max);
    TH2F *hProtons = new TH2F("hProtons", "Proton Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hElectrons = new TH2F("hElectrons", "Electron Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hGammas = new TH2F("hGammas", "Gamma Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hXrays = new TH2F("hXrays", "X-ray Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hNeutrons = new TH2F("hNeutrons", "Neutron Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);
    TH2F *hOtherParticles = new TH2F("hOtherParticles", "e+g+X Hits;X (mm);Y (mm)", bins, min, max, bins, min, max);

    // Manual for loop for variable bin edges
    const int nBins = 280;
    double binEdgesManual[nBins + 1];
    double BinEdge = 0.001;
    for (int i = 0; i <= nBins; ++i) {
        binEdgesManual[i] = BinEdge;
        BinEdge = BinEdge + BinEdge * 0.05;
    }

    // For LET
    TH1D *hLET = new TH1D("LET", "", nBins, binEdgesManual);

    /*
    // For Deposited Energy
    TH1D *h2 = new TH1D("DepositedEnergy", "", binEdgesEdep.size() - 1, 1, maxEdep+maxEdep/20);


    // Define histograms for kinetic and deposited energy for protons and electrons
    TH1F *hKineticEnergyProtons = new TH1F("hKineticEnergyProtons", "Kinetic Energy;Kinetic Energy (MeV);Counts", binEdgesEk.size() - 1, 0.001, maxEk+maxEk/20);
    TH1F *hDepositedEnergyProtons = new TH1F("hDepositedEnergyProtons", "Deposited Energy;Deposited Energy (MeV);Counts", binEdgesEdep.size() - 1, 0.001, maxEdep+maxEdep/20);
    TH1F *hKineticEnergyElectrons = new TH1F("hKineticEnergyElectrons", "Kinetic Energy of Electrons;Kinetic Energy (MeV);Counts", binEdgesEk.size() - 1, 0.001, maxEk+maxEk/20);
    TH1F *hDepositedEnergyElectrons = new TH1F("hDepositedEnergyElectrons", "Deposited Energy by Electrons;Deposited Energy (MeV);Counts", binEdgesEdep.size() - 1, 0.001, maxEdep+maxEdep/20);

    */

    // Loop over all entries in the tree to fill the histogram
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        // Fill the kinetic energy histogram
        h1->Fill(Ek); 

        // Fill the total hits histogram
        hTotalHits->Fill(x, y);

        if (ParentID == 0) {
            hPrimaries->Fill(x, y);
        }
        else if (ParentID > 0) {
            hSecondaries->Fill(x, y); 
        }

        // Fill the LET histogram
        hLET->Fill(LET);

        if (Type == 'p') {
            hProtons->Fill(x, y);
            //hKineticEnergyProtons->Fill(Ek);
            //hDepositedEnergyProtons->Fill(Edep);
        } 

        else if (Type == 'e') {
            hElectrons->Fill(x, y);
            //hKineticEnergyElectrons->Fill(Ek);
        } 
        else if (Type == 'x') { // assuming "x-ray" is used for X-rays
            hXrays->Fill(x, y);
        }
        else if (Type == 'g') {
            hGammas->Fill(x, y);
        }
        else if (Type == 'n') {
            hNeutrons->Fill(x, y);
        }
        else
        {
            hOtherParticles->Fill(x, y);
        }
        

        /*
        h2->Fill(Edep);
        h3->Fill(LET);
        
        
        if (ParentID == 0) {
            hPrimaries->Fill(x, y);
        }
        else if (ParentID > 0) {
            hSecondaries->Fill(x, y); 
        }*/
    }       


    // Kinetic Energy plot
    std::string title_c1 = title + "_Energies"; // Define the title when saved
    TCanvas *c1 = new TCanvas(title_c1.c_str(), title_c1.c_str(), 800, 800); // Define the canvas size e.g. 800, 800 is square aspect ratio 

    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    h1->SetTitle("Kinetic Energy Spectrum at Detector; Kinetic Energy [MeV]; Count [#]"); // Title in plot, X-axis label, Y-axis label
    double mean = h1->GetMean(); // Get the mean value of the data

    // Create a legend and set its position
    TLegend *legend = new TLegend(0.18, 0.7, 0.53, 0.9); // Adjust position (x1, y1, x2, y2)
    legend->SetTextSize(0.02); // Adjust text size
    legend->SetMargin(0.08); // Adjust margin size

    // Add entries to the legend
    legend->AddEntry((TObject*)0, Form("Generated particles = %.0f", GeneratedParticles), "");
    legend->AddEntry((TObject*)0, Form("Hits on detector= %.2lld ", nentries), "");
    legend->AddEntry((TObject*)0, Form("Generated particle type = %s", GeneratedParticleType), "");
    legend->AddEntry((TObject*)0, Form("Energy @ generator = %.1f MeV", GeneratorEnergy), "");
    legend->AddEntry((TObject*)0, Form("Simulation Environment = %s", WorldVolume), "");
    legend->AddEntry((TObject*)0, Form("Shielding = %s", Shielding), "");
    legend->AddEntry((TObject*)0, Form("Mean energy @ detector = %.4f MeV", mean), ""); // "l" is for line style

    // Draw the histogram and legend
    h1->Draw();
    legend->Draw();  


    //FLUX plots 
    std::string title_c2 = title + "_Flux";
    TCanvas *c2 = new TCanvas(title_c2.c_str(), "Hits per pixel", 2000, 600);
    c2->Divide(3, 1);

    c2->cd(1);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.12);
    hTotalHits->SetTitle("Hits on Detector; X [mm]; Y [mm]"); // Title in plot, X-axis label, Y-axis label    double mean = h1->GetMean(); // Get the mean value of the data

    // Create a legend and set its position
    TLegend *legend_flux1 = new TLegend(0.5, 0.7, 0.88, 0.9); // Adjust position (x1, y1, x2, y2)
    legend_flux1->SetTextSize(0.02); // Adjust text size
    legend_flux1->SetMargin(0.08); // Adjust margin size

    // Add entries to the legend
    legend_flux1->AddEntry((TObject*)0, Form("Generated particles = %.0f", GeneratedParticles), "");
    legend_flux1->AddEntry((TObject*)0, Form("Hits on detector = %.2lld ", nentries), "");
    legend_flux1->AddEntry((TObject*)0, Form("Generated particle type = %s", GeneratedParticleType), "");
    legend_flux1->AddEntry((TObject*)0, Form("Energy @ generator = %.2f MeV", GeneratorEnergy), "");
    legend_flux1->AddEntry((TObject*)0, Form("Simulation Environment = %s", WorldVolume), "");
    legend_flux1->AddEntry((TObject*)0, Form("Shielding = %s", Shielding), "");
    legend_flux1->AddEntry((TObject*)0, Form("Mean energy @ detector = %.2f MeV", mean), ""); // "l" is for line style

    hTotalHits->Draw("COLZ");
    legend_flux1->Draw();


    c2->cd(2);
    gPad->SetLeftMargin(0.12);  
    gPad->SetRightMargin(0.12);
    hPrimaries->SetTitle("Primary Particle Hits; X [mm]; Y [mm]"); // Title in plot, X-axis label, Y-axis label
    TLegend *legend_flux2 = new TLegend(0.5, 0.8, 0.88, 0.9); // Adjust position (x1, y1, x2, y2)
    legend_flux2->SetTextSize(0.02); // Adjust text size
    legend_flux2->SetMargin(0.08); // Adjust margin size

    // Add entries to the legend
    legend_flux2->AddEntry(hPrimaries, Form("Primary particles counted: %g", hPrimaries->GetEntries()), "");
    legend_flux2->AddEntry(hSecondaries, Form("Secondary particles counted: %g", hSecondaries->GetEntries()), "");

    hPrimaries->Draw("COLZ");
    legend_flux2->Draw();

    c2->cd(3);
    gPad->SetLeftMargin(0.12);
    gPad->SetRightMargin(0.12);
    // Create a legend and set its position
    TLegend *legend_flux3 = new TLegend(0.6, 0.7, 0.88, 0.9); // Adjust position (x1, y1, x2, y2)
    legend_flux3->SetTextSize(0.02); // Adjust text size
    legend_flux3->SetMargin(0.08); // Adjust margin size
    //legend->SetTextAlign(12); // Align text to the left

    // Add entries to the legend
    legend_flux3->AddEntry(hProtons, Form("Protons counted: %g", hProtons->GetEntries()), "");
    legend_flux3->AddEntry(hElectrons, Form("Electrons counted: %g", hElectrons->GetEntries()), "");
    legend_flux3->AddEntry(hGammas, Form("Gammas counted: %g", hGammas->GetEntries()), "");
    legend_flux3->AddEntry(hXrays, Form("X-rays counted: %g", hXrays->GetEntries()), "");
    legend_flux3->AddEntry(hNeutrons, Form("Neutrons counted: %g", hNeutrons->GetEntries()), "");
    legend_flux3->AddEntry(hOtherParticles, Form("Other particles counted: %g", hOtherParticles->GetEntries()), "");
    //legend_flux->AddEntry((TObject*)0, Form("Simulation Environment = %s", WorldVolume), "");
    //legend_flux->AddEntry((TObject*)0, Form("Shielding? = %s", Shielding), "");
    //legend_flux->AddEntry((TObject*)0, Form("Mean energy @ detector = %.2f MeV", 1), ""); // "l" is for line style
    hSecondaries->SetTitle("Secondary Particle Hits; X [mm]; Y [mm]"); // Title in plot, X-axis label, Y-axis label
    hSecondaries->Draw("COLZ");
    legend_flux3->Draw();


    


    // LET plots
    TH1D *hLET_log = (TH1D*)hLET->Clone("hLET_log"); //for log scale

    std::string title_c4 = title + "_LET";
    TCanvas *c4 = new TCanvas(title_c4.c_str(), "LET", 1800, 900);
    c4->Divide(1, 2);

    double mean_LET = hLET->GetMean(); // Get the mean value of the data

    // Create a legend and set its position
    TLegend *legend_LET = new TLegend(0.75, 0.6, 0.9, 0.9); // Adjust position (x1, y1, x2, y2)
    legend_LET->SetTextSize(0.025); // Adjust text size
    legend_LET->SetMargin(0.08); // Adjust margin size

    // Add entries to the legend
    legend_LET->AddEntry((TObject*)0, Form("Generated particles = %.0f", GeneratedParticles), "");
    legend_LET->AddEntry((TObject*)0, Form("Hits on detector= %.2lld", nentries), "");
    legend_LET->AddEntry((TObject*)0, Form("Generated particle type = %s", GeneratedParticleType), "");
    legend_LET->AddEntry((TObject*)0, Form("Energy @ generator = %.1f MeV", GeneratorEnergy), "");
    legend_LET->AddEntry((TObject*)0, Form("Simulation Environment = %s", WorldVolume), "");
    legend_LET->AddEntry((TObject*)0, Form("Shielding = %s", Shielding), "");
    legend_LET->AddEntry((TObject*)0, Form("Mean LET @ detector = %.2f keV/um", mean_LET), ""); // "l" is for line style
    
    // Top plot with linear y-axis
    c4->cd(1);
    gPad->SetLogy(0);
    gPad->SetLogx();
    hLET->Draw();
    legend_LET->Draw();
    hLET->GetXaxis()->SetRangeUser(0.1, 200);  // Set x-axis range
    hLET->GetXaxis()->SetTitle("LET [keV/um]");
    hLET->GetYaxis()->SetTitle("N[-]");
    hLET->SetTitle("Histogram of LET (Linear Scale)");

    // Bottom plot with logarithmic y-axis
    c4->cd(2);
    gPad->SetLogy(1);
    gPad->SetLogx();
    hLET_log->Draw();
    hLET_log->GetXaxis()->SetRangeUser(0.1, 200);  // Set x-axis range
    hLET_log->GetXaxis()->SetTitle("LET [keV/um]");
    hLET_log->GetYaxis()->SetTitle("N[-]");
    hLET_log->SetTitle("Histogram of LET (Log Scale)");    

 
} 