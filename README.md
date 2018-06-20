# epoch_pattern
More powerfull version of dirty flag. Each mutation incerease epoch counter. Allow to track changes in entagled parts of object.

## Motivavtion

Consider, that we have Triangle, and it can return length of each edge.
```c++
struct Triangle{
    Point a,b,c;
    
    float calculate_ab();
    float calculate_bc();
    float calculate_ac();
    
    float ab(){ return calculate_ab(); }
    float bc();
    float ac();
};
```

Now consider, that we can change each vertex position. And we also want to cache edges lengths.
```c++
class Triangle{
    Point a,b,c;
    float m_ab, m_bc, m_ac;
    
    float calculate_ab();
    float calculate_bc();
    float calculate_ac();    
public:    
    void set_a(Point point){
        if (a == point) return;
        a = point;
        m_ab = calculate_ab();
        m_ac = calculate_ac();
    }
    void set_b();
    void set_c();
    
    float ab();
    float bc();
    float ac();
};
```

Now:
1. As you see, since each point belongs to 2 edges, we must do 2 updates per each point set. But! If we change all 3 points, we will do 6 updates, when 3 is sufficient (we have only 3 edges).
2. If we move the same point n times, and **then** get edge length, we'll do n updates, but only 1 is sufficient.

Let's try solve this problems with dirty flag. First, start from one flag per whole object.

```c++
class Triangle{
    bool is_dirty = true;
    
    Point a,b,c;
    float m_ab, m_bc, m_ac;
    
    float calculate_ab();
    float calculate_bc();
    float calculate_ac();    
    
    void update(){
        if (!is_dirty) return;  is_dirty = false;
        m_ab = calculate_ab();
        m_ac = calculate_ac();        
        m_bc = calculate_bc();
    }
public:    
    void set_a(Point point){
        if (a == point) return;
        a = point;
        is_dirty = true;
    }
    void set_b();
    void set_c();
    
    float ab(){
        update();
        return m_ab;
    }
    float bc();
    float ac();
};
```

Better, much better. But now we update whole object each time. We can do better, let's have dirty flag for each edge (we can't have dirty flags for points, since 2 updates will depends on each flag)!
```c++
class Triangle{
    bool ab_dirty = true;
    bool ac_dirty = true;
    bool bc_dirty = true;
    
    Point a,b,c;
    float m_ab, m_bc, m_ac;
    
    float calculate_ab();
    float calculate_bc();
    float calculate_ac();        
public:    
    void set_a(Point point){
        if (a == point) return;
        a = point;
        ab_dirty = true;
        ac_dirty = true;
    }
    void set_b();
    void set_c();
    
    float ab(){
        if (ab_dirty){
            ab_dirty = false;
            m_ab = calculate_ab();
        }        
        return m_ab;
    }
    float bc();
    float ac();
};
```

Perfect. Performance-wise. End of story?

And now, we want that our Triangle can tell it's area. And we want it to be cached too, and lazily calculated in the same manner. Well ok, no problem let's add another area_dirty flag, and update all setters with it.
But at the end of the day, we decided to move to 3D - and make our Triangle a Pyramid!
```c++
    void set_a(Point point){
        if (a == point) return;
        a = point;
        area_dirty = true;
        ab_dirty = true;
        ac_dirty = true;
        ad_dirty = true;
    }
    void set_b(Point point){
        if (b == point) return;
        b = point;
        area_dirty = true;
        ba_dirty = true;
        bc_dirty = true;
        bd_dirty = true;
    }    
```
The more cahced values we have, the bigger becomes our setter's dirty update list.... And you need not mess up with combination too... Which dirty flag depends on which setter group...

One probable solution would be having dirty-list for each setter, like:
```c++
    std::vector<*bool> set_a_dirty_list;
    void set_a(Point point){
        if (a == point) return;
        a = point;
        
        for(*bool b : set_a_dirty_list){
            *b = true;
        }
    }
    
    bool ab_dirty = true;
    bool inited = false;
    void update_ab(){            
        if (!inited){
            set_a_dirty_list.emplace_back(&ab_dirty);
            inited = true;
        }
        
        if (!ab_dirty) return;  ab_dirty = false;
        m_ab = calculate_ab();
    }
```
But in this form - your class must be immovable, non copyable, otherwise you invalidate your dirty-pointers.

To bypass this, you could store pointers to members:
```c++
    std::vector<bool Self::*> set_a_dirty_list;
    void set_a(Point point){
        a = point;
        
        for(bool Self::* ptr : set_a_dirty_list){
            (*this).*(ptr) = true;
        }
    }
```

But with this you can't add them from inherited class.


## Object Epoch

Another solution is to have epoch of object. Each time we change object, we move it's epoch forward. Think of it as about object's time or revision.

So, in each setter (which we care about), we increase object epoch, and save epoch. In this way, we know at which epoch field was mutated. We also save epoch during update. In this way we can know, does changes in filed occur before our update or not.

```c++
    int obj_epoch = 0;
    
    int a_mut_epoch = 0;
    Point a;
    
    int b_mut_epoch = 0;
    Point b;
    
    void set_a(Point point){
        a = point;
        obj_epoch++;
        a_mut_epoch = obj_epoch;
    }
    
    void set_b(Point point);
    
    int ab_update_epoch = -1;
    void update_ab(){
       if (obj_epoch == ab_update_epoch) return;    // fast check
       if (ab_update_epoch >= a_epoch && ab_update_epoch >= b_epoch) return;
       
       ab_update_epoch = obj_epoch;
       
       m_ab = calculate_ab();
    }
```
As you can see, setter move time. But we have one problem here. Since we compare for greater or equal we are sensitive for int overflow. If we would just check for equal, we would not have that problem. But if you sure, that you work with relatively short-lived object (not running for weeks), this will do the job.

## Per field Epoch

Simple solution would be storing epoch tuple in update_epoch, like:
```c++
    int a_mut_epoch = 0;
    Point a;
    
    int b_mut_epoch = 0;
    Point b;
    
    void set_a(Point point){
        a = point;
        a_mut_epoch++;
    }
    
    void set_b(Point point);
    
    std::tuple<int,int> ab_update_epoch = {-1};
    void update_ab(){
       if (ab_update_epoch == {a_epoch, b_epoch}) return;
       
       ab_update_epoch = {a_epoch, b_epoch};
       
       m_ab = calculate_ab();
    }
```
In this way we can use equal, instead of greater operation. It is enough to call update once per each MAX_INT mutations per field, and we are fine with int overflow. But now we waste memory for storing epoch tuple. If you fine with memory overhead (it may be not that big, after all), you can safely use this method.

## Final solution.

To get rid of memory overhead of previos method, and deal with int overflow, we can somehow "reset" all stored epoch's. In this way, after overflow, in each update we will be forced to recalculate once and store actual epoch. IOW have callbacks which set all epoch values to 0.

Let's separate Epoch (latest "time point", can be moved forward, one per object), and EpochPoint (contains int value, stored "time point").

```c++
template<class ParentClass>
class Epoch{
    using Value = unsigned int;
    const static Value start_value = 0;
    Value value = start_value;
    using EpochRestartCallback = std::function<void(void*)>;
    static std::vector<EpochRestartCallback> restart_callbacks;
public:
    void restart(void* self){
        for(EpochRestartCallback& callback : restart_callbacks){
            callback(self);
        }
        value = start_value;
    }

    Epoch& next(void* self){
        if [[unlikely]] (value == max){
            restart(self);
        } else {
            value++;
        }
        return *this;
    }

    static void on_restart(EpochRestartCallback&& callback){
        restart_callbacks.emplace_back(std::move(callback));
    }
}

template<class Tag, int start_value = 0>
class EpochPoint{
    using Self = EpochPoint<Tag, start_value>;
    int value = start_value;    
public:
    template<class EpochParentClass, class Res, class Class>
    EpochPoint(Epoch<EpochParentClass>& epoch, Res Class::* member_pointer){
        static_assert(std::is_base_of_v<Self, Res>);
        static_assert(std::is_base_of_v<EpochParentClass, Class>);
        
        // register only once per Epoch + Class Member
        static bool registered = false;
        if (!registered){
            registered = true;

            epoch.on_restart([member_pointer](void* self){
                // keep old style cast for auto static/dynamic cast
                EpochPoint& epoch_point = ((Class*)self)->*member_pointer;
                epoch_point.value = start_value;
            });
        }
    }
    
    template<class EpochParentClass>
    EpochPoint& operator=(const Epoch<EpochParentClass>& epoch){
       value = epoch.get_value();   
       return *this;
    }
    
    int get_value() const{
        return value;
    }
};


    Epoch<Self> epoch;

    EpochPoint<struct A_mut> a_epoch{epoch, &Self::a_epoch};
    Point a;
    
    EpochPoint<struct B_mut> b_epoch{epoch, &Self::b_epoch};
    Point b;
    
    void set_a(Point point){
        a = point;
        a_mut_epoch = epoch.next(this);
    }
    
    void set_b(Point point);
    
    EpochPoint<struct AB_upd> ab_upd_epoch{epoch, &Self::ab_upd_epoch};
    void update_ab(){
       if (ab_upd_epoch == epoch) return;
       if (ab_update_epoch >= a_epoch && ab_update_epoch >= b_epoch) return;
       
       ab_update_epoch = epoch;
       
       m_ab = calculate_ab();
    }

```

As you see, quite verbose definition...


## Epoch advantages

With Epoch you can track does value/object changed without touching target class, you don't need even inherit it:

```c++
struct A{
    Epoch epoch;
    ...
};


struct Data{
    A a;
    B b;
    
    int sum;
    tuple<Epoch, Epoch> update_epoch;
    void update(){
        if (update_epoch == {a.epoch, b.epoch}) return;
        update_epoch = {a.epoch, b.epoch};
                
        sum = a.value + b.value;
    }
};

```

You don't need events, callbacks, nothing. Object state = object time.
