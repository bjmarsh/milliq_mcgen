
#include "MCPTree.h"
#include "TTree.h"
#include "TDirectory.h"
#include <iostream>
#include <vector>

using namespace std;

MCPTree::MCPTree(TTree *t){
    if(t != NULL)
        Init(t);
}

void MCPTree::Init(TTree *t){
    if(t == NULL){
        // if we don't pass a tree, open in "write" mode. pointers have to be initialized,
        // and then create the branches.
        this->t = new TTree("Events","");

        this->parent_p4    = new LorentzPtEtaPhiMf;
        this->p4_p         = new LorentzPtEtaPhiMf;
        this->p4_m         = new LorentzPtEtaPhiMf;

        b_event        = this->t->Branch("event", &event, "event/i");
        b_parent_p4    = this->t->Branch("parent_p4", &parent_p4);
        b_parent_pdgId = this->t->Branch("parent_pdgId", &parent_pdgId, "parent_pdgId/I");
        b_decay_flag   = this->t->Branch("decay_flag", &decay_flag, "decay_flag/I");
        b_p4_p         = this->t->Branch("p4_p", &p4_p);
        b_p4_m         = this->t->Branch("p4_m", &p4_m);
        b_xsec         = this->t->Branch("xsec", &xsec, "xsec/F");
        b_BR_q1        = this->t->Branch("BR_q1", &BR_q1, "BR_q1/F");
        b_filter_eff   = this->t->Branch("filter_eff", &filter_eff, "filter_eff/F");
        b_weight       = this->t->Branch("weight", &weight, "weight/F");
        b_weight_up    = this->t->Branch("weight_up", &weight_up, "weight_up/F");
        b_weight_dn    = this->t->Branch("weight_dn", &weight_dn, "weight_dn/F");

        Reset();
    }else{
        // if we do pass a tree, open in "read" mode
        this->t = t;
        //this->t->SetMakeClass(1);

        this->t->SetBranchAddress("event", &event, &b_event);
        this->t->SetBranchAddress("parent_p4", &parent_p4, &b_parent_p4);
        this->t->SetBranchAddress("parent_pdgId", &parent_pdgId, &b_parent_pdgId);
        this->t->SetBranchAddress("decay_flag", &decay_flag, &b_decay_flag);
        this->t->SetBranchAddress("p4_p", &p4_p, &b_p4_p);
        this->t->SetBranchAddress("p4_m", &p4_m, &b_p4_m);
        this->t->SetBranchAddress("xsec", &xsec, &b_xsec);
        this->t->SetBranchAddress("BR_q1", &BR_q1, &b_BR_q1);
        this->t->SetBranchAddress("filter_eff", &filter_eff, &b_filter_eff);
        this->t->SetBranchAddress("weight", &weight, &b_weight);
        this->t->SetBranchAddress("weight_up", &weight_up, &b_weight_up);
        this->t->SetBranchAddress("weight_dn", &weight_dn, &b_weight_dn);

    }
    t_old = std::chrono::system_clock::now();
}

void MCPTree::Fill(){
    t->Fill();
}

void MCPTree::Reset(){
    event = -999;
    *parent_p4 = LorentzPtEtaPhiMf();
    parent_pdgId = -999;
    decay_flag = -999;
    *p4_p = LorentzPtEtaPhiMf();
    *p4_m = LorentzPtEtaPhiMf();
    xsec = -999;
    BR_q1 = -999;
    filter_eff = -999;
    weight = -999;
    weight_up = -999;
    weight_dn = -999;

}

void MCPTree::Write(TDirectory *d){
    d->cd();
    t->Write();
}

void MCPTree::GetEntry(ULong64_t i){
    this->t->GetEntry(i);
}

void MCPTree::progress( int curr, int tot, int period, unsigned int smoothing) {
    if(curr%period == 0) {
        auto now = std::chrono::system_clock::now();
        double dt = ((std::chrono::duration<double>)(now - t_old)).count();
        t_old = now;
        // if (deq.size() >= smoothing) deq.pop_front();                                                                                                                                                            
        if (deq.size() >= smoothing) deq.erase(deq.begin());
        deq.push_back(dt);
        double avgdt = std::accumulate(deq.begin(),deq.end(),0.)/deq.size();
        float prate = (float)period/avgdt;
        float peta = (tot-curr)/prate;
        if (isatty(1)) {
            float pct = (float)curr/(tot*0.01);
            if( ( tot - curr ) <= period ) pct = 100.0;
            printf("\015\033[32m ---> \033[1m\033[31m%4.1f%% \033[34m [%.3f kHz, ETA: %.0f s] \033[0m\033[32m  <---\033[0m\015 ", pct, prate/1000.0, peta);
            if( ( tot - curr ) > period ) fflush(stdout);
            else cout << endl;
        }
    }
}
