# Bytecode Describe

BMVM(BerryMath Virtual Machine) bytecode describe.

## Bytecode header
74 af be 28  
|magic code|  
61 62 63 ... 00  
|raw entry file|  
01 6c c1 dd ab 9e  
|---built time---|  
a0 01  
|compiler version^|  
00 07 d5 75 8a 9f 0a 5d  
|----entry address----|  
___Notes___  
_^: The first bit means the version type. a: Official version, b: Advanced version, c: Beta version_    

## Bytecode&ASM table
| Bytecode | ASM | Describe | Note | Example |
|---|---|---|---|---|
| 00 | load | type, value | Load values into the the stack | load NUM, 0 |
| 01 | load | SDid | Load static data into the the stack | load #a3 |
| 02 | mov | reg1, reg2 | Mov the value of reg2 into reg1 | mov ax, bx |
| 03 | mov | reg, address | Mov the value of address into reg | mov ax, [bx] |
| 04 | mov | address1, address2 | Mov the value of address2 into address1 | mov [ax], [bx] |
| 05 | mov | address, reg | Mov the value of reg into address | mov [ax], bx |
| 06 | push | address | Push the value of address into the the stack | push [ax] |
| 07 | push | reg | Push the value of reg into the the stack | push [ax] |
| 08 | call | label-address | Call the function at label-address | push 0f9c187c |
| 09 | call | Module-address, offset | Call the function in module-address at offset-address | push [bp+1], 0f9c187c |
| 0a | ret | / | Return | ret |
| 0b | adp | reg1, rstr, reg2 | Push a property which is in reg2 into reg1 with rstr | adp ax, "name", bx |
| 0c | adp | address, rstr, reg | Push a property which is in reg into address with rstr | adp [ax], "name", bx |
| 0d | adp | address1, rstr, address2 | Push a property which is in address2 into address1 with rstr | adp [ax], "name", [bx] |
| 0e | adp | reg1, address, reg2 | Push a property which is in reg2 into reg1 with address | adp ax, [cx], bx |
| 0f | adp | address1, address2, reg | Push a property which is in reg into address1 with address2 | adp [ax], [cx], bx |
| 10 | adp | address1, address2, address3 | Push a property which is in address3 into address1 with address2 | adp [ax], [cx], [bx] |
| 11 | adp | reg1, reg2, reg3 | Push a property which is in reg3 into reg1 with reg2 | adp ax, cx, bx |
| 12 | adp | address, reg1, reg2 | Push a property which is in reg2 into address with reg1 | adp [ax], cx, bx |
| 13 | adp | address1, reg, address2 | Push a property which is in address2 into address1 with reg | adp [ax], cx, [bx] |
| 14 | add | reg1, reg2 | Add reg1's value and reg2's value and saves in reg1 | add ax, bx |
| 15 | add | reg, address | Add reg's value and address's value and saves in address | add ax, [bx] |
| 16 | add | address, reg | Add reg's value and address's value and saves in reg | add [ax], bx |
| 17 | add | address1, address2 | Add address1's value and address2's value and saves in address1 | add [ax], [bx] |
| 18 | sub | reg1, reg2 | Sub reg1's value and reg2's value and saves in reg1 | sub ax, bx |
| 19 | sub | reg, address | Sub reg's value and address's value and saves in address | sub ax, [bx] |
| 1a | sub | address, reg | Sub reg's value and address's value and saves in reg | sub [ax], bx |
| 1b | sub | address1, address2 | Sub address1's value and address2's value and saves in address1 | sub [ax], [bx] |
| 1c | mul | reg1, reg2 | Mul reg1's value and reg2's value and saves in reg1 | mul ax, bx |
| 1d | mul | reg, address | Mul reg's value and address's value and saves in address | mul ax, [bx] |
| 1e | mul | address, reg | Mul reg's value and address's value and saves in reg | mul [ax], bx |
| 1f | mul | address1, address2 | Mul address1's value and address2's value and saves in address1 | mul [ax], [bx] |
| 20 | div | reg1, reg2 | Div reg1's value and reg2's value and saves in reg1 | div ax, bx |
| 21 | div | reg, address | Div reg's value and address's value and saves in address | div ax, [bx] |
| 22 | div | address, reg | Div reg's value and address's value and saves in reg | div [ax], bx |
| 23 | div | address1, address2 | Div address1's value and address2's value and saves in address1 | div [ax], [bx] |
| 24 | mod | reg1, reg2 | Mod reg1's value and reg2's value and saves in reg1 | div ax, bx |
| 25 | mod | reg, address | Mod reg's value and address's value and saves in address | div ax, [bx] |
| 26 | mod | address, reg | Mod reg's value and address's value and saves in reg | div [ax], bx |
| 27 | mod | address1, address2 | Mod address1's value and address2's value and saves in address1 | div [ax], [bx] |
| 28 | pow | reg1, reg2 | Pow reg1's value and reg2's value and saves in reg1 | pow ax, bx |
| 29 | pow | reg, address | Pow reg's value and address's value and saves in address | pow ax, [bx] |
| 2a | pow | address, reg | Pow reg's value and address's value and saves in reg | pow [ax], bx |
| 2b | pow | address1, address2 | Pow address1's value and address2's value and saves in address1 | pow [ax], [bx] |
| 2c | shl | reg1, reg2 | Shift left reg1's value and reg2's value and saves in reg1 | shl ax, bx |
| 2d | shl | reg, address | Shift left reg's value and address's value and saves in address | shl ax, [bx] |
| 2e | shl | address, reg | Shift left reg's value and address's value and saves in reg | shl [ax], bx |
| 2f | shl | address1, address2 | Shift left address1's value and address2's value and saves in address1 | shl [ax], [bx] |
| 30 | shr | reg1, reg2 | Shift right reg1's value and reg2's value and saves in reg1 | shr ax, bx |
| 31 | shr | reg, address | Shift right reg's value and address's value and saves in address | shr ax, [bx] |
| 32 | shr | address, reg | Shift right reg's value and address's value and saves in reg | shr [ax], bx |
| 33 | shr | address1, address2 | Shift right address1's value and address2's value and saves in address1 | shr [ax], [bx] |
| 34 | les | reg1, reg2 | Compare is reg1's value less than reg2's value and saves in reg1 | les ax, bx |
| 35 | les | reg, address | Compare is reg's value less than address's value and saves in address | les ax, [bx] |
| 36 | les | address, reg | Compare is reg's value less than address's value and saves in reg | les [ax], bx |
| 37 | les | address1, address2 | Compare is address1's value less than address2's value and saves in address1 | les [ax], [bx] |
| 38 | mor | reg1, reg2 | Compare is reg1's value more than reg2's value and saves in reg1 | mor ax, bx |
| 39 | mor | reg, address | Compare is reg's value more than address's value and saves in address | mor ax, [bx] |
| 3a | mor | address, reg | Compare is reg's value more than address's value and saves in reg | mor [ax], bx |
| 3b | mor | address1, address2 | Compare is address1's value more than address2's value and saves in address1 | mor [ax], [bx] |
| 3c | eles | reg1, reg2 | Compare is reg1's value equal or less than reg2's value and saves in reg1 | eles ax, bx |
| 3d | eles | reg, address | Compare is reg's value equal or less than address's value and saves in address | eles ax, [bx] |
| 3e | eles | address, reg | Compare is reg's value equal or less than address's value and saves in reg | eles [ax], bx |
| 3f | eles | address1, address2 | Compare is address1's value equal or less than address2's value and saves in address1 | eles [ax], [bx] |
| 40 | emor | reg1, reg2 | Compare is reg1's value equal or more than reg2's value and saves in reg1 | emor ax, bx |
| 41 | emor | reg, address | Compare is reg's value equal or more than address's value and saves in address | emor ax, [bx] |
| 42 | emor | address, reg | Compare is reg's value equal or more than address's value and saves in reg | emor [ax], bx |
| 43 | emor | address1, address2 | Compare is address1's value equal or more than address2's value and saves in address1 | emor [ax], [bx] |
| 44 | eq | reg1, reg2 | Compare is reg1's value equal reg2's value and saves in reg1 | eq ax, bx |
| 45 | eq | reg, address | Compare is reg's value equal address's value and saves in address | eq ax, [bx] |
| 46 | eq | address, reg | Compare is reg's value equal address's value and saves in reg | eq [ax], bx |
| 49 | eq | address1, address2 | Compare is address1's value equal address2's value and saves in address1 | eq [ax], [bx] |
| 4a | neq | reg1, reg2 | Compare isn't reg1's value equal reg2's value and saves in reg1 | neq ax, bx |
| 4b | neq | reg, address | Compare isn't reg's value equal address's value and saves in address | neq ax, [bx] |
| 4c | neq | address, reg | Compare isn't reg's value equal address's value and saves in reg | neq [ax], bx |
| 4d | neq | address1, address2 | Compare isn't address1's value equal address2's value and saves in address1 | neq [ax], [bx] |
| 4e | and | reg1, reg2 | reg1's value and reg2's value and saves in reg1 | and ax, bx |
| 4f | and | reg, address | reg's value and address's value and saves in address | and ax, [bx] |
| 50 | and | address, reg | reg's value and address's value and saves in reg | and [ax], bx |
| 51 | and | address1, address2 | address1's value and address2's value and saves in address1 | and [ax], [bx] |
| 52 | or | reg1, reg2 | reg1's value and reg2's value or saves in reg1 | or ax, bx |
| 53 | or | reg, address | reg's value and address's value or saves in address | or ax, [bx] |
| 54 | or | address, reg | reg's value and address's value or saves in reg | or [ax], bx |
| 55 | or | address1, address2 | address1's value or address2's value and saves in address1 | or [ax], [bx] |
| 56 | xor | reg1, reg2 | reg1's value xor reg2's value xor saves in reg1 | xor ax, bx |
| 57 | xor | reg, address | reg's value xor address's value xor saves in address | xor ax, [bx] |
| 58 | xor | address, reg | reg's value xor address's value xor saves in reg | xor [ax], bx |
| 59 | xor | address1, address2 | address1's value xor address2's value and saves in address1 | xor [ax], [bx] |
| 5a | land | reg1, reg2 | reg1's value and (logic) reg2's value and saves in reg1 | land ax, bx |
| 5b | land | reg, address | reg's value and (logic) address's value and saves in address | land ax, [bx] |
| 5c | land | address, reg | reg's value and (logic) address's value and saves in reg | land [ax], bx |
| 5d | land | address1, address2 | address1's value and (logic) address2's value and saves in address1 | land [ax], [bx] |
| 5e | lor | reg1, reg2 | reg1's value or (logic) reg2's value and saves in reg1 | lor ax, bx |
| 5f | lor | reg, address | reg's value or (logic) address's value and saves in address | lor ax, [bx] |
| 60 | lor | address, reg | reg's value or (logic) address's value and saves in reg | lor [ax], bx |
| 61 | lor | address1, address2 | address1's value or (logic) address2's value and saves in address1 | lor [ax], [bx] |
| 62 | not | reg | not reg and saves in reg | not ax |
| 63 | not | address | not address's value and saves in address | not [ax] |
| 64 | lnot | reg | not (logic) reg and saves in reg | lnot ax |
| 65 | lnot | address | not (logic) address's value and saves in address | lnot [ax] |
| 66 | get | reg, rstr | get reg[rstr] and saves in the the stack | get ax, "name" |
| 67 | get | address, rstr | get address[rstr] and saves in the the stack | lnot [ax], "name" |
| 68 | get | reg1, reg2 | get reg1[reg2] and saves in the the stack | get ax, "name" |
| 69 | get | address, reg | get address[reg] and saves in the the stack | get [ax], bx |
| 6a | get | reg, address | get reg[*address] and saves in the the stack | get ax, [bx] |
| 6b | get | address1, address2 | get address1[*address2] and saves in the the stack | get [ax], [bx] |
| 6c | jmp | label-address | Jump to label-address | jmp 0007d5758a9f0a5d |
| 6d | jmpc | label-address | If the value in ax is true, jump to label-address | jmpc 0007d5758a9f0a5d |
| 6e | imp | rstr | Import rstr and push it into the the stack | imp "bmlang" |
| 6f | imp | reg | Import reg and push it into the the stack | imp ax |
| 70 | imp | address | Import *address and push it into the the stack | imp [ax] |
| 71 | bit | reg | Get the begin iterator of reg and saves it in an | bit ax |
| 72 | bit | address | Get the begin iterator of *address and saves it in an | bit [ax] |
| 73 | eit | reg | Get the end iterator of reg and saves it in an | eit ax |
| 74 | eit | address | Get the end iterator of *address and saves it in an | eit [ax] |
| 75 | key | native-reg | Get the key of native-reg and saves it in ax | key an |
| 76 | key | native-address | Get the key of *native-address and saves it in ax | key [an] |
| 77 | val | native-reg | Get the val of native-reg and saves it in ax | val an |
| 78 | val | native-address | Get the val of *native-address and saves it in ax | val [an] |
| 79 | nxt | native-reg | Get the next iterator of native-reg and saves it in native-reg | nxt an |
| 7a | nxt | native-address | Get the next iterator of *native-address and saves it in native-address | nxt [an] |
| 7b | pre | native-reg | Get the previous iterator of native-reg and saves it in native-reg | pre an |
| 7c | pre | native-address | Get the previous iterator of *native-address and saves it in native-address | pre [an] |
| 7d | nop | / | Useless statements, similar to pass in BerryMath | nop |

## Register table
| Bytecode Name | ASM Name | describe | note | address |
|---|---|---|---|---|
| 00 | ax | General Register | / | 00000000 |
| 01 | bx | General Register | / | 00000001 |
| 02 | cx | General Register | / | 00000002 |
| 03 | dx | General Register | / | 00000003 |
| 04 | cs | Pointer Register | Store the current instruction offset address | 00000004 |
| 05 | sp | Pointer Register | Store the stack top address | 00000005 |
| 06 | bp | Pointer Register | Store the stack base address | 00000006 |
| 07 | an | Native Register | / | 00000007 |
| 08 | bn | Native Register | / | 00000008 |
| 09 | cn | Native Register | / | 00000009 |
| 0a | dn | Native Register | / | 0000000a |

## Explanation of Address
bp register points to address 0x0000000b in initial state, sp points to address 0x0000000c

## Notes
Native Registers store the native object in c/c++, the underlying implementation is of type void*
