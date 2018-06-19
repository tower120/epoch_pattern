# epoch_pattern
More powerfull version of dirty flag. Each mutation incerease epoch counter. Allow to track changes in entagled parts of object.

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

Better, much better. But now we update whole object each time. We can do better, let's have dirty flag for each edge!
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
    void update_ab(){    
        static bool inited = false;
        if (!inited){
            set_a_dirty_list.emplace_back(&ab_dirty);
            inited = true;
        }
        
        if (!ab_dirty) return;  ab_dirty = false;
        m_ab = calculate_ab();
    }
```

Another solution is to have epoch for each mutable field. Epoch is an int value, which increased each time we mutate value. By storing mutation epoch, and update epoch, we can check does update happens in the same epoch.
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
As you can see, setter move time. But we have one problem here. Since we compare for greater or equal we are sensitive for int overflow. If we would just check for equal, we would not have that problem.
