include \masm32\include\masm32rt.inc

.code
start:

PUSH 0

SUB esp, 4
POP [ebp-4]

L0:
PUSH 5
PUSH [ebp-4]
POP eax
POP ebx
cmp eax, ebx
JL L2
PUSH 0
JMP L3
L2: PUSH 1
L3:

POP eax
CMP eax, 0
JZ L1
PUSH 2
PUSH [ebp-4]
POP eax
POP ebx
cmp eax, ebx
JE L6
PUSH 0
JMP L7
L6: PUSH 1
L7:

POP eax ;; Save condition result
CMP eax, 0
JZ L5 ;; If conditin is false, jump to false label

PUSH 1

POP eax
ADD [ebp-4], eax

JMP L0 ; continue
JMP L4
L5:
L4:
PUSH [ebp-4]
POP eax
print sstr$(eax), 13, 10
PUSH 1

POP eax
ADD [ebp-4], eax

JMP L0
L1:

end start
