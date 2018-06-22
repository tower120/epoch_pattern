## API

### Epoch

Wrapper around `unigned int`.

* `operator++()`
* `operator==`
* `operator!=`
* `bool update(const Epoch& other)` - return `true` if update is needed. (other != *this)


<table>
<tr> <th>without update</th> <th>with update</th> </tr>
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
Epoch epoch;

Epoch save_epoch;
void save(){
    if (!save_epoch.update(epoch)) return;

    do_save();
}
</pre>

</td>
</tr>
</table>

* `auto count()` - return inner int value.
* `static Epoch Pre()` - construct `Epoch` with value smaller then default constructed. You may need this, if you want your update trigger always at first call:
```c++
Epoch epoch;

Epoch save_epoch = Epoch::Pre();
void save(){
    // will trigger at first call
    if (!save_epoch.update(epoch)) return;

    do_save();
}
```


### EpochTupleView

Helper for tuple of Epoch's. Constructed with global function `EpochTupleView epoch_tuple(std::tuple<Epochs>...)`

* `bool update(const Epochs&...)` - return `true` if update is needed.
* `auto guard(const Epochs&...)`  - for early return / fast fail. See below.

without:

```c++
Epoch epoch;
Epoch a_epoch;
Epoch b_epoch;

std::tuple<Epoch, Epoch, Epoch> update_epoch;
void update_ab(){
    if (std::get<0>(update_epoch) == epoch) return;
    if (std::get<1>(update_epoch) == a_epoch
        && std::get<2>(update_epoch) == b_epoch) return;

    update_epoch = {epoch, a_epoch, b_epoch};

    do_update();
}
```

with:

```c++
Epoch epoch;
Epoch a_epoch;
Epoch b_epoch;

std::tuple<Epoch, Epoch, Epoch> update_epoch;
void update_ab(){
    if (!epoch_tuple(update_epoch)
                .guard(epoch)
                .update(a_epoch, b_epoch)) return;

    do_update();
}
```

---

It is recommended to inherit `Epoch`, this way you'll have more type safety, and compiler assisted checks:

```c++
Epoch epoch;
struct AEpoch : Epoch{} a_epoch;
struct BEpoch : Epoch{} b_epoch;

std::tuple<Epoch, AEpoch, BEpoch> update_epoch;
void update_ab(){
    // this is compiler error! types don't match
    if (!epoch_tuple(update_epoch)
                .guard(epoch)
                .update(b_epoch, a_epoch)) return;

    // ok
    if (!epoch_tuple(update_epoch)
                .guard(epoch)
                .update(a_epoch, b_epoch)) return;

    do_update();
}
```

### EpochTuple

`std::tuple<Epochs...>` + `EpochTupleView` :

```c++
Epoch epoch;
Epoch a_epoch;
Epoch b_epoch;

EpochTuple<Epoch, Epoch, Epoch> update_epoch;
void update_ab(){
    if (!update_epoch
        .guard(epoch)
        .update(a_epoch, b_epoch)) return;

    do_update();
}
```