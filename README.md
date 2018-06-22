```c++
class Settings{
    Epoch epoch;
public:
    void set_resolution(){
        epoch++;
    }
    void set_gamma(){
        epoch++;
    }
    void set_anything(){
        epoch++;
    }

    Epoch get_epoch() const{
        return epoch;
    }
};


Settings settings;


// each 5 min check for settings changed, if yes - save
Epoch settings_autosave_epoch;
if (settings_autosave_epoch.update(settings.get_epoch())){
    save();
}
```

---

Pay attention on setters.

<table>
<tr> <th>with dirty-flags</th> <th>with Epochs</th> </tr>
<tr>
<td valign="top">

<pre lang="cpp">
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
        a = point;
        ab_dirty = true;
        ac_dirty = true;
    }
    void set_b(Point point){
        b = point;
        ab_dirty = true;
        bc_dirty = true;
    }
    void set_c(Point point){
        c = point;
        ac_dirty = true;
        bc_dirty = true;
    }

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
</pre>

</td>
<td valign="top">

<pre lang="cpp">
class Triangle{
    Epoch a_epoch, b_epoch, c_epoch;
    Point a,b,c;

    EpochTuple<Epoch, Epoch> ab_epoch, ac_epoch, bc_epoch;
    float m_ab, m_bc, m_ac;

    float calculate_ab();
    float calculate_bc();
    float calculate_ac();
public:
    void set_a(Point point){
        a = point;
        a_epoch++;
    }
    void set_b(Point point){
        b = point;
        b_epoch++;
    }
    void set_c(Point point){
        c = point;
        c_epoch++;
    }

    float ab(){
        if (ab_epoch.update(a_epoch, b_epoch)){
            m_ab = calculate_ab();
        }
        return m_ab;
    }
    float bc();
    float ac();
};
</pre>

</td>
</tr>
</table>


---

<table>
<tr> <th>with dirty-flags</th> <th>with Epochs</th> </tr>
<tr>
<td valign="top">

<pre lang="cpp">
class SettingsWindow{
    bool save_dirty   = false;
    bool canvas_dirty = false;

    void rise_dirty_flags(){
        // O(n)
        save_dirty   = true;
        canvas_dirty = true;
    }
public:
    void set_resolution(){
        rise_dirty_flags();
    }
    void set_gamma(){
        rise_dirty_flags();
    }
    void set_anything(){
        rise_dirty_flags();
    }

    void draw(){
        if (canvas_dirty){
            canvas_dirty = false;
            redraw_canvas();
        }
    }

    void SaveClk(){
        if (save_dirty){
            save_dirty = false;
            save();
        }
    }
};
</pre>

</td>
<td valign="top">

<pre lang="cpp">
class SettingsWindow{
    Epoch epoch;
    Epoch save_epoch;
    Epoch canvas_update_epoch;

public:
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
</pre>

</td>
</tr>
</table>