package main

import (
    "fmt"
    "os"
    "strconv"

    "lab2/core"
)

const tableSize = 103

type HashTable struct {
    Count   int
    Method  int
    Keys    [tableSize]int
    Used    [tableSize]bool
    Removed [tableSize]bool
}

var table HashTable

func fold(key int) int {
    rest := key
    if rest < 0 {
        rest = -rest
    }
    sum := 0
    for rest > 0 {
        sum += rest % 1000
        rest /= 1000
    }
    return sum
}

func foldIndex(key int) int {
    return fold(key) % tableSize
}

func doubleStep(key int) int {
    return 1 + (key % (tableSize - 1))
}

func initTable(table *HashTable) {
    table.Count = 0
    table.Method = 1
    for index := 0; index < tableSize; index++ {
        table.Keys[index] = 0
        table.Used[index] = false
        table.Removed[index] = false
    }
}

func stepFor(table *HashTable, key int) int {
    if table.Method == 2 {
        return doubleStep(key)
    }
    return 1
}

func addKey(table *HashTable, key int, probes *int) bool {
    *probes = 0
    if table.Count >= tableSize {
        return false
    }
    step := stepFor(table, key)
    index := foldIndex(key)
    for table.Used[index] && !table.Removed[index] && table.Keys[index] != key {
        *probes++
        index = (index + step) % tableSize
    }
    if table.Used[index] && !table.Removed[index] && table.Keys[index] == key {
        return false
    }
    table.Keys[index] = key
    table.Used[index] = true
    table.Removed[index] = false
    table.Count++
    *probes++
    return true
}

func findKey(table *HashTable, key int, probes *int) bool {
    *probes = 0
    step := stepFor(table, key)
    index := foldIndex(key)
    for attempt := 0; attempt < tableSize; attempt++ {
        if !table.Used[index] {
            return false
        }
        if !table.Removed[index] && table.Keys[index] == key {
            *probes++
            return true
        }
        *probes++
        index = (index + step) % tableSize
    }
    return false
}

func deleteKey(table *HashTable, key int) bool {
    step := stepFor(table, key)
    index := foldIndex(key)
    for attempt := 0; attempt < tableSize; attempt++ {
        if !table.Used[index] {
            return false
        }
        if !table.Removed[index] && table.Keys[index] == key {
            table.Removed[index] = true
            table.Count--
            return true
        }
        index = (index + step) % tableSize
    }
    return false
}

func readTable(table *HashTable) string {
    result := ""
    first := true
    for index := 0; index < tableSize; index++ {
        if table.Used[index] && !table.Removed[index] {
            if !first {
                result += ","
            }
            result += strconv.Itoa(table.Keys[index])
            first = false
        }
    }
    return result
}

func analyze() {
    seed := uint32(12345)
    generated := make([]int, tableSize)
    generatedCount := 0
    for probe := 0; probe < tableSize; probe++ {
        duplicate := false
        for {
            seed = seed*1103515245 + 12345
            generated[generatedCount] = int(seed % 1000000000)
            duplicate = false
            for index := 0; index < generatedCount; index++ {
                if generated[index] == generated[generatedCount] {
                    duplicate = true
                }
            }
            if !duplicate {
                break
            }
        }
        generatedCount++
    }

    fmt.Printf("load | linear add probes | linear find avg | double add probes | double find avg\n")
    loads := [4]float64{0.3, 0.5, 0.7, 0.9}
    for _, load := range loads {
        amount := int(float64(tableSize) * load)
        linearProbes := 0
        linearFindProbes := 0
        doubleProbes := 0
        doubleFindProbes := 0

        initTable(&table)
        table.Method = 1
        for index := 0; index < amount; index++ {
            probes := 0
            addKey(&table, generated[index], &probes)
            linearProbes += probes
        }
        for index := 0; index < amount; index++ {
            probes := 0
            findKey(&table, generated[index], &probes)
            linearFindProbes += probes
        }

        initTable(&table)
        table.Method = 2
        for index := 0; index < amount; index++ {
            probes := 0
            addKey(&table, generated[index], &probes)
            doubleProbes += probes
        }
        for index := 0; index < amount; index++ {
            probes := 0
            findKey(&table, generated[index], &probes)
            doubleFindProbes += probes
        }

        fmt.Printf("%.1f | %d | %.2f avg | %d | %.2f avg\n",
            load, linearProbes,
            float64(linearFindProbes)/float64(amount),
            doubleProbes,
            float64(doubleFindProbes)/float64(amount))
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
        fmt.Printf("usage: hashtab --file file.data --query 'LADD 523456795' | 'DADD 523456795' | 'ANALYZE'\n")
        os.Exit(1)
    }

    rows := core.ReadFile(file)
    initTable(&table)
    for _, row := range rows {
        if row.Type == 'N' {
            table.Method = core.TextToInt(row.Value)
        }
    }
    for _, row := range rows {
        if row.Type == 'T' {
            for _, part := range core.SplitCsv(row.Value) {
                probes := 0
                addKey(&table, core.TextToInt(part), &probes)
            }
        }
    }

    tokens := core.SplitWords(query, 10)
    command := tokens[0]

    result := ""
    probes := 0

    switch command {
    case "ANALYZE":
        analyze()
        return
    case "FOLD":
        fmt.Printf("-> %d\n", fold(core.TextToInt(tokens[1])))
        return
    }

    methodLetter := command[0]
    keyword := command[1:]
    switch methodLetter {
    case 'L':
        table.Method = 1
    case 'D':
        table.Method = 2
    default:
        fmt.Printf("usage: hashtab --file file.data --query 'LADD key' | 'DADD key' | 'LAT key' | 'DAT key' | 'LDEL key' | 'DDEL key' | 'LREAD' | 'DREAD' | 'FOLD key' | 'ANALYZE'\n")
        os.Exit(1)
    }
    key := 0
    if len(tokens) > 1 {
        key = core.TextToInt(tokens[1])
    }

    switch keyword {
    case "ADD":
        if addKey(&table, key, &probes) {
            result = "TRUE"
        } else {
            result = "FALSE"
        }
    case "AT":
        if findKey(&table, key, &probes) {
            result = "TRUE"
        } else {
            result = "FALSE"
        }
    case "DEL":
        if deleteKey(&table, key) {
            result = "TRUE"
        } else {
            result = "FALSE"
        }
    case "READ":
        result = readTable(&table)
    default:
        fmt.Printf("usage: hashtab --file file.data --query 'LADD key' | 'DADD key' | 'LAT key' | 'DAT key' | 'LDEL key' | 'DDEL key' | 'LREAD' | 'DREAD' | 'FOLD key' | 'ANALYZE'\n")
        os.Exit(1)
    }

    output := make([]core.FileRow, 0)
    output = append(output, core.FileRow{Name: "table", Type: 'T', Value: readTable(&table)})
    output = append(output, core.FileRow{Name: "method", Type: 'N', Value: strconv.Itoa(table.Method)})
    core.WriteFile(file, output)

    if result != "" {
        fmt.Printf("-> %s\n", result)
    }
}
