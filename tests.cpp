// This tells Catch to provide a main()
//      (only do this in one cpp file)
#define CATCH_CONFIG_MAIN  
#include "catch2/catch.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <functional>

#include "src/triangular.h"
#include "src/multiply.h"
#include "src/formats.h"

using namespace std;

string Lp = "./data/smallL.mtx";
string bp = "./data/smallb.mtx";

TEST_CASE("formats") {

    const auto test_coo = [](
        const string& path_to_mtx,
        int m, int n, int nnz, 
        const vector<int>& r, const vector<int>&c, const vector<double>& x)
    {
        auto M = COO<double>(path_to_mtx.c_str());
        auto expectedM = COO<double>();
        expectedM.m = m;
        expectedM.n = n;
        expectedM.nnz = nnz;
        expectedM.rowidx = (int*) malloc(nnz * sizeof(int));
        expectedM.colidx = (int*) malloc(nnz * sizeof(int));
        expectedM.values = (double*) malloc(nnz * sizeof(double));
        std::copy(r.begin(), r.end(), expectedM.rowidx);
        std::copy(c.begin(), c.end(), expectedM.colidx);
        std::copy(x.begin(), x.end(), expectedM.values);
        REQUIRE(expectedM == M);
    };

    const auto test_csc = [](
        const string& path_to_mtx,
        int m, int n, int nnz,
        const vector<int>& p, const vector<int>&i, const vector<double>& x)
    {
        auto M = CSC<double>(path_to_mtx.c_str());
        auto expectedM = CSC<double>();
        expectedM.m = m;
        expectedM.n = n;
        expectedM.nnz = nnz;
        expectedM.p = (int*) malloc((n+1) * sizeof(int));
        expectedM.i = (int*) malloc(nnz * sizeof(int));
        expectedM.x = (double*) malloc(nnz * sizeof(double));
        std::copy(p.begin(), p.end(), expectedM.p);
        std::copy(i.begin(), i.end(), expectedM.i);
        std::copy(x.begin(), x.end(), expectedM.x);
        REQUIRE(expectedM == M);
    };


    SECTION("L") {
        vector<int> p = { 0, 2, 4, 6, 9 };
        vector<int> r = { 0, 1, 1, 3, 2, 3, 1, 2, 3 };
        vector<int> c = { 0, 0, 1, 1, 2, 2, 3, 3, 3 };
        vector<double> x = { 1., 7., 2., 9., 1., 8., 4., 1., 1. };
        test_coo(Lp,4,4,9,r,c,x);
        test_csc(Lp,4,4,9,p,r,x);
    }

    SECTION("b") {
        vector<int> p = { 0, 3 };
        vector<int> r = { 0, 1, 3 };
        vector<int> c = { 0, 0, 0 };
        vector<double> x = { 0.1428571429, 3., 10. };
        test_coo(bp,4,1,3,r,c,x);
        test_csc(bp,4,1,3,p,r,x);
    }
}

using lsolveT = function<void (int, int*, int*, double*, double*)>;

const auto test_lsolve = [](
    lsolve_type type,
    const string& Lp, const string& bp,
    const vector<double> expected_x)
{
    auto L = CSC<double>(Lp.c_str(), true);
    auto b = CSC<double>(bp.c_str(), true);

    CSC<double> xs;
    lsolve(type, L, b, xs);

    vector<double> x;
    csc_to_vec(xs, x);
    for (int i = 0; i < x.size(); ++i) {
        // WARN("expected: "<<expected_x[i]<<" true: "<<x[i]);
        REQUIRE(expected_x[i] == Approx(x[i]));
    }
};

const auto tovec = [](const string& file, vector<double>& v) {
    v.clear();
    ifstream in(file);
    string l;
    while(std::getline(in, l)) 
        v.push_back(stod(l));
};
const auto tofile = [](const string& file, const vector<double>& v) {
    ofstream out(file);
    for (auto x : v) 
        out << x << '\n';
};


TEST_CASE("triangular_small") {
    vector<double> sol = { 1./7., 1., 0., 1. };
    auto L = CSC<double>(Lp.c_str());
    auto b = CSC<double>(bp.c_str());
    test_lsolve(lsolve_type::simple, Lp, bp, sol);
    test_lsolve(lsolve_type::eigen, Lp, bp, sol);
    test_lsolve(lsolve_type::reachset, Lp, bp, sol);
}

TEST_CASE("triangular_medium") {
    vector<double> sol;
    tovec("./data/sol_s_medium", sol);
    string Lp = "./data/s_mediumL.mtx";
    string bp = "./data/s_mediumb.mtx";
    test_lsolve(lsolve_type::simple, Lp, bp, sol);
    test_lsolve(lsolve_type::eigen, Lp, bp, sol);
    test_lsolve(lsolve_type::reachset, Lp, bp, sol);
}


TEST_CASE("triangular_large", "[.][long]") {
    SECTION("torso") {
        vector<double> sol;
        tovec("./data/sol_s_torso", sol);
        string Lp = "./data/s_torsoL.mtx";
        string bp = "./data/s_torsob.mtx";
        test_lsolve(lsolve_type::simple, Lp, bp, sol);
        test_lsolve(lsolve_type::eigen, Lp, bp, sol);
        test_lsolve(lsolve_type::reachset, Lp, bp, sol);
    }
}