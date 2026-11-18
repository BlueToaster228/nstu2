package main

import (
    "fmt"
    "os"

    "lab2/core"
)

var elements [256]int
var elementCount = 0
var used [256]bool
var buckets [256][256]int
var bucketSizes [256]int
var bucketCount = 0
var solutionFound = false

func fillBucket(bucketIndex int, currentSum int, startIndex int, target int) {
    if solutionFound {
        return
    }
    if currentSum == target {
        if bucketIndex+1 == bucketCount {
            solutionFound = true
        }
        if !solutionFound {
            fillBucket(bucketIndex+1, 0, 0, target)
        }
        return
    }
    for index := startIndex; index < elementCount && !solutionFound; index++ {
        if used[index] {
            continue
        }
        if currentSum+elements[index] > target {
            continue
        }
        used[index] = true
        buckets[bucketIndex][bucketSizes[bucketIndex]] = elements[index]
        bucketSizes[bucketIndex]++
        fillBucket(bucketIndex, currentSum+elements[index], index+1, target)
        if !solutionFound {
            bucketSizes[bucketIndex]--
            used[index] = false
        }
    }
}

func main() {
    file := ""
    query := ""
    for index := 1; index < len(os.Args); index++ {
        if os.Args[index] == "--file" && index+1 < len(os.Args) {
            file = os.Args[index+1]
            index++
        } else if os.Args[index] == "--query" && index+1 < len(os.Args) {
            query = os.Args[index+1]
            index++
        }
    }
    if file == "" || query == "" {
        fmt.Printf("usage: split --file file.data --query 'SPLIT 15'\n")
        os.Exit(1)
    }

    rows := core.ReadFile(file)
    var set core.BitSet
    set.Init()
    for _, row := range rows {
        if row.Type != 'E' {
            continue
        }
        for _, part := range core.SplitCsv(row.Value) {
            set.Add(byte(core.TextToInt(part)))
        }
    }

    tokens := core.SplitWords(query, 8)
    if tokens[0] != "SPLIT" {
        fmt.Printf("usage: split --file file.data --query 'SPLIT 15'\n")
        os.Exit(1)
    }
    target := core.TextToInt(tokens[1])

    for value := 0; value < 256; value++ {
        if set.Contains(byte(value)) {
            elements[elementCount] = value
            elementCount++
        }
    }

    total := 0
    for index := 0; index < elementCount; index++ {
        total += elements[index]
    }
    if target <= 0 || total%target != 0 {
        fmt.Printf("-> ERROR\n")
        return
    }
    bucketCount = total / target
    if bucketCount == 0 || bucketCount > elementCount {
        fmt.Printf("-> ERROR\n")
        return
    }
    for index := 0; index < bucketCount; index++ {
        bucketSizes[index] = 0
    }
    for index := 0; index < elementCount; index++ {
        used[index] = false
    }
    solutionFound = false
    fillBucket(0, 0, 0, target)
    if !solutionFound {
        fmt.Printf("-> ERROR\n")
        return
    }

    fmt.Printf("-> ")
    for bucket := 0; bucket < bucketCount; bucket++ {
        fmt.Printf("{")
        for position := 0; position < bucketSizes[bucket]; position++ {
            if position > 0 {
                fmt.Printf(",")
            }
            fmt.Printf("%d", buckets[bucket][position])
        }
        fmt.Printf("} ")
    }
    fmt.Printf("\n")
}
