void macro() {
    gStyle->SetOptStat(0000000);
    //Getting data from file
    auto file = new TFile("data.root", "read");
    auto list = static_cast<TList*>(file->Get("list"));

    //Validating data
    double counter[7];
    double total = 0;
    for(int i = 0; i < 7; ++i) {
        counter[i] = static_cast<TH1F*>(list->At(0))->GetBinContent(i+1);
        total += counter[i];
    }
    std::cout << "Particles ratio:\n";
    std::cout << "Pion+ : " << counter[0]/total << "\n";
    std::cout << "Pion- : " << counter[1]/total << "\n";
    std::cout << "Kaon+ : " << counter[2]/total << "\n";
    std::cout << "Kaon- : " << counter[3]/total << "\n";
    std::cout << "Proton : " << counter[4]/total << "\n";
    std::cout << "Antiproton : " << counter[5]/total << "\n";
    std::cout << "K* : " << counter[6]/total << "\n";


    /////////////////////////////////////////////////////////////////////
    //Data analysis
    /////////////////////////////////////////////////////////////////////
    auto canv1 = new TCanvas("canv1", "Graph", 20, 20, 1920, 1080);
    canv1->Divide(3,2);

    canv1->cd(1);

    static_cast<TH1F*>(list->At(0))->GetXaxis()->SetTitle("Prova");
    static_cast<TH1F*>(list->At(0))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(0))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(0))->GetYaxis()->SetTitleOffset(1.5);

    list->At(0)->Draw();

    //Azimutal
    canv1->cd(2);
    
    static_cast<TH1F*>(list->At(1))->Fit("pol0");

    static_cast<TH1F*>(list->At(1))->GetXaxis()->SetTitle("Angle (rad)");
    static_cast<TH1F*>(list->At(1))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(1))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(1))->GetYaxis()->SetTitleOffset(1.5);

    list->At(1)->Draw();

    //Polar
    canv1->cd(3);

    static_cast<TH1F*>(list->At(2))->Fit("pol0");

    static_cast<TH1F*>(list->At(2))->GetXaxis()->SetTitle("Angle (rad)");
    static_cast<TH1F*>(list->At(2))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(2))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(2))->GetYaxis()->SetTitleOffset(1.5);

    list->At(2)->Draw();

    //Impulse
    canv1->cd(4);
    static_cast<TH1F*>(list->At(3))->Fit("expo");

    static_cast<TH1F*>(list->At(3))->GetXaxis()->SetTitle("Impulse module (GeV)");
    static_cast<TH1F*>(list->At(3))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(3))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(3))->GetYaxis()->SetTitleOffset(1.5);

    list->At(3)->Draw();

    canv1->cd(5);

    static_cast<TH1F*>(list->At(4))->GetXaxis()->SetTitle("Trasversal impulse module (GeV)");
    static_cast<TH1F*>(list->At(4))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(4))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(4))->GetYaxis()->SetTitleOffset(1.5);

    list->At(4)->Draw();

    canv1->cd(6);

    static_cast<TH1F*>(list->At(5))->GetXaxis()->SetTitle("Energy (?)");
    static_cast<TH1F*>(list->At(5))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(5))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(5))->GetYaxis()->SetTitleOffset(1.5);

    list->At(5)->Draw();

    canv1->Print("Graphs.jpg");
    delete canv1;
    /////////////////////////////////////////////////////////////////////
    // Comparison
    auto canv10 = new TCanvas("canv10", "canv10",20, 20,1920,540);
    canv10->Divide(3,1);

    canv10->cd(1);
    auto hDiff1 = new TH1F("hDiff1", "Pion/Kaon sign combination difference", 1e3, 0., 4.);
    auto fitFoo = new TF1("fitFoo", "gaus", 0.65, 1.25);

    auto first = static_cast<TH1F*>(list->At(9));
    auto last = static_cast<TH1F*>(list->At(10));
    hDiff1->Add(first , last, 1, -1);

    hDiff1->GetXaxis()->SetTitle("Invariant mass (#frac{GeV}{c^{2}})");
    hDiff1->GetXaxis()->SetTitleOffset(1.2);
    hDiff1->GetYaxis()->SetTitle("Entries");
    hDiff1->GetYaxis()->SetTitleOffset(1.6);
    hDiff1->SetMaximum(4000);

    auto r = hDiff1->Fit(fitFoo, "RS");
    auto cor = r->GetCorrelationMatrix();
    auto cov = r->GetCovarianceMatrix();
    cov.Print();
    cor.Print();
    hDiff1->Draw();

    canv10->cd(2);

    static_cast<TH1F*>(list->At(11))->GetXaxis()->SetTitle("Invariant mass (#frac{GeV}{c^{2}})");
    static_cast<TH1F*>(list->At(11))->GetXaxis()->SetTitleOffset(1.2);
    static_cast<TH1F*>(list->At(11))->GetYaxis()->SetTitle("Entries");
    static_cast<TH1F*>(list->At(11))->GetYaxis()->SetTitleOffset(1.5);

    list->At(11)->Draw();
    first->Reset();
    last->Reset();

    canv10->cd(3);
    auto hDiff2 = new TH1F("hDiff2", "All particles sign combination difference", 1e3, 0., 4.);
    delete fitFoo;
    fitFoo = new TF1("fitFoo", "gaus", 0.4, 1.6);

    first = static_cast<TH1F*>(list->At(7));
    last = static_cast<TH1F*>(list->At(8));
    hDiff2->Add(first , last, 1, -1);

    hDiff2->GetXaxis()->SetTitle("Invariant mass (#frac{GeV}{c^{2}})");
    hDiff2->GetXaxis()->SetTitleOffset(1.2);
    hDiff2->GetYaxis()->SetTitle("Entries");
    hDiff2->GetYaxis()->SetTitleOffset(1.6);

    r = hDiff2->Fit(fitFoo, "RS");
    cor = r->GetCorrelationMatrix();
    cov = r->GetCovarianceMatrix();
    hDiff2->Draw();

    std::cout << hDiff2->GetMean();

    canv10->Print("Fit.jpg");
    delete hDiff1;
    delete hDiff2;
    delete canv10;
}
