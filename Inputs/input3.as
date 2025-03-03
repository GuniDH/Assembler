;operation errors
mov r1,#4
sub 1,r4
jmp #1(r3,r4)
mov r1,r1         woopsv1
;definitions
L1: sub r1,r4

;a warning
K3: .extern W

;data and string errors
AK47: .data 1,#5
L9: .data
T1: .data 1,sda,j23
DK: .string "abcde"               woopsv2
LNG: .string

;label errors
bne: add #1,r6
jmp 9LNG(#1,#3)




