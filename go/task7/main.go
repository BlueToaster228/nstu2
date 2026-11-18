package main

import (
    "fmt"
    "os"
    "strconv"
    "strings"

    "lab2/core"
)

type LruNode struct {
    Key   int
    Value int
    Prev  *LruNode
    Next  *LruNode
}

type LruCache struct {
    Capacity int
    Count    int
    Head     *LruNode
    Tail     *LruNode
}

var nodes []*LruNode

func takeNode() *LruNode {
    node := &LruNode{}
    nodes = append(nodes, node)
    return node
}

func initCache(cache *LruCache, capacity int) {
    cache.Capacity = capacity
    cache.Count = 0
    cache.Head = nil
    cache.Tail = nil
}

func moveToHead(cache *LruCache, node *LruNode) {
    if cache.Head == node {
        return
    }
    if node.Prev != nil {
        node.Prev.Next = node.Next
    }
    if node.Next != nil {
        node.Next.Prev = node.Prev
    }
    if cache.Tail == node {
        cache.Tail = node.Prev
    }
    node.Prev = nil
    node.Next = cache.Head
    if cache.Head != nil {
        cache.Head.Prev = node
    }
    cache.Head = node
    if cache.Tail == nil {
        cache.Tail = node
    }
}

func findNode(cache *LruCache, key int) *LruNode {
    current := cache.Head
    for current != nil {
        if current.Key == key {
            return current
        }
        current = current.Next
    }
    return nil
}

func putValue(cache *LruCache, key int, value int) {
    existing := findNode(cache, key)
    if existing != nil {
        existing.Value = value
        moveToHead(cache, existing)
        return
    }
    if cache.Capacity <= 0 {
        return
    }
    if cache.Count == cache.Capacity {
        victim := cache.Tail
        if victim.Prev != nil {
            victim.Prev.Next = nil
        }
        cache.Tail = victim.Prev
        if cache.Head == victim {
            cache.Head = nil
        }
        cache.Count--
    }
    node := takeNode()
    node.Key = key
    node.Value = value
    node.Next = cache.Head
    if cache.Head != nil {
        cache.Head.Prev = node
    }
    cache.Head = node
    if cache.Tail == nil {
        cache.Tail = node
    }
    cache.Count++
}

func getValue(cache *LruCache, key int) int {
    node := findNode(cache, key)
    if node == nil {
        return -1
    }
    moveToHead(cache, node)
    return node.Value
}

func readCache(cache *LruCache) string {
    result := ""
    current := cache.Head
    for current != nil {
        if current != cache.Head {
            result += ","
        }
        result += strconv.Itoa(current.Key) + ":" + strconv.Itoa(current.Value)
        current = current.Next
    }
    return result
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
        fmt.Printf("usage: lru --file file.data --query 'CAP 2 SET 1 2 GET 1'\n")
        os.Exit(1)
    }

    rows := core.ReadFile(file)
    var cache LruCache
    initCache(&cache, 2)
    for _, row := range rows {
        if row.Type != 'R' {
            continue
        }
        parts := core.SplitCsv(row.Value)
        for position := len(parts) - 1; position >= 0; position-- {
            pair := strings.SplitN(parts[position], ":", 2)
            if len(pair) < 2 {
                continue
            }
            putValue(&cache, core.TextToInt(pair[0]), core.TextToInt(pair[1]))
        }
    }

    tokens := core.SplitWords(query, 64)
    result := ""
    position := 0
    for position < len(tokens) {
        command := tokens[position]
        if command == "CAP" && position+1 < len(tokens) {
            cache.Capacity = core.TextToInt(tokens[position+1])
            if cache.Count > cache.Capacity && cache.Capacity > 0 {
                for cache.Count > cache.Capacity && cache.Tail != nil {
                    victim := cache.Tail
                    cache.Tail = victim.Prev
                    if cache.Tail != nil {
                        cache.Tail.Next = nil
                    }
                    if cache.Head == victim {
                        cache.Head = nil
                    }
                    cache.Count--
                }
            }
            position += 2
        } else if command == "SET" && position+2 < len(tokens) {
            putValue(&cache, core.TextToInt(tokens[position+1]), core.TextToInt(tokens[position+2]))
            position += 3
        } else if command == "GET" && position+1 < len(tokens) {
            result += strconv.Itoa(getValue(&cache, core.TextToInt(tokens[position+1]))) + " "
            position += 2
        } else {
            position++
        }
    }

    output := make([]core.FileRow, 0)
    output = append(output, core.FileRow{Name: "cache", Type: 'R', Value: readCache(&cache)})
    core.WriteFile(file, output)

    if result != "" {
        fmt.Printf("-> %s\n", result)
    }
}
