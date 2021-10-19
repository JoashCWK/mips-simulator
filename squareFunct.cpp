//Square function for integers without multiplication in C++
//"instructions.txt" is based on this code
//Reference:
//https://codereview.stackexchange.com/questions/74296/calculate-square-of-a-number-without-multiplication

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <bitset>
using namespace std; 
int Square(int value){
    int mask = 1;
    int factor = value;
    int sum = 0;
    while (value >= mask){
        if ((value & mask) == mask){
            sum += factor;
        }
        factor = (factor << 1);
        mask = (mask << 1);
    }
    return sum;
}
int array[99];
int main(){
	for(int i=1; i<100; i++){
		array[i] = Square(i);
	}
}
/*
	addi $t0, $zero, 1
BIG_LOOP:
	addi $t1, $zero, 1
	add  $t2, $zero, $t0
	addi $t3, $zero, 0
	slti $t4, $t0, 100
	beq  $t4, $zero, EXIT_BIG_LOOP
LOOP:
	and  $t6, $t1, $t0
	beq  $t6, $t1, IF
	j    EXIT_IF
IF:
	add  $t3, $t3, $t2
EXIT_IF:
	sll  $t2, $t2, 1
	sll  $t1, $t1, 1
	slt  $t5, $t0, $t1
	beq  $t5, $zero, LOOP
EXIT:
	addi $t7, $t0, -1
	sll  $t7, $t7, 2
	add  $t7, $t7, $zero
	sw   $t3, 0($t7)
	addi $t0, $t0, 1
	j    BIG_LOOP
EXIT_BIG_LOOP:
*/

/*
	addi $s0, $zero, 0
L1:
	addi $sp, $sp, -4
	sw   $s0, 0($sp)
	addi $s0, $zero, 8
	add  $t0, $zero, $s0
	sll  $t1, $t0, 2
	slt  $t2, $t0, $t1
	addi $t3, $zero, 2
	bne  $t2, $zero, L2
	addi $t3, $zero, -5
L2:
	sw   $t3, 0($t1)
	lw   $t4, 0($t1)
	j    L3
	sub  $t5, $t2, $t1
L3:
	lw   $s0, 0($sp)
	addi $sp, $sp, 4
*/