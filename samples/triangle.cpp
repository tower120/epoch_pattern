#include <iostream>
#include <tuple>
#include <cmath>

#include "../Epoch.h"
#include "../EpochTuple.h"
#include "../EpochTupleView.h"

using namespace tower120::utils;

struct Point {
    float x,y;
};
float distance(const Point& a, const Point& b){
    const float delta_x = b.x - a.x;
    const float delta_y = b.y - a.y;

    return std::sqrt(delta_x*delta_x + delta_y*delta_y);
}


class Triangle{
protected:
    Point a,b,c;

    Epoch a_epoch, b_epoch, c_epoch;
public:
    void set_a(Point point){
        a = point;
        a_epoch++;
    }
    void set_b(Point point){
        a = point;
        b_epoch++;
    }
    void set_c(Point point){
        a = point;
        c_epoch++;
    }
};


// Can you do this with dirty-flag, without changing Triangle?
class Pyramid : public Triangle{
    // cached:
    float ab, ac, bc;
    EpochTuple<Epoch, Epoch> ab_epoch = ab_epoch.Pre();
    EpochTuple<Epoch, Epoch> ac_epoch = ac_epoch.Pre();
    EpochTuple<Epoch, Epoch> bc_epoch = bc_epoch.Pre();

    void update_ab(){
        if (!ab_epoch.update(a_epoch, b_epoch)) return;
        ab = distance(a,b);
    }
    void update_ac(){
        if (!ac_epoch.update(a_epoch, c_epoch)) return;
        ac = distance(a,c);
    }
    void update_bc(){
        if (!ac_epoch.update(b_epoch, c_epoch)) return;
        bc = distance(b,c);
    }
public:
    float get_ab(){
        update_ab();
        return ab;
    }
    float get_bc(){
        update_bc();
        return bc;
    }
    float get_ac(){
        update_ac();
        return ac;
    }
};

int main(){

}