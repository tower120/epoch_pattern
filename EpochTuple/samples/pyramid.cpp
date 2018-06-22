#include <iostream>
#include <tuple>
#include <cmath>

#include "../Epoch.h"
#include "../EpochTuple.h"
#include "../EpochTupleView.h"

using namespace tower120::utils;

struct Point {
    float x,y,z;
};

class Triangle{
    Point a,b,c;
    Epoch epoch;
public:
    void set_a(Point point){
        a = point;
        epoch++;
    }
    void set_b(Point point){
        a = point;
        epoch++;
    }
    void set_c(Point point){
        a = point;
        epoch++;
    }

    const Point& get_a() const{ return a; }
    const Point& get_b() const{ return b; }
    const Point& get_c() const{ return c; }
    Epoch get_epoch() const{ return epoch; }
};


class Pyramid{
    EpochTuple<Epoch, Epoch, Epoch, Epoch> volume_update_epoch;

    float volume;
    void update_volume(){
        if (!volume_update_epoch.update(
                t1.get_epoch()
                ,t2.get_epoch()
                ,t3.get_epoch()
                ,t4.get_epoch()
        )) return;

        //computationally expensive
    }
public:
    Triangle t1, t2, t3, t4;

    float get_volume(){
        update_volume();
        return volume;
    }
};

int main(){

}