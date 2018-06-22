

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