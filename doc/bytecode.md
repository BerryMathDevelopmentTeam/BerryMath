# Bytecode Describe

BMVM(BerryMath Virtual Machine) bytecode syntax based with x86 cpu ASM.

## Bytecode header
ac de (magic code) 68 65 6c 6c 6f (entry source filename) 00 (filename end)

## Bytecode&ASM table
| Bytecode | ASM | describe | note | usage |
|---|---|---|---|---|
| 00 00 | .DATA | Declare static datas | / | `.DATA` |
| 00 01 | .TEXT | Program start | / | `.TEXT` |
| 00 10 | push | Push value into stack | / | `push <reg64>` `push <mem>` `push <con32>` |
| 00 11 | mov | Mov value into register | / | `mov <reg>,<reg>` `mov <reg>,<mem>` `mov <mem>,<reg>` `mov <reg>,<const>` `mov <mem>,<const>` |
| 00 12 | pop | Pop the top element of the stack into a register or a memory | / | `pop <reg>` `pop <mem>` |
| 00 13 | add | Add numbers | / | `add <reg>,<reg>` `add <reg>,<mem>` `add <mem>,<reg>` `add <reg>,<con>` `add <mem>,<con>` |
| 00 14 | sub | Sub numbers | / | `sub <reg>,<reg>` `sub <reg>,<mem>` `sub <mem>,<reg>` `sub <reg>,<con>` `sub <mem>,<con>` |
| 00 15 | mul | Mul numbers | / | `mul <reg>,<reg>` `mul <reg>,<mem>` `mul <mem>,<reg>` `mul <reg>,<con>` `mul <mem>,<con>` |
| 00 16 | div | Div numbers | / | `div <reg>,<reg>` `div <reg>,<mem>` `div <mem>,<reg>` `div <reg>,<con>` `div <mem>,<con>` |
| 00 17 | idiv | Div numbers to get the quotient and the residual | The quotient saves in rax, the residual saves in rbx | `idiv <reg>,<reg>` `idiv <reg>,<mem>` `idiv <mem>,<reg>` `idiv <reg>,<con>` `idiv <mem>,<con>` |
| 00 18 | and | Math and operator | / | `and <reg>,<reg>` `and <reg>,<mem>` `and <mem>,<reg>` `and <reg>,<con>` `and <mem>,<con>` |
| 00 19 | or | Math or operator | / | `or <reg>,<reg>` `or <reg>,<mem>` `or <mem>,<reg>` `or <reg>,<con>` `or <mem>,<con>` |
| 00 1a | xor | Math xor operator | / | `xor <reg>,<reg>` `xor <reg>,<mem>` `xor <mem>,<reg>` `xor <reg>,<con>` `xor <mem>,<con>` |
| 00 1b | not | Bitwise bits | / | `not <reg>` `not <mem>` |
| 00 1c | neg | Negate | / | `neg <reg>` `neg <mem>` |
| 00 1d | shl | Shift left | / | `shl <reg>,<con8>` `shl <mem>,<con8>` `shl <reg>,<cl>` `shl <mem>,<cl>` |
| 00 1e | shr | Shift right | / | `shr <reg>,<con8>` `shr <mem>,<con8>` `shr <reg>,<cl>` `shr <mem>,<cl>` |
| 00 1f | jmp | Jump | / | `jmp <label>` |
| 00 20 | je | Jump when equal | Conditional saves in register al (The notes for the following instruction jump are the same as here) | `je <label>` |
| 00 21 | jne | Jump when not equal | / | `jne <label>` |
| 00 22 | jz | Jump when the result was zero | / | `jz <label>` |


## Register table
| Bytecode Name | ASM Name | describe | note |
|---|---|---|---|
| 00 | rax | To save operation data | 8 bytes |
| 01 | eax | To save operation data | 4 bytes (It's included in rax, right) |
| 02 | ax | To save operation data | 2 bytes (It's included in eax, right) |
| 03 | ah | To save operation data | 1 byte (It's included in ax, left) |
| 04 | al | To save operation data | 1 byte (It's included in ax, right) |
| 05 | rbx | To save operation data | 8 bytes |
| 06 | ebx | To save operation data | 4 bytes (It's included in rbx, right) |
| 07 | bx | To save operation data | 2 bytes (It's included in ebx, right) |
| 08 | bh | To save operation data | 1 byte (It's included in bx, left) |
| 09 | bl | To save operation data | 1 byte (It's included in bx, right) |
| 0a | rcx | To save loop data or quotient | 8 bytes |
| 0b | ecx | To save loop data or quotient | 4 bytes (It's included in rcx, right) |
| 0c | cx | To save loop data or quotient | 2 bytes (It's included in ecx, right) |
| 0d | ch | To save loop data or quotient | 1 byte (It's included in cx, left) |
| 0e | cl | To save loop data or quotient | 1 byte (It's included in cx, right) |
| 0f | rdx | To save remainder | 8 bytes |
| 10 | edx | To save remainder | 4 bytes (It's included in rdx, right) |
| 11 | dx | To save remainder | 2 bytes (It's included in edx, right) |
| 12 | dh | To save remainder | 1 byte (It's included in dx, left) |
| 13 | dl | To save remainder | 1 byte (It's included in dx, right) |
| 14 | rsp | Stack point | 8 bytes |
| 15 | rbp | Stack base point | 8 bytes |
| 16 | cs | Bytecode point | 8 bytes |
| 17 | ds | Static data point | 8 bytes |

## Static data saved flag
| Bytecode Name | name | describe |
|---|---|---|
| 00 | DB | 1 byte |
| 01 | DW | 2 bytes |
| 02 | DD | 4 bytes |
| 03 | DQ | 8 bytes |
| 04 | DUP | Declare array or string |