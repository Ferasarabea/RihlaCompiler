

# **Rihla Compiler**

Rihla Compiler is a custom-built compiler for the **Rihla Programming Language**. Designed as part of a computer science project, Rihla focuses on simplicity and readability, making it ideal for both academic learning and exploring compiler construction techniques. The compiler implements lexical analysis, parsing, semantic analysis, and code generation to execute Rihla programs.

---

## **Features**

- **Lexical Analysis**:
  - Tokenizes Rihla source code into meaningful tokens using a scanner.
  - Supports single-line comments (`//`) and newline (`nl`) detection.

- **Parsing**:
  - Implements recursive-descent parsing for Rihla's grammar.
  - Supports non-terminal entities like `<variable>`, `<assignmentStatement>`, `<INPUTStatement>`, and more.

- **Semantic Analysis**:
  - Validates syntax and semantics to prevent runtime errors.
  - Detects undeclared variables, type mismatches, and invalid operations.

- **Code Generation**:
  - Converts Rihla programs into executable intermediate code.
  - Outputs assembly-like instructions for efficient execution.

---

## **Installation**

### **Prerequisites**
1. **Programming Language**:
   - The compiler is written in **C++** (or any language you chose for implementation).
2. **Development Environment**:
   - A compiler like GCC or Visual Studio Code with appropriate extensions for C++.


## **Usage**

1. Write a Rihla program in a `.rhl` file. Example:
   ```
// Rihla Sample Program
WRITE x
y = x + 5
SHOW y
   ```

2. Compile the program:
   ```bash
   ./rihla_compiler sample.rhl
   ```

3. Execute the generated intermediate code or binary.

---

## **Rihla Grammar Overview**

- **Comments**:
  - Use `//` for single-line comments.
- **Variables**:
  - Declare and use variables for computations and I/O.
- **Statements**:
  - **Input**: `WRITE x`
  - **Output**: `SHOW x`
  - **Assignment**: `x = y + z`

### Example:
```rhl
// Compute sum of two numbers
INPUT a
INPUT b
c = a + b
SHOW c
```

---

## **Key Components**

1. **Scanner**:
   - Reads source code and generates tokens.
   - Handles keywords, variables, operators, and literals.

2. **Parser**:
   - Implements recursive-descent parsing for:
     - Input and output statements.
     - Assignment statements.
     - Structured flow-of-control constructs.

3. **Semantic Analyzer**:
   - Ensures all variables are declared before use.
   - Validates type compatibility for operations.

4. **Code Generator**:
   - Translates valid Rihla programs into intermediate assembly-like code.
   - Optimizes instructions for execution.

---

## **Planned Enhancements**

- Add support for:
  - Functions and procedures.
  - Complex flow-of-control constructs like `while`, `for`, and `if-else`.
  - Error recovery for parsing errors.
- Extend support for additional data types and operations.

---

## **Contributing**

Contributions are welcome! If you have suggestions or would like to extend the Rihla Compiler, feel free to open an issue or submit a pull request.

---



## **Contact**

For questions or feedback, please contact:
- **Feras Rabea**  
- Email: [ferasarabea@gmail.com](mailto:ferasarabea@gmail.com)

