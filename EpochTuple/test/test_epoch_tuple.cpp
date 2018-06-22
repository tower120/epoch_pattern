#include <iostream>
#include <tuple>

#include "../Epoch.h"
#include "../EpochTuple.h"
#include "../EpochTupleView.h"

using namespace tower120::utils;

struct Data{
    Epoch epoch;

    struct AEpoch : Epoch{} a_epoch;
    Epoch b_epoch;
    int a,b;

    void set_a(int a){
        this->a  = a;
        epoch++;  a_epoch++;
    }
    void set_b(int b){
        this->b  = b;
        epoch++;  b_epoch++;
    }

    std::tuple<Epoch, AEpoch, Epoch> update_epoch;
    void update(){
        if (!epoch_tuple(update_epoch)
                .update(epoch, a_epoch, b_epoch)) return;
    }

    EpochTuple<Epoch, AEpoch, Epoch> update_epoch2;
    void update_2(){
        if (!update_epoch2.update(epoch, a_epoch, b_epoch)) return;

    }

};

int main(){
    Data().update();
}