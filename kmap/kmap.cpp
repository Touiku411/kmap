#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;
struct Term {
    vector<int> bits;
    vector<int> nums;
    bool used = false;
};
vector<int> ToBinary(string& s) {
    vector<int> mask(4);
    int num = stoi(s);
    mask[0] = (num >> 3) & 1;
    mask[1] = (num >> 2) & 1;
    mask[2] = (num >> 1) & 1;
    mask[3] = (num >> 0) & 1;
    return mask;
}
bool canMerge(vector<int>& v1, vector<int>& v2) {
    int diff = 0;
    for (int i = 0; i < v1.size(); ++i) {
        if (v1[i] != v2[i])
            diff++;
    }
    return (diff == 1);
}
int diffIdx(vector<int>& v1, vector<int>& v2) {
    for (int i = 0; i < v1.size(); ++i) {
        if (v1[i] != v2[i])
            return i;
    }
    return -1;
}
Term termMerge(Term& t1, Term& t2) {
    t1.used = true;
    t2.used = true;
    Term t;
    int idx = diffIdx(t1.bits, t2.bits);
    t.bits = t1.bits;
    t.bits[idx] = -1;
    t.nums.insert(t.nums.begin(), t1.nums.begin(), t1.nums.end());
    t.nums.insert(t.nums.begin(), t2.nums.begin(), t2.nums.end());
    sort(t.nums.begin(), t.nums.end());
    return t;
}
void printSolution(vector<Term>& t) {
    cout << "Simplified SOP Expression:" << endl;

    if (t.empty()) {
        cout << "0" << endl;
        return;
    }
    char vars[] = { 'A','B','C','D' };
    string out;
    for (int k = 0; k < t.size(); ++k) {
        string s;
        for (int i = 0; i < 4; ++i) {
            if (t[k].bits[i] == 1) {
                s += vars[i];
            }
            else if (t[k].bits[i] == 0) {
                s += vars[i];
                s += "'";
            }
        }
        if (s.empty()) {
            cout << "1\n";
            return;
        }
        out += s;
        if (k < t.size() - 1) {
            out += " + ";
        }
    }
    cout << out << endl;
}
int main()
{
    string input;
    while (getline(cin, input)) {
        stringstream ss(input);
        string word;
        vector<Term> minterms;
        while (ss >> word) {
            Term t;
            t.bits = ToBinary(word);
            t.nums.push_back(stoi(word));
            minterms.push_back(t);
        }

        vector<vector<Term>> groups(5);
        for (auto &m : minterms) {
            Term t;
            int ones = count(m.bits.begin(), m.bits.end(), 1);
            t.bits = m.bits;
            t.nums = m.nums;
            groups[ones].push_back(t);
        }
        vector<Term> primeImplicants;
        while (true) {
            vector<vector<Term>> nextgroups(5);
            bool merge = false;
            for (int i = 0; i < 4; ++i) {
                for (auto& term1 : groups[i]) {
                    for (auto& term2 : groups[i + 1]) {
                        if (canMerge(term1.bits, term2.bits)) {
                            merge = true;
                            Term t = termMerge(term1, term2);
                            int ones = count(t.bits.begin(), t.bits.end(), 1);
                            bool exist = false;
                            for (auto& c : nextgroups[ones]) {
                                if (t.bits == c.bits)
                                    exist = true;
                            }
                            if (!exist)
                                nextgroups[ones].push_back(t);
                        }
                    }
                }
            }

            for (auto& i : groups) {
                for (auto& t : i) {
                    if (!t.used) {
                        primeImplicants.push_back(t);
                    }
                }
            }
            if (!merge)break;
            for (int i = 0; i < 5; ++i) {
                groups[i] = nextgroups[i];
            }
        }
        vector<int> counts(16, 0);

        for (auto& pi : primeImplicants) {
            for (auto i : pi.nums) {
                counts[i]++;
            }
        }
        vector<Term> solution;
        vector<bool> isCovered(16, false);
        for (auto& pi : primeImplicants) {
            bool isEPI = false;
            for (auto i : pi.nums) {
                if (counts[i] == 1) {
                    isEPI = true;
                    break;
                }
            }
            if (isEPI) {
                solution.push_back(pi);
                for (auto i : pi.nums) {
                    isCovered[i] = true;
                }
            }
        }
        while (true) {
            vector<int> unCovered;
            for (auto& i : minterms) {
                int num = i.nums[0];
                if (isCovered[num] == false) {
                    unCovered.push_back(num);
                }  
            }
            if (unCovered.empty())break;
            Term BestPI;
            int MaxCoveredCount = -1;
            for (auto& pi : primeImplicants) {
                int currentCoveredCount = 0;
                for (auto i : pi.nums) {
                    for (auto t : unCovered) {
                        if (i == t) {
                            currentCoveredCount++;
                        }
                    }
                }
                if (currentCoveredCount > MaxCoveredCount) {
                    MaxCoveredCount = currentCoveredCount;
                    BestPI = pi;
                }
            }
            solution.push_back(BestPI);
            for (auto i : BestPI.nums) {
                isCovered[i] = true;
            }
        }

        printSolution(solution);
    }
}

