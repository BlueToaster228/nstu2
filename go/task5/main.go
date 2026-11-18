package main

import (
    "fmt"
    "os"
    "strings"

    "lab2/core"
)

const symbolTextSize = 257
const codeTextSize = 1024

type HuffmanNode struct {
    Weight  int
    Symbols []byte
    Left    *HuffmanNode
    Right   *HuffmanNode
}

var nodes []*HuffmanNode
var active []bool
var frequency [256]int
var codes [256]string

func takeNode() *HuffmanNode {
    node := &HuffmanNode{}
    node.Symbols = make([]byte, 0, symbolTextSize)
    nodes = append(nodes, node)
    active = append(active, true)
    return node
}

func findMinimum() int {
    best := -1
    for index := range nodes {
        if !active[index] {
            continue
        }
        if best < 0 || nodes[index].Weight < nodes[best].Weight {
            best = index
        }
    }
    return best
}

func assignCodes(node *HuffmanNode, path []byte, depth int) {
    if node.Left == nil && node.Right == nil {
        code := string(path[:depth])
        if depth == 0 {
            codes[node.Symbols[0]] = "0"
        } else {
            codes[node.Symbols[0]] = code
        }
        return
    }
    path[depth] = '0'
    assignCodes(node.Left, path, depth+1)
    path[depth] = '1'
    assignCodes(node.Right, path, depth+1)
}

func buildTree() {
    for symbol := 0; symbol < 256; symbol++ {
        if frequency[symbol] > 0 {
            leaf := takeNode()
            leaf.Weight = frequency[symbol]
            leaf.Symbols = append(leaf.Symbols, byte(symbol))
        }
    }
    if len(nodes) == 0 {
        return
    }
    for {
        first := findMinimum()
        if first < 0 {
            break
        }
        active[first] = false
        second := findMinimum()
        if second < 0 {
            active[first] = true
            break
        }
        active[second] = false
        parent := takeNode()
        parent.Weight = nodes[first].Weight + nodes[second].Weight
        parent.Symbols = append(parent.Symbols, nodes[first].Symbols...)
        parent.Symbols = append(parent.Symbols, nodes[second].Symbols...)
        parent.Left = nodes[first]
        parent.Right = nodes[second]
    }
    path := make([]byte, codeTextSize)
    root := -1
    for index := range nodes {
        if active[index] {
            root = index
        }
    }
    assignCodes(nodes[root], path, 0)
}

func buildTable() string {
    var output strings.Builder
    for symbol := 0; symbol < 256; symbol++ {
        if frequency[symbol] > 0 {
            if output.Len() > 0 {
                output.WriteString(",")
            }
            output.WriteString(fmt.Sprintf("%c:%d:%s", symbol, frequency[symbol], codes[symbol]))
        }
    }
    return output.String()
}

func encodeText(text string) string {
    var output strings.Builder
    for _, symbol := range []byte(text) {
        output.WriteString(codes[symbol])
    }
    return output.String()
}

func decodeCode(code string) (string, bool) {
    var output strings.Builder
    if len(nodes) == 0 {
        return "", false
    }
    root := -1
    for index := range nodes {
        if active[index] {
            root = index
        }
    }
    if root < 0 {
        return "", false
    }
    current := nodes[root]
    for _, symbol := range []byte(code) {
        if current.Left == nil && current.Right == nil {
            output.Write(current.Symbols)
            current = nodes[root]
        }
        if symbol == '0' {
            current = current.Left
        } else {
            current = current.Right
        }
        if current == nil {
            return "", false
        }
    }
    if current.Left == nil && current.Right == nil {
        output.Write(current.Symbols)
        return output.String(), true
    }
    return "", false
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
        fmt.Printf("usage: huffman --file file.data --query 'HENC text' | 'HDEC'\n")
        os.Exit(1)
    }

    rows := core.ReadFile(file)
    storedText := ""
    storedCode := ""
    storedTable := ""
    for _, row := range rows {
        if row.Type == 'X' {
            storedText = row.Value
        } else if row.Type == 'C' {
            storedCode = row.Value
        } else if row.Type == 'B' {
            storedTable = row.Value
        }
    }

    tokens := core.SplitWords(query, 16)

    switch tokens[0] {
    case "HENC":
        text := strings.Join(tokens[1:], " ")
        if text == "" {
            text = storedText
        }
        for index := range frequency {
            frequency[index] = 0
        }
        nodes = nil
        active = nil
        for _, symbol := range []byte(text) {
            frequency[symbol]++
        }
        buildTree()
        table := buildTable()
        code := encodeText(text)
        fmt.Printf("-> %s\n", code)
        fmt.Printf("table: %s\n", table)
        storedText = text
        storedCode = code
        storedTable = table
        output := make([]core.FileRow, 0)
        output = append(output, core.FileRow{Name: "text", Type: 'X', Value: storedText})
        output = append(output, core.FileRow{Name: "code", Type: 'C', Value: storedCode})
        output = append(output, core.FileRow{Name: "table", Type: 'B', Value: storedTable})
        core.WriteFile(file, output)
        return
    case "HDEC":
        for index := range frequency {
            frequency[index] = 0
        }
        nodes = nil
        active = nil
        for _, piece := range strings.Split(storedTable, ",") {
            if piece == "" {
                continue
            }
            fields := strings.SplitN(piece, ":", 3)
            if len(fields) < 2 {
                continue
            }
            if fields[0] == "" {
                continue
            }
            frequency[fields[0][0]] = core.TextToInt(fields[1])
        }
        buildTree()
        decoded, ok := decodeCode(storedCode)
        if !ok {
            fmt.Printf("-> ERROR\n")
            return
        }
        fmt.Printf("-> %s\n", decoded)
        return
    default:
        fmt.Printf("usage: huffman --file file.data --query 'HENC text' | 'HDEC'\n")
        os.Exit(1)
    }
}
