

<table>
<tr> <th>with dirty-flags</th> <th>with Epochs</th> </tr>
<tr>
<td valign="top">

<pre lang="cpp">
Epoch epoch;

Epoch save_epoch;
void save(){
    if (save_epoch == epoch) return;
    save_epoch = epoch;

    do_save();
}
</pre>

</td>
<td valign="top">

<pre lang="cpp">
class SettingsWindow{
    Epoch epoch;
    Epoch save_epoch;
    Epoch canvas_update_epoch;

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
</pre>

</td>
</tr>
</table>