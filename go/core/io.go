package core

import (
    "fmt"
    "os"
    "strconv"
    "strings"
)

type FileRow struct {
    Name  string
    Type  byte
    Value string
}

func IntToText(value int) string {
    return strconv.Itoa(value)
}

func TextToInt(text string) int {
    value, _ := strconv.Atoi(text)
    return value
}

func SkipSpaces(text string, position *int) {
    for *position < len(text) && (text[*position] == ' ' || text[*position] == '\n' || text[*position] == '\r' || text[*position] == '\t' || text[*position] == ',') {
        *position++
    }
}

func ReadQuoted(text string, position *int) string {
    var result strings.Builder
    if *position < len(text) && text[*position] == '"' {
        *position++
    }
    for *position < len(text) && text[*position] != '"' {
        result.WriteByte(text[*position])
        *position++
    }
    if *position < len(text) && text[*position] == '"' {
        *position++
    }
    return result.String()
}

func ReadFile(path string) []FileRow {
    rows := make([]FileRow, 0)
    data, err := os.ReadFile(path)
    if err != nil {
        return rows
    }
    text := string(data)

    typeNames := make([]string, 0)
    typeValues := make([]byte, 0)

    position := 0
    for position < len(text) && text[position] != '{' {
        position++
    }
    position++
    for position < len(text) {
        SkipSpaces(text, &position)
        if position >= len(text) || text[position] == '}' {
            break
        }
        key := ReadQuoted(text, &position)
        for position < len(text) && text[position] != ':' {
            position++
        }
        position++
        SkipSpaces(text, &position)
        if key == "__types" {
            value := ReadQuoted(text, &position)
            for _, piece := range strings.Split(value, ",") {
                separator := strings.Index(piece, ":")
                if separator >= 0 {
                    typeNames = append(typeNames, piece[:separator])
                    typeValues = append(typeValues, piece[separator+1])
                }
            }
        } else if len(rows) < 48 {
            row := FileRow{}
            row.Name = key
            row.Type = '?'
            for index := range typeNames {
                if typeNames[index] == key {
                    row.Type = typeValues[index]
                }
            }
            row.Value = ReadQuoted(text, &position)
            rows = append(rows, row)
        }
    }
    return rows
}

func WriteFile(path string, rows []FileRow) {
    file, err := os.Create(path)
    if err != nil {
        return
    }
    defer file.Close()

    fmt.Fprintf(file, "{\"__types\": \"")
    for index, row := range rows {
        if index > 0 {
            fmt.Fprintf(file, ",")
        }
        fmt.Fprintf(file, "%s:%c", row.Name, row.Type)
    }
    fmt.Fprintf(file, "\"")
    for _, row := range rows {
        fmt.Fprintf(file, ", \"%s\": \"%s\"", row.Name, row.Value)
    }
    fmt.Fprintf(file, "}")
}

func SplitCsv(text string) []string {
    parts := make([]string, 0)
    current := ""
    for _, symbol := range []byte(text) {
        if symbol == ',' {
            if current != "" {
                parts = append(parts, current)
            }
            current = ""
        } else {
            current += string(symbol)
        }
    }
    if current != "" {
        parts = append(parts, current)
    }
    return parts
}

func SplitWords(text string, limit int) []string {
    words := make([]string, 0)
    current := ""
    for _, symbol := range []byte(text) {
        if symbol == ' ' || symbol == '\t' {
            if current != "" && len(words) < limit {
                words = append(words, current)
            }
            current = ""
        } else {
            current += string(symbol)
        }
    }
    if current != "" && len(words) < limit {
        words = append(words, current)
    }
    return words
}
