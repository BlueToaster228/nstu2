package core

import (
    "strconv"
    "strings"
)

type BitSet struct {
    Regs [4]uint64
}

func (set *BitSet) Init() {
    for index := 0; index < 4; index++ {
        set.Regs[index] = 0
    }
}

func (set *BitSet) Add(value byte) {
    reg := value / 64
    set.Regs[reg] |= 1 << (value % 64)
}

func (set *BitSet) Remove(value byte) {
    reg := value / 64
    set.Regs[reg] &^= 1 << (value % 64)
}

func (set *BitSet) Contains(value byte) bool {
    reg := value / 64
    return set.Regs[reg]&(1<<(value%64)) != 0
}

func (set *BitSet) Count() int {
    total := 0
    for index := 0; index < 4; index++ {
        reg := set.Regs[index]
        for reg != 0 {
            reg = reg & (reg - 1)
            total++
        }
    }
    return total
}

func (set *BitSet) Read() string {
    var output strings.Builder
    first := true
    for value := 0; value < 256; value++ {
        if set.Contains(byte(value)) {
            if !first {
                output.WriteString(",")
            }
            output.WriteString(strconv.Itoa(value))
            first = false
        }
    }
    return output.String()
}

func (set *BitSet) NextElement(from byte) (byte, bool) {
    for value := int(from); value < 256; value++ {
        if set.Contains(byte(value)) {
            return byte(value), true
        }
    }
    return 0, false
}
