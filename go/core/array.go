package core

const Capacity = 1024

type Array struct {
    Size     int
    Count    int
    Elements [Capacity]int
}

func (array *Array) Init() {
    array.Size = Capacity
    array.Count = 0
}

func (array *Array) InsertEnd(value int) {
    if array.Count >= array.Size {
        return
    }
    array.Elements[array.Count] = value
    array.Count++
}
