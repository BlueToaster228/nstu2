package main

import (
    "fmt"
    "os"

    "lab2/core"
)

var setNames []string
var sets []core.BitSet

func findName(wanted string) int {
    for index := range setNames {
        if setNames[index] == wanted {
            return index
        }
    }
    return -1
}

func takeName(wanted string) (int, bool) {
    created := false
    index := findName(wanted)
    if index < 0 {
        setNames = append(setNames, wanted)
        sets = append(sets, core.BitSet{})
        index = len(setNames) - 1
        created = true
    }
    return index, created
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
        fmt.Printf("usage: set --file file.data --query 'SETADD name value'\n")
        os.Exit(1)
    }

    rows := core.ReadFile(file)
    for _, row := range rows {
        if row.Type != 'E' {
            continue
        }
        parts := core.SplitCsv(row.Value)
        slot, created := takeName(row.Name)
        if created {
            sets[slot].Init()
        }
        for _, part := range parts {
            sets[slot].Add(byte(core.TextToInt(part)))
        }
    }

    tokens := core.SplitWords(query, 10)
    command := tokens[0]
    target := tokens[1]

    result := ""
    slot, created := takeName(target)
    if created {
        sets[slot].Init()
    }
    set := &sets[slot]

    argument := ""
    if len(tokens) > 2 {
        argument = tokens[2]
    }
    switch command {
    case "SETADD":
        set.Add(byte(core.TextToInt(argument)))
    case "SETDEL":
        set.Remove(byte(core.TextToInt(argument)))
    case "SET_AT":
        if set.Contains(byte(core.TextToInt(argument))) {
            result = "TRUE"
        } else {
            result = "FALSE"
        }
    case "SETREAD":
        result = set.Read()
    }

    output := make([]core.FileRow, 0)
    for index := range sets {
        output = append(output, core.FileRow{Name: setNames[index], Type: 'E', Value: sets[index].Read()})
    }
    core.WriteFile(file, output)

    if result != "" {
        fmt.Printf("-> %s\n", result)
    }
}
