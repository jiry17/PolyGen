#include "decision_tree_unifier.h"
#include "config.h"
#include "glog/logging.h"
#include "util.h"
#include <queue>
#include <set>
#include <boost/dynamic_bitset.hpp>
#include <unordered_map>
#include <semantics_factory.h>
std::vector<boost::dynamic_bitset<> > cover;
std::vector<boost::dynamic_bitset<> > pred_val;
std::vector<Program*> terms;
std::vector<PointExample*> pts;
std::vector<Program*> preds;
namespace {
    
    double calc_entropy(const std::vector<int> &pts_id, const std::vector<int> &relate_term_id)
    {
        const double eps = 1e-7;
        double ans2 = 0;
        std::vector<std::vector<int>> ipts;
        std::unordered_map<int , int> icnt;
        for (int i = 0; i < relate_term_id.size(); ++i)
        {
            int tid = relate_term_id[i];
            std::vector<int> ipt;
            for(auto j : pts_id) if(cover[tid][j]) ipt.push_back(j);
            for(auto j : ipt) icnt[j] += ipt.size();
            ipts.push_back(ipt);
        }
        for (size_t i = 0; i < relate_term_id.size(); ++i)
        {
            size_t tid = relate_term_id[i];
            double prob = 0;
            double top = ipts[i].size();
            for (auto pid : ipts[i])
            {
                double bot = icnt[pid];
                prob += 1 / bot;
            }
            prob *= top;
            if (prob > eps)
            {
                prob /= pts_id.size();
                double v = -prob * log2(prob);
                ans2 += v;
            }
        }
        return ans2;
    }

    int choose_pred(const std::vector<int>& pts_id, const std::vector<int>& preds_id, std::pair<std::vector<int> ,std::vector<int> > &son_pts_id){

        double best_info_gain = 1e9;
        int res_id = -1;

    
        std::vector<int> relate_term_id;
        for(int i = 0; i < terms.size(); i++){
            int useful = 0;
            for(auto eid : pts_id) if (cover[i][eid]) {
                useful = 1;
                break;
            }
            if(useful) relate_term_id.push_back(i);
        }

        for(auto pid : preds_id){
            std::vector<int> pts_t,pts_f;
            for(auto eid : pts_id) {
                if (pred_val[pid][eid] == 1) pts_t.push_back(eid);else pts_f.push_back(eid);
            } 
            double entrpy_true = calc_entropy(pts_t, relate_term_id);
            double entrpy_false = calc_entropy(pts_f, relate_term_id);
            double info_gain = (entrpy_true * pts_t.size() +
                                entrpy_false * pts_f.size()) /pts_id.size();
            if (info_gain < best_info_gain)
            {
                best_info_gain = info_gain;
                res_id = pid;
                son_pts_id = std::make_pair(pts_t, pts_f);
            }
        }
        return res_id;
    }

    Program* DTlearn(const std::vector<int>& pts_id, const std::vector<int>& preds_id){


        boost::dynamic_bitset<> need_cov = boost::dynamic_bitset<>(pts.size());
        for(auto i : pts_id) need_cov[i] = 1;
        

        for(int i = 0; i < terms.size(); i++)
            if ((need_cov & cover[i]) == need_cov) return terms[i];
        
        if (preds_id.empty()) return nullptr;

        std::pair<std:: vector<int> , std:: vector<int> > son_pts_id; 
        int chosen_pred_id = choose_pred(pts_id, preds_id, son_pts_id);

        if(son_pts_id.first.size() == 0 || son_pts_id.second.size() == 0) return nullptr;

        std::vector<int> son_preds_id = preds_id;
        son_preds_id.erase(remove_if(son_preds_id.begin(), son_preds_id.end(), 
                                    [chosen_pred_id](int x){return x == chosen_pred_id;}), son_preds_id.end());

        auto branch_true = DTlearn(son_pts_id.first , son_preds_id);
        if(nullptr == branch_true) return nullptr;
        auto branch_false = DTlearn(son_pts_id.second , son_preds_id);
        if(nullptr == branch_false) return nullptr;
        
        auto res = new Program({preds[chosen_pred_id], branch_true, branch_false}, semantics::getSemanticsFromName("ite"));
        return res;
    }
}

Program* DecisionTreeUnifier::unify(const std::vector<PointExample*>& example_list,  const std::vector<Program*>& term_list, const std::vector<Program*>& pred_list){
    std::vector<boost::dynamic_bitset<> > cov(term_list.size(),boost::dynamic_bitset<>(example_list.size()));

    for(int i = 0; i < term_list.size(); i++){
        for(int j = 0; j < example_list.size(); j++){
            auto term = term_list[i];
            auto exp = example_list[j];
            cov[i][j] = (term->run(exp->first) == exp->second); 
        }
    }
    
    std::vector<boost::dynamic_bitset<> > pred_value(pred_list.size(),boost::dynamic_bitset<>(example_list.size()));
    for(int i = 0; i < pred_list.size(); i++){
        for(int j = 0; j < example_list.size(); j++){
            auto pred = pred_list[i];
            auto exp = example_list[j];
            pred_value[i][j] = (pred->run(exp->first) == Data(true)); 
        }
        
    }
    cover = cov;
    pred_val = pred_value;
    terms = term_list;
    preds = pred_list;
    pts = example_list;
    std::vector<int> pts_id, preds_id;
    for(int i = 0 ; i < example_list.size(); i ++) pts_id.push_back(i);
    for(int i = 0 ; i < pred_list.size(); i ++) preds_id.push_back(i);
    Program* res = DTlearn(pts_id, preds_id);
    
    return res;
}
