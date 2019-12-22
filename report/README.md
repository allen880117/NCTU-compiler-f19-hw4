# 作業 4 報告

> 學生：王祥任 Xiang-Ren Wang
>
> 學號：0616309

## 概要

繼作業三完成 AST 後，要利用此 AST 來建立 Symbol Table ，並利用此 Symbol Table 來進行 Semantic Check ， 完成語意檢查的最後一環。

* **`[BONUS] Preserving symbol tables for multi-pass`**
* **`[BONUS] No memory leak`**

## 功能

* 編譯 `cd src/ && make clean && make`

* 執行 `./parser [input file]`

* 偽註解 `//D+` `//D-`

    > 控制是否需要列印 Symbol Table 至 `stdout` 。

若語意沒有任何問題，輸出以下訊息：

```cpp
|---------------------------------------------|
|  There is no syntactic and semantic error!  |
|---------------------------------------------|
```

若語意有任何問題，則會依照問題輸出相應訊息至`stderr`，例如：

```cpp
<Error> Found in line x, column y: <error message>
    <source code>
    <notation>
```

## 既有文件的修改

### scanner.l

新增偽註解 `//D+` `//D-` 。

```yacc
%{
    ...
static int32_t OptSrc = 1;
static int32_t OptTok = 1;
int32_t        OptDum = 1; // Not static
    ...
%}
```

```yacc
    /* Pseudocomment */
"//&"[STD][+-].* {
    LIST;
    char option = yytext[3];
    switch (option) {
    case 'S':
        OptSrc = (yytext[4] == '+') ? 1 : 0;
        break;
    case 'T':
        OptTok = (yytext[4] == '+') ? 1 : 0;
        break;
    case 'D':
        OptDum = (yytext[4] == '+') ? 1 : 0;
        break;
    }
}
```

### parser.y

新增偽註解 `//D+` `//D-` 的支援。

```lex
%{
    ...
/* Declared by scanner.l */
extern int32_t LineNum;
extern char Buffer[512];
extern int32_t OptDum;    // From scanner.l
    ...
}%
```

新增 Semantic Analyzer 和相應功能的支援。

```lex
SemanticAnalyzer semantic_analyzer(string(argv[1]), fp);
AST->accept(semantic_analyzer);

if(OptDum == 1)
    semantic_analyzer.dump_symbol_table();

if(semantic_analyzer.is_semantic_error() == 0)
    printf("\n"
        "|---------------------------------------------|\n"
        "|  There is no syntactic and semantic error!  |\n"
        "|---------------------------------------------|\n");
else
    semantic_analyzer.output_err_msg();
```

### AST/ast.hpp

修改 `ASTNodeBase`

* 增加子型別共有變數
* 增加部分跨子型別傳遞用變數

```cpp
class ASTNodeBase
{
    public:
        int line_number; // NEW 共有
        int col_number;  // NEW 共有
        string name;     // NEW 傳遞用

    public:
        virtual void accept(class ASTVisitorBase &v) = 0;
        virtual ~ASTNodeBase(){};
};
```

### AST Nodes

修改 `所有繼承自 ASTNodeBase 的型別`

* 移除原先各自宣告的共有變數。
* 視需求於建構子中賦值予傳遞用變數。

```cpp
class VariableReferenceNode : public ASTNodeBase
{
    public:
        //int line_number; // REMOVE
        //int col_number;  // REMOVE
        string variable_name;
        NodeList* expression_node_list; // indices

    ...
};
```

```cpp
VariableReferenceNode::VariableReferenceNode(
    int _line_number,
    int _col_number,
    string _variable_name,
    NodeList* _expression_node_list
    ){
        this->line_number = _line_number;
        this->col_number = _col_number;
        this->variable_name = _variable_name;
        this->expression_node_list = _expression_node_list;

        this->name = _variable_name; // HW4 ADD // From Base Class
    }
```

### AST/function.cpp

訂正 `FunctionNode` 建構子錯誤

```cpp
FunctionNode::FunctionNode(
    ...
    string _end_name,
    vector<VariableInfo*> _prototype
    ){
        ...
        this->end_name = _end_name;   // Forget to add at HW3
        this->prototype = _prototype;
    }
```

## Symbol Table

請容許我直接分段節錄 `Spec` 中的敘述。

* 每一個條目都是 `帶有屬性的 identifier` 。
* 在被宣告時放入 Symbol Table 中。
* 依照宣告的順序放入。
* 輸出至`stdout`。

|Field|Description (節錄注意重點)|
|:-:|:-|
|Name|-- 長度不超過32字|
|Kind|-- `program`、`function`、`parameter`、`variable`、`loop_var`、`constant`。|
|Level|-- 0 為 Global ， 1 以上(含)為 Local 。<br>-- Level 在進入 scope 時上升，離開 scope 時下降。|
|Type|-- `integer`、`real`、`boolean`、`string`、`the signature of an array`。 <br>-- 可被用於 function 的 return type。<br>-- program 的 return type 固定為 `void` 。|
|Attribute|-- 常數值。<br>-- list of the types of the formal parameters of a function。|

## Semantic Definition

此段是為了幫助我理清每一種語意的注意重點而加的，算是一種實作紀錄。

同樣請容許我直接分段節錄 `Spec` 中的敘述。

### 注意事項

一旦在某節點的子節點發生錯誤，回到某節點本身時，不需檢查某節點和子節點相關的錯誤。因為已經發生錯誤，再檢查也沒有意義。

但不相關的部分還是需要檢查的。

### Program Unit

#### program

* `end`後的 `id` 必須和宣告時的 `id` 相同。
* `id` 必須和 `file name` 相同。
* 因為沒有`return type` ，`program` 裡是不能 `return` 的。

#### function

* `end`後的 `id` 必須和宣告時的 `id` 相同。
* 回傳值型別必須和 `return type` 相同。

### Scope Rules

#### Id & Scope

* 同一 `Scope` 中， `id` 的 `name` 必須是唯一的。
* `id` 使用最靠近的 `Scope` 中的。

#### Compound Statement

* `Compound Statement` 形成一個 `Inner Scope`。
* `Inner Scope` 中的宣告在離開 `Compound Statement` 後刪除。

#### loop_var

* 會存在只有一個`loop_var`的`Scope`。
* `loop_var`不能在其`Scope`和其`Inner Scope`被再次宣告。

### Variable Declaration and Reference

#### Array Declaration

* `Array Declration` 中，`Lower Bound <= Upper Bound` 。
* **如果 `Array` 宣告有錯，當不須訪問此 `Array` 的 `VariableReferenceNode`。**

#### Array Index

* `Index of Array Declaraton` 型別必須是 `Integer` 。
* **如果前述者有錯，當不須檢查參考此者的節點。**
* 請從左至右檢查型別。
* 不須檢查上、下界合理性。
* `Array Reference` 不能超過其最大維度。

### Expression

#### arithmetic operator (+, -, *, or /)

* `LHS` 和 `RHS` 都必須是 `Integer` 或 `Real` 。
* 結果型別為 `Integer` 或 `Real` 。

    ```cpp
    I aop I -> I;
    I aop R -> R;
    R aop I -> R;
    R aop R -> R;
    ```

#### **mod**

* `LHS` 和 `RHS` 都必須是 `Integer` 。
* 結果型別為 `Integer` 。

    ```cpp
    I mod I -> I;
    ```

#### Boolean operator (**and**, **or**, or **not**)

* `LHS` 和 `RHS` 都必須是 `Boolean` 。
* 結果型別為 `Boolean` 。

    ```cpp
    B bop B -> B;
    ```

#### relational operator (<, <=, =, >=, >, or <>)

* `LHS` 和 `RHS` 都必須是 `Integer` 或 `Real` 。
* 結果型別為 `Boolean` 。

    ```cpp
    I rop I -> B;
    I rop R -> B;
    R rop I -> B;
    R rop R -> B;
    ```

#### string

* **可用 Operator 只有 "+"**
* `LHS` 和 `RHS` 都必須是 `String` 。
* 結果型別為 `String` 。

    ```cpp
    STR + STR -> STR
    ```

### Type Coercion and Comparison

* 在某些情況下， `Integer` 可以轉 `Real` 。

    1. Assignment
    2. Parameter Passing
    3. Arithmetic Expression
    4. etc.

* 在合法的 `Arithmatic Operation` 中，含 `Real` 的運算，其結果為 `Real` 。
* `Array` 只有在

    1. 元素型別相同
    2. 維度數相同
    3. 每一個維度大小相同

    三者皆合乎的情況下才是作相等型別。

### Statement

#### Simple

* `print`、`read` ， Variable Reference 必須是 `Scalar Type`.
* `assignment` ， `lhs` 和 `rhs` 型別必須相同， **`允許轉型`** 。  
* `assignment` ， 不允許對`array`和`constant`。

#### if、while

* `if` 、 `while` ， `condition expression` 必須是 `Boolean` 。

#### For

* `For` ， `loop_var` 不可以在迴圈內被變更。
* `For` ， `loop_var` 在其 `Scope` 、 `Inner Scope` 是唯一且不可二度宣告的。
* `For` ， `LowerBound <= UpperBound`。

#### Return

* 因為沒有`return type` ，`program` 裡是不能 `return` 的。
* `Function` 回傳值型別必須和 `return type` 相同。

    > 所以若是 `return type = void` ， 不能 `return`。

#### Function Call

* 所謂的 `procedure` ，即， `return type` = `void` ;
* 傳入 `parameter` 數必須等於 `function` 宣告時的數量。
* 傳入 `parameter` 型別必須等於 `function` 宣告時的型別，**`且順序相同`**。

### Identifer

* 最長32字元，超過部分捨棄。

## 實作細節

以下大部分功能皆屬於 Semantic Analzyer 的一部分，用於維護Inter-Node間資訊的傳遞。

### How to Visit

每一個 Node 的 Visit 流程大致都如下：

1. Push Symbol Table of the Scope
2. Bulid & Push Symbol into Symbol Table
3. Visit Child Nodes
4. Semantic Check
5. Pop  Symbol Table of the Scope

其中

* Step 1 和 Step 5 是成對進行。
* 每一個步驟都是可選的，但執行順序不會變化。

    > 唯一的例外是 FunctionNode 。

另外

* 所有屬於 Expression 的 Node 都必須去注意 Expression Stack 的使用。

### Expression Stack

在 Visit Child Node 時使用，用於運算式型別壓縮。

所有的 Expression Node 都會有其所代表的僅僅一種的 Type，例如：

```cpp
90 > 66 -> True
```

左側是一個 Binary Operator Expression ， 由兩個 Integer 坍縮為右側的一個 Boolean 。

類似 Post-fix 的計算順序，讓我們可以利用 Stack 來將一長串的運算型別壓縮，在最後觀察 Stack Top 就可以知道此一運算式其所回傳的型別為何者。

不過請注意押入的順序和取出的順序是相反的，所以 `LHS` 和 `RHS` 會是 `RHS` 先取出。

### Specify Kind

在 Visit Child Node 前使用，用於強制指定之後放入 Symbol Table 之 Identifer 的 Kind 為何者。

會有如此需求是因為 variable、parameter、loop_var 三者都是源自 VariableNode ，但其 Kind 都必須交由 Inter-Node 提供的資訊才能決定。

### Source Node Stack

在 Visit Child Node 前使用，用於告知前一級的 Node 為何者。

主要用途是告訴 Compound Statement 當前一級是 Function Node 時，不須新增Scope。

我覺得應該可以有更多用處就是了，待開發。

### Source Code Listing

懶人特性超級發作，沒有去修改 `scanner.l`。

我的做法就是把`Cursor`拉回文件起點，讀取所需行數，直到讀到我需要的那一行為止。這個做法最大的問題就是慢的不得了。

```cpp
string temp="";
char buffer[1024];

fseek( fp, 0, SEEK_SET );       // Back to Start

for(uint i=0; i<line_num; i++){
    fgets(buffer, 1024, fp);
}

temp+="    ";
temp+=string(buffer);
```

### Multi-Pass Symbol Table

由於 Stack 的特性， Pop 完後的資訊是找不到的。且礙於`C++ STL`的設計，是無法在不操作 Stack 的情況下，取得除了 Stack Top 以外的資訊的。

為了完成 Multi-Pass Symbol Table ， 利用雙向 Tree 的形式會比較好， 具體而言如下 ：

```cpp
class SymbolTable{
    public:
        // Link Info
        SymbolTable*            prev_scope;
        vector<SymbolTable*>    next_scope_list;

        // General Info
        unsigned int             level;
        map<string, SymbolEntry> entry;
        vector<string>           entry_name;

        ...
};
```

每一個 Push ，都是在母節點下增加新的子節點，並移動到其中。

每一個 Pop ，都是從子節點回到母節點。
