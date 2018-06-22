#include <iostream>
#include <tuple>
#include <cmath>

#include "../Epoch.h"
#include "../EpochTuple.h"
#include "../EpochTupleView.h"

using namespace tower120::utils;


class SettingsWindow{
    Epoch epoch;
    Epoch save_epoch;
    Epoch canvas_update_epoch;

    void save(){
        std::cout << "Saving" << std::endl;
    }
    void redraw_canvas(){
        std::cout << "Redraw canvas" << std::endl;
    }
public:
    // Pay attention to setters. They update only one "flag". With "dirty-flags" you'll need to update 2.
    // one for redraw, one for save. The more features you'll have the more dirty-flags you need.
    // With Epochs you need to update only one.
    void set_resolution(){
        epoch++;
    }
    void set_gamma(){
        epoch++;
    }
    void set_anything(){
        epoch++;
    }

    void draw(){
        if (canvas_update_epoch.update(epoch)){
            redraw_canvas();
        }
    }

    void SaveClk(){
        if (save_epoch.update(epoch)){
            save();
        }
    }
};



int main(){
}