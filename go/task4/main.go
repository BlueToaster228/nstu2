package main

import (
    "fmt"
    "os"
    "strconv"
    "strings"

    "lab2/core"
)

const subArrayMax = 5000

var printed [subArrayMax]string
var printedCount = 0

func alreadyPrinted(text string) bool {
    for index := 0; index < printedCount; index++ {
        if printed[index] == text {
            return true
        }
    }
    return false
}

func main() {
    file := ""
    for index := 1; index < len(os.Args); index++ {
        if os.Args[index] == "--file" && index+1 < len(os.Args) {
            file = os.Args[index+1]
            index++
        }
    }
    if file == "" {
        fmt.Printf("usage: subarrays --file file.data --query 'SUBS'\n")
        os.Exit(1)
    }

    rows := core.ReadFile(file)
    var array core.Array
    array.Init()
    for _, row := range rows {
        if row.Type != 'M' {
            continue
        }
        for _, part := range core.SplitCsv(row.Value) {
            array.InsertEnd(core.TextToInt(part))
        }
    }

    fmt.Printf("-> [")
    first := true
    if !alreadyPrinted("{}") {
        printed[printedCount] = "{}"
        printedCount++
        fmt.Printf("{}")
        first = false
    }
    for start := 0; start < array.Count; start++ {
        for end := start; end < array.Count; end++ {
            var piece strings.Builder
            piece.WriteString("{")
            for position := start; position <= end; position++ {
                if position > start {
                    piece.WriteString(",")
                }
                piece.WriteString(strconv.Itoa(array.Elements[position]))
            }
            piece.WriteString("}")
            if alreadyPrinted(piece.String()) {
                continue
            }
            printed[printedCount] = piece.String()
            printedCount++
            if !first {
                fmt.Printf(", ")
            }
            fmt.Printf("%s", piece.String())
            first = false
        }
    }
    fmt.Printf("]\n")
}
