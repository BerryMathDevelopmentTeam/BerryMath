# Bytecode Describe

BMVM(BerryMath Virtual Machine) bytecode describe.

## Bytecode header
**74 af be 28**  
|_magic code_|  
**61 62 63 ... 00**  
|_raw entry file_|  
**00 00 01 6c cd a6 89 1a**  
|---_built time_---|  
**a0 01**  
|*compiler version***^**|  
**00 07 d5 75 8a 9f 0a 5d**  
|----_entry address_----|  
___Notes___  
_^: The first bit means the version type. a: Official version, b: Advanced version, c: Test version_    

## Bytecode&ASM table
| Bytecode | ASM | Describe | Note | Example |
|---|---|---|---|---|
| 00 | push | type, value     | Push values into the the stack | push NUM, 0 |
| 01 | add  | /               | Add the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | add |
| 02 | sub  | /               | Subtract the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | sub |
| 03 | mul  | /               | Multiply the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | mul |
| 04 | div  | /               | Divide the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | div |
| 05 | mod  | /               | Remaining the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | mod |
| 06 | pow  | /               | The two elements on the top of the stack, the second one being the base number, the top being the index, are used to calculate the power of the two elements, and the two elements are deleted. Finally, the value push is returned to the top of the stack. | pow |
| 07 | and  | /               | Arithmetically and the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | and |
| 08 | or   | /               | Arithmetically or the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | or |
| 09 | xor  | /               | Arithmetically xor the two elements at the top of the stack, delete them, and finally push the value back to the top of the stack. | xor |
| 0a | nop  | /               | Meaningless symbols, similar to pass statements in BerryMath. | nop |
| 0b | shl  | /               | The two elements on the top of the stack, the second element as the base, the top as the number of times to shift left, find the results, and delete the two elements, and finally push the value back to the top of the stack. | shl |
| 0c | shr  | /               | The two elements on the top of the stack, the second element as the base, the top as the number of times to shift right, find the results, and delete the two elements, and finally push the value back to the top of the stack. | shr |
| 0d | eq   | /               | Compare the two elements on the top of the stack to be equal, equal to 1, otherwise 0, and delete the two elements. Finally, push the result back to the top of the stack. | eq |
| 0e | neq  | /               | Compare the two elements on the top of the stack to be not equal, equal to 1, otherwise 0, and delete the two elements. Finally, push the result back to the top of the stack. | neq |
| 0f | lt   | /               | Whether the second element is larger than the top of the stack, larger than 1, otherwise 0, and delete the two elements on the top of the stack, and finally push the result into the stack. | lt |
| 10 | gt   | /               | Whether the second element is smaller than the top of the stack, larger than 1, otherwise 0, and delete the two elements on the top of the stack, and finally push the result into the stack. | gt |
| 11 | le   | /               | Whether the second element is larger or equal than the top of the stack, larger than 1, otherwise 0, and delete the two elements on the top of the stack, and finally push the result into the stack. | le |
| 12 | ge   | /               | Whether the second element is smaller or equal than the top of the stack, larger than 1, otherwise 0, and delete the two elements on the top of the stack, and finally push the result into the stack. | ge |
| 13 | lan  | /               | The second element and the first element are logically and operationally calculated, and the two elements at the top of the stack are deleted. Finally, the result push is put on the stack. | lan |
| 14 | lor  | /               | The second element and the first element are logically or operationally calculated, and the two elements at the top of the stack are deleted. Finally, the result push is put on the stack. | lor |
| 15 | not  | /               | Arithmetic inversion of elements at the top of stack. | not |
| 16 | lnt  | /               | Logical inversion of elements at the top of stack. | lnt |
| 17 | min  | /               | Inversion of the element symbol at the top of the stack. | min |
| 18 | jmp  | address         | Jump to an address. | jmp |
| 19 | jmpt | address         | If the top element is true, jump to an address. | jmpt |
| 1a | jmpf | address         | If the top element is false, jump to an address. | jmpt |
| 1b | call | offset          | Call the function on the address(top - offset). | call 0 |
| 1c | ret  | /               | Finish function processing. | ret |
| 1d | mov  | offset          | Move the top value to the top-offset. | mov 2 |
| 1e | load | offset         | Push the value in top-offset onto the stack. | load 3 |
| 2f | imp  | /               | Import modules based on the value of the top element of the stack. | imp |
| 20 | line | source-line     | Identification is the line of source code. | line |
| 21 | adp  |                 | Add attributes to the top-2 element of the stack, named the value pointed to top-1 and the value pointed to top. | adp |
| ff | .JLAB|                 | Pseudo instructions for setting labels (used only when the compiler generates code). | / |

## Types
| Bytecode Name | ASM Name | describe |
|---|---|---|
| 00 | NUM | Number |
| 01 | STR | String |
| 02 | OBJ | Object |
| 03 | NUL | Null |
| 04 | UND | Undefined |

## Explanation of Address
bp register points to address 0x00000002 in initial state, sp points to address 0x00000002

## Notes
- When push null type, object type and undefined type are used, the value after push null is 0 of 1 byte.
- Push OBJ, 0 is to push an empty object into the stack。
