package core

type StackNode struct {
    Value int
    Below *StackNode
}

type Stack struct {
    Top *StackNode
}

func (stack *Stack) Init() {
    stack.Top = nil
}

func (stack *Stack) Push(value int) {
    node := &StackNode{}
    node.Value = value
    node.Below = stack.Top
    stack.Top = node
}

func (stack *Stack) Pop() (int, bool) {
    if stack.Top == nil {
        return 0, false
    }
    removed := stack.Top
    value := removed.Value
    stack.Top = removed.Below
    return value, true
}
