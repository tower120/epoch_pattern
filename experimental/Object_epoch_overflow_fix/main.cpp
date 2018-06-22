#include <iostream>
#include <functional>

class EpochBase{};

template<class Tag>
class Epoch : public EpochBase{
    template<typename> friend class Epoch;

    using Self = Epoch<Tag>;
    constexpr const static int start_value = 0;
    constexpr const static int max_value = 4;
    int value = start_value;

    using EpochRestartCallback = std::function<void(void*)>;
    inline static std::vector<EpochRestartCallback> restart_callbacks;
public:
    Epoch(){}

    template<class OtherEpoch, class Res, class Class>
    Epoch(OtherEpoch& epoch, Res Class::* member_pointer){
        // register only once per Epoch + Class Member
        static bool registered = false;
        if (!registered){
            registered = true;

            epoch.on_restart([member_pointer](void* self){
                // keep old style cast for auto static/dynamic cast
                Self& epoch_point = ((Class*)self)->*member_pointer;
                epoch_point.value = start_value;
            });
        }
    }

    void restart(void* self){
        for(EpochRestartCallback& callback : restart_callbacks){
            callback(self);
        }
        value = start_value;
    }

    Epoch& next(void* self){
        if (value == max_value){
            restart(self);
        } else {
            value++;
        }
        return *this;
    }

    static void on_restart(EpochRestartCallback&& callback){
        restart_callbacks.emplace_back(std::move(callback));
    }

    auto get_value() const {
        return value;
    }

    template<class OtherTag>
    Epoch& operator=(const Epoch<OtherTag>& other) {
        value = other.value;
        return *this;
    }
};



struct Data{
    Epoch<Data> epoch;

    Epoch<struct P1> p1{epoch, &Data::p1}; //auto register "reset callback"
    Epoch<struct P2> p2{epoch, &Data::p2};
};


int main () {
    Data data0;
    data0.p1 = data0.epoch.next(&data0).next(&data0);
    Data& data = data0;

    std::cout << data.p1.get_value() << std::endl;
    data.epoch.next(&data).next(&data);

    std::cout << data.p1.get_value() << std::endl;
}