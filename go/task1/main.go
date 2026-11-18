package main

import (
    "fmt"
    "os"
    "strconv"

    "lab2/core"
)

func main() {
    query := ""
    for index := 1; index < len(os.Args); index++ {
        if os.Args[index] == "--query" && index+1 < len(os.Args) {
            query = os.Args[index+1]
            index++
        }
    }
    if query == "" {
        fmt.Printf("usage: baseball --query '5 2 C D +'\n")
        os.Exit(1)
    }

    tokens := core.SplitWords(query, 16)

    var stack core.Stack
    stack.Init()
    total := 0
    failed := false

    for index := 0; index < len(tokens) && !failed; index++ {
        token := tokens[index]
        switch token {
        case "+":
            first, ok1 := stack.Pop()
            second, ok2 := stack.Pop()
            if !ok1 || !ok2 {
                failed = true
                break
            }
            stack.Push(first)
            stack.Push(second)
            stack.Push(first + second)
        case "D":
            top, ok := stack.Pop()
            if !ok {
                failed = true
                break
            }
            stack.Push(top)
            stack.Push(top * 2)
        case "C":
            _, ok := stack.Pop()
            if !ok {
                failed = true
            }
        default:
            value, _ := strconv.Atoi(token)
            stack.Push(value)
        }
    }

    if failed {
        fmt.Printf("-> ERROR\n")
        return
    }
    for {
        value, ok := stack.Pop()
        if !ok {
            break
        }
        total += value
    }
    fmt.Printf("-> %d\n", total)
}
