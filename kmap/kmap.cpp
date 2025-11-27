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
vector<int> ToBinary(int num) {
    vector<int> mask(4);
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
string generatSoPStr(vector<Term>& t) {
    if (t.empty()) {
        return "0";
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
            return "1";
        }
        out += s;
        if (k < t.size() - 1) {
            out += " + ";
        }
    }
    return out;
}
void toPos(string F) {
    if (F == "0") {
        cout << "0\n";
        return;
    }
    else if (F == "1") {
        cout << "1\n";
        return;
    }
    stringstream ss(F);
    string w;
    vector<string> word;
    vector<string> trans;
    while (ss >> w) {
        if (w != "+")
            word.push_back(w);
    }
    for (auto& w : word) {
        string POS;
        for (int i = 0; i < w.size(); ++i) {
            if (isalpha(w[i])) {
                if (i + 1 < w.size() && w[i + 1] == '\'') {
                    POS += w[i];
                    i++;
                }
                else {
                    POS += w[i];
                    POS += "'";
                }
                if (i < w.size() - 1)
                    POS += " + ";
            }
        }
        if (!POS.empty())
            trans.push_back(POS);
    }
    cout << "Output(POS) ";

    for (auto& w : trans) {
        cout << "(" << w << ")";
    }
    cout << endl;
}
void printkmap(vector<bool>& isMinterm, vector<bool>& isDontCare) {
    string grayLabels[] = { "00", "01", "11", "10" };
    cout << "K-Map:" << endl;
    cout << "AB\\CD\t";
    for (int j = 0; j < 4; ++j) {
        cout << grayLabels[j] << "\t";
    }
    cout << endl;
    cout << "---------------------------------" << endl;

    vector<vector<char>> vec(4, vector<char>(4, '0'));
    vector<vector<int>> pos(4, vector<int>(4));
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == 0)
                pos[i][j] = i + j;
            else if (i == 1)
                pos[i][j] = i + j + 3;
            else if (i == 2)
                pos[i][j] = i + j + 10;
            else if (i == 3)
                pos[i][j] = i + j + 5;
        }
    }
    for (int i = 0; i < 4; ++i) {
        swap(pos[i][2], pos[i][3]);
    }
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int num = pos[i][j];
            if (isMinterm[num]) vec[i][j] = '1';
            else if (isDontCare[num]) vec[i][j] = 'd';
            else vec[i][j] = '0';
        }
    }
    for (int i = 0; i < 4; ++i) {
        cout << grayLabels[i] << "   |\t";
        for (int j = 0; j < 4; ++j) {
            cout << vec[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
}
// 0 1 3 2
// 4 5 7 6
// 12 13 15 14
// 8 9 11 10.
bool alldigit(string& s) {
    for (char c : s) {
        if (!isdigit(c))
            return false;
    }
    return true;
}
string fun(vector<Term>& minterms,vector<bool> isRequired) {
    vector<vector<Term>> groups(5);
    for (auto& m : minterms) {
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
            if (isRequired[i])
                counts[i]++;
        }
    }
    vector<Term> solution;
    vector<bool> isCovered(16, false);
    for (auto& pi : primeImplicants) {
        bool isEPI = false;
        for (auto i : pi.nums) {
            if (isRequired[i] && counts[i] == 1) {
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
            if (isCovered[num] == false && isRequired[num]) {
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
    return generatSoPStr(solution);
}
int main()
{
    string input;
    int cnt = 1;
    while (true) {
        cout << "Test Case: " << cnt++ << endl;
        cout << "Enter minterms (space-separated):" << endl;
        if (!getline(cin, input))break;
        if (input.empty())continue;
        stringstream ss(input);
        string word;
        vector<bool> isMinterm(16,false);
        vector<bool> isDontCare(16, false);
        bool Digit = true;
        while (ss >> word) {
            int num;
            if (word[0] == 'd' || word[0] == 'D') {
                string sub = word.substr(1);
                num = stoi(sub);
                isDontCare[num] = true;
            }
            else if(alldigit(word)){
                num = stoi(word);
                isMinterm[num] = true;
            }
            else {
                Digit = false;
                break;
            }
        }
        if (!Digit)continue;
        vector<Term> termsSOP;
        vector<bool> requiredSOP(16, false);
        for (int i = 0; i < 16; ++i) {
            if (isMinterm[i] || isDontCare[i]) {
                Term t;
                t.bits = ToBinary(i);
                t.nums.push_back(i);
                termsSOP.push_back(t);

                if (isMinterm[i]) requiredSOP[i] = true; // 只有 1 是必須覆蓋的
            }
        }
        vector<Term> termsPOS;
        vector<bool> requiredPOS(16, false);
        for (int i = 0; i < 16; ++i) {
            // 如果不是 Minterm (1)，也不是 DontCare (d)，那就是 Maxterm (0)
            bool isMaxterm = !isMinterm[i] && !isDontCare[i];

            if (isMaxterm || isDontCare[i]) {
                Term t;
                t.bits = ToBinary(i);
                t.nums.push_back(i);
                termsPOS.push_back(t);

                if (isMaxterm) requiredPOS[i] = true; // 在反向邏輯中，0 是必須覆蓋的
            }
        }
        string sopResult = fun(termsSOP, requiredSOP);
        cout << "Output (SOP): " << sopResult << endl;
        string inverseSOP = fun(termsPOS, requiredPOS);
        toPos(inverseSOP);
        printkmap(isMinterm,isDontCare);
    }
}

