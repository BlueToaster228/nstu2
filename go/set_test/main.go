package main

import (
    "fmt"

    "lab2/core"
)

func main() {
    var set core.BitSet
    set.Init()
    set.Add(0)
    set.Add(5)
    set.Add(100)
    set.Add(255)
    set.Add(5)

    fmt.Printf("read:    -> %s\n", set.Read())
    fmt.Printf("count:   -> %d\n", set.Count())
    fmt.Printf("has 5:   -> %s\n", boolText(set.Contains(5)))
    fmt.Printf("has 7:   -> %s\n", boolText(set.Contains(7)))
    fmt.Printf("has 0:   -> %s\n", boolText(set.Contains(0)))
    fmt.Printf("has 255: -> %s\n", boolText(set.Contains(255)))

    set.Remove(100)
    set.Remove(0)
    fmt.Printf("has 100 after remove: -> %s\n", boolText(set.Contains(100)))
    fmt.Printf("has 0 after remove:   -> %s\n", boolText(set.Contains(0)))
    fmt.Printf("read:    -> %s\n", set.Read())
    fmt.Printf("count:   -> %d\n", set.Count())

    if found, ok := set.NextElement(6); ok {
        fmt.Printf("next after 6: -> %d\n", found)
    } else {
        fmt.Printf("next after 6: -> none\n")
    }
    if found, ok := set.NextElement(250); ok {
        fmt.Printf("next after 250: -> %d\n", found)
    } else {
        fmt.Printf("next after 250: -> none\n")
    }
}

func boolText(value bool) string {
    if value {
        return "TRUE"
    }
    return "FALSE"
}
