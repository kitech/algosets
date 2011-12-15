#include <stdlib.h>
#include <string>
#include <vector>
#include <list>

#include "stdlog.h"

#include "disjointset.h"

using namespace std;

int find_e(std::vector<std::vector<std::string> > & sets, std::string &psen)
{
    int idx = -1;

    std::vector<std::string> ses;
    std::string sen;
    for (int i = 0; i < sets.size(); ++i) {
        ses = sets.at(i);
        for (int j = 0; j < ses.size(); ++j) {
            sen = ses.at(j);
            if (psen == sen) {
                return i;
            }
        }
    }

    return idx;
}

int combine_tow_sets(std::vector<std::vector<std::string> > & sets, int idx, std::vector<std::string> &setx)
{
    bool found = false;
    for (int i = 0; i < setx.size(); ++i) {
        found = false;
        for (int j = 0; j < sets[idx].size(); ++j) {
            if (sets[idx].at(j) == setx.at(i)) {
                found = true;
                break;
            }
        }
        if (!found) {
            sets[idx].push_back(setx.at(i));
        }
    }

    return -1;
}

int append_to_sets(std::vector<std::vector<std::string> > & sets, std::vector<std::string> &setx)
{
    return -1;
}

std::vector<std::vector<std::string> >
disjoint_set_combine(std::vector<std::vector<std::string> > &avs)
{
    std::vector<std::vector<std::string> > rsets;

    std::vector<std::string> ses;
    std::string sen;
    int fidx;
    for (int i = 0; i < avs.size(); ++i) {
        ses = avs.at(i);
        for (int j = 0; j < ses.size(); ++j) {
            sen = ses.at(j);

            if ((fidx = find_e(rsets, sen)) >=0 ) {
                combine_tow_sets(rsets, fidx, ses);
                break;
            } else {
                if (j == ses.size()-1) {
                    rsets.push_back(ses);
                    break;
                }
            }
        }
    }

    return rsets;
}

void test_disjoint_set()
{
    std::vector<std::string> vs1;
    vs1.push_back("aaa");
    vs1.push_back("bbb");
    vs1.push_back("ccc");

    std::vector<std::string> vs2;
    vs2.push_back("bbb");
    vs2.push_back("ddd");

    std::vector<std::string> vs3;
    vs3.push_back("eee");
    vs3.push_back("fff");

    std::vector<std::string> vs4;
    vs4.push_back("ggg");

    std::vector<std::string> vs5;
    vs5.push_back("ddd");
    vs5.push_back("hhh");


    std::vector<std::vector<std::string> > avs;
    avs.push_back(vs1);
    avs.push_back(vs2);
    avs.push_back(vs3);
    avs.push_back(vs4);
    avs.push_back(vs5);



    std::vector<std::vector<std::string> > diset;

    diset = disjoint_set_combine(avs);

    qLogx()<<diset.size();
    std::string sstr;

    for (int i = 0; i < diset.size(); ++i) {

        for (int j = 0; j < diset.at(i).size(); ++j) {
            if (j == 0) {
                sstr += "{";
            }

            sstr += diset.at(i).at(j);

            if (j < diset.at(i).size()-1) {
                sstr += ", ";
            }

            if (j == diset.at(i).size()-1) {
                sstr += "}";
            }
        }

        if (i < diset.size()-1) {
            sstr += ", ";
        }
    }

    qLogx()<<sstr;
}
