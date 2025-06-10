#ifndef ENV
#define ENV

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>

using namespace std;

template <typename T>
class Environment {
   private:
    vector<unordered_map<string, T> > ribs;
    int search_rib(string var) {
        int idx = ribs.size() - 1;
        while (idx >= 0) {
            typename std::unordered_map<std::string, T>::const_iterator it =
                ribs[idx].find(var);
            if (it != ribs[idx].end())
                return idx;
            idx--;
        }
        return -1;
    }

   public:
    Environment() {}
    void clear() { ribs.clear(); }
    void add_level() {
        unordered_map<string, T> r;
        ribs.push_back(r);
    }
    void add_var(string var, T value) {
        if (ribs.size() == 0) {
            cout << "Environment sin niveles: no se pueden agregar variables"
                 << endl;
            exit(0);
        }
        ribs.back()[var] = value;
    }
    void add_var(string var) { ribs.back()[var] = 0; }

    bool remove_level() {
        if (ribs.size() > 0) {
            ribs.pop_back();
            return true;
        }
        return false;
    }
    bool update(string x, T v) {
        int idx = search_rib(x);
        if (idx < 0)
            return false;
        ribs[idx][x] = v;
        return true;
    }
    bool check(string x) {
        int idx = search_rib(x);
        return (idx >= 0);
    }
    T lookup(string x) {
        T a;
        int idx = search_rib(x);
        if (idx < 0)
            return a;
        else
            return ribs[idx][x];
    }
    bool lookup(string x, T& v) {
        int idx = search_rib(x);
        if (idx < 0)
            return false;
        v = ribs[idx][x];
        return true;
    }
};

#endif
