//MIPS Multi-Cycle Processor Simulation
//Written by: Joash Chan (Student ID: 3120 9513)
//for ELEC 2204 Computer Engineering Coursework
//MEng Electrical and Electronic Engineering, University of Southampton
//References:
//https://opencores.org/projects/plasma/opcodes (MIPS Instruction sets)
//https://www.youtube.com/watch?v=23NONE7u94A&t=353s (YouTube video on general code structure)
//http://www.fredosaurus.com/notes-cpp/io/readtextfile.html (Read from txt using fstream)
//https://codereview.stackexchange.com/questions/74296/calculate-square-of-a-number-without-multiplication (Square Function)

//Format of text file has to be strictly as is the case in
//"instructions.txt" for the assembly code to properly be translated
//into machine code

//Use fileAddress = "instructions.txt" for showcase testing
//"instructions.txt" contains the assembly code that is equivalent to the function in "squareFunct.cpp"
//Change variable "fileAddress" for different files

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <bitset>
#include <fstream>
#include <string>
using namespace std; 

//Declare integer twos used for twos compliment calculation for negative values
int twos = pow(2,16) - 1;

void ReadFile(string fileAddress, string *instructions, string *JumpDest, int *JumpDestInt, int *JumpDestLast, int *instructionsLast);
void GetMachineCode(string *instructions, unsigned *MemoryText, string *JumpDest, int *JumpDestInt, int JumpDestLast, int instructionsLast);

void IF(unsigned PC, unsigned* MemoryText, unsigned* IR, unsigned* PCnext);
void ID(unsigned* IR, unsigned *Register, unsigned *RegA, unsigned *RegB, unsigned* RegC, unsigned* A, unsigned* B, unsigned *ALUout, 
		unsigned* PCnext, unsigned* OpCode, unsigned* Function, unsigned* ShiftAmount, 
		unsigned* MemOffset, bool* ALUAccess, bool* MemAccess, bool* RegWrite, bool* StackAccess);
void EX(unsigned* ALUout, unsigned RegA, unsigned RegB, unsigned* RegC, unsigned A, unsigned B, unsigned OpCode, unsigned Function, 
		unsigned ShiftAmount, unsigned MemOffset, unsigned* PCnext);
void MEM(unsigned* MemoryData, unsigned* MemoryStack, unsigned* ALUout, unsigned OpCode, unsigned B, unsigned* RegC, unsigned RegB, bool* StackAccess);
void WB(unsigned OpCode, unsigned ALUout, unsigned RegC, unsigned* Register);

int main(){
	//variables for extracting machine code from assembly in text file
	
	//change for different text files
	string fileAddress = "instructions.txt";
	string instructions[100];
	unsigned MemoryText[400];
	string JumpDest[100];
	int JumpDestInt[100];
	int JumpDestLast = 0;
	int instructionsLast = 0;
	
	//initial declaration of variables used later
	unsigned PC = 0;
	unsigned PCnext = 0;
	unsigned IR[32];
	unsigned RegA, RegB, RegC, A, B, ALUout, OpCode, Function, ShiftAmount, MemOffset;
	bool MemAccess = false;
	bool RegWrite = false;
	bool ALUAccess = false;
	bool StackAccess = false;
	//separate array used to declare memory for store word
	//to illustrate different layers in the memory block of a MIPS processor
	unsigned MemoryData[400] ={0};
	unsigned MemoryStack[200] = {0};
	//32 bit Register
	//Reg[0] = $zero
	//Reg[10] - Reg[19] = $t0 - $t9
	//Reg[20] - Reg[27] = $s0 - $s7
	//Addresses do not follow the MIPs model
	//Just for simplicity of coding and viewing
	//Only $t0-$t7 are used in the showcase testing program
	unsigned Register[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,199,0,0};
	
	//read text file
	ReadFile(fileAddress, instructions, JumpDest, JumpDestInt, &JumpDestLast, &instructionsLast);
	//convert the file into machine code
	GetMachineCode(instructions, MemoryText, JumpDest, JumpDestInt, JumpDestLast, instructionsLast);
	
	int CycleCount = 0;
	//while the program has not reached the last line
	while(PC < (instructionsLast)*4){
		IF(PC, MemoryText, IR, &PCnext);
		CycleCount++;
		
		ID(IR, Register, &RegA, &RegB, &RegC, &A, &B, &ALUout, &PCnext, &OpCode, &Function, &ShiftAmount, &MemOffset,
			&ALUAccess, &MemAccess, &RegWrite, &StackAccess);
		CycleCount++;
		
		//skip this cycle for jump
		if(ALUAccess){
			EX(&ALUout, RegA, RegB, &RegC, A, B, OpCode, Function, ShiftAmount, MemOffset, &PCnext);
			CycleCount++;
			ALUAccess = false;
		}
		
		//only go through for store/load word
		if(MemAccess){
			MEM(MemoryData, MemoryStack, &ALUout, OpCode, B, &RegC, RegB, &StackAccess);
			CycleCount++;
			MemAccess = false;
		}
		
		//only go through for R type, ADDI or load word
		if(RegWrite){
			WB(OpCode, ALUout, RegC, Register);
			CycleCount++;
			RegWrite = false;
		}
		PC = PCnext;
	}
	
	//Printing Outputs
	cout << "Number of cycles taken: " << dec << CycleCount << endl << endl;
	cout << "Temporary registers: Reg[10]-Reg[19]" << endl;
	cout << "Saved registers: Reg[20]-Reg[27]" << endl;
	cout << "Stack Pointer: Reg[29]" << endl << endl;
	cout << "Values in Register[0] to Register[31]: " << endl;
	for(int i=0; i<16; i++){
		if(i<10){cout << " ";}
		cout << "$t" << i << ": " << Register[10+i];
		if(Register[10+i]<10){cout << " ";}
		if(Register[10+i]<100){cout << " ";}
		if(Register[10+i]<1000){cout << " ";}
		if(Register[10+i]<10000){cout << " ";}
		if(16+i<10){cout << " ";}
		cout << "   $t" << 16+i << ": " << Register[14 + i] << endl; 
	}
	
	cout << endl << "Squares of each integer stored in MemoryData[395-0] (hex): " << endl;
	
	int diff = 132;
	for(int i=0; i<diff; i++){
		if(i%4 == 0){
			cout << dec << (i+1)/4 + 1 << ": ";
			if((i+1)/4 + 1 < 10){ cout << " ";}
		}
		if(MemoryData[i] < 16){cout << " ";}
		cout << hex << MemoryData[i] <<  " ";
		if((i+1)%4 == 0){
			cout << "   " << dec << (i-3)/4 + diff/4 + 1 << ": ";
			for(int j=0; j<4; j++){
				if(MemoryData[(i-3)+ diff +j] < 16){cout << " ";}
				cout << hex << MemoryData[i-3+ diff +j] << " ";
			}
			cout << "   " << dec << (i-3)/4 + diff/2 + 1 << ": ";
			for(int j=0; j<4; j++){
				if(MemoryData[(i-3)+ 2*diff +j] < 16){cout << " ";}
				cout << hex << MemoryData[i-3 + 2*diff + j] << " ";
			}
			cout << endl;
		}
	}
}

void ReadFile(string fileAddress, string *instructions, string *JumpDest, int *JumpDestInt, int *JumpDestLast, int *instructionsLast){
	fstream file;
	//open a file to perform read operation using file object
	file.open(fileAddress,ios::in);
	
	string instruction;
	int line = 0;
	//check whether the file is open
	if (file.is_open()){
		//read data from file object and put it into string.
		while(getline(file, instruction)){
			line++;
			
			//replace $zero with $z0 so that it can be more easily decoded
			int zeroFound = instruction.find("$zero");
			if(zeroFound != string::npos){
				instruction.replace(zeroFound, 5, "$z0");
			}
			
			//if : is present in line signifying that it is a branch label
			if(instruction.find(":") != string::npos){
				//store the name of the branch and its line number
				JumpDest[*JumpDestLast] = instruction.substr(0, instruction.size()-1);
				JumpDestInt[*JumpDestLast] = --line;
				*JumpDestLast += 1;
			}
			//if it is an ordinary instruction set, add to the array
			else{
				instructions[*instructionsLast] = instruction;
				*instructionsLast += 1;
			}
		}
		file.close(); //close the file object.
	}
}

void GetMachineCode(string *instructions, unsigned *MemoryText, string *JumpDest, int *JumpDestInt, int JumpDestLast, int instructionsLast){
	int line = 0;
	unsigned rs, rt, rd;
	unsigned MachineCode;
	//run until the last instruction in the array
	for(int line=0; line<(instructionsLast + 1); line++){
		//Get the value of rd if present
		if(instructions[line][6] == '$'){
			if(instructions[line][7] == 't'){
				rd = (int)(instructions[line][8]) - (int)'0' + 10;
			}
			else if(instructions[line][7] == 's'){
				if(instructions[line][8] == 'p'){
					rd = 29;
				}
				else{
					rd = (int)(instructions[line][8]) - (int)'0' + 20;
				}
			}
		}
		else{
			rd = 100;
		}
		//Get the value of rs if present
		if(instructions[line][11] == '$'){
			if(instructions[line][12] == 't'){
				rs = (int)(instructions[line][13]) - (int)'0' + 10;
			}
			else if(instructions[line][12] == 's'){
				if(instructions[line][13] == 'p'){
					rs = 29;
				}
				else{
					rs = (int)(instructions[line][13]) - (int)'0' + 20;
				}
			}
			else if(instructions[line][12] == 'z'){
				rs = 0;
			}
		}
		else{
			rs = 100;
		}
		//Get the value of rt if present
		if(instructions[line][16] == '$'){
			if(instructions[line][17] == 't'){
				rt = (int)(instructions[line][18]) - (int)'0' + 10;
			}
			else if(instructions[line][17] == 's'){
				if(instructions[line][18] == 'p'){
					rt = 29;
				}
				else{
					rt = (int)(instructions[line][18]) - (int)'0' + 20;
				}
			}
			else if(instructions[line][17] == 'z'){
				rt = 0;
			}
		}
		else{
			rt = 100;
		}
		
		//STORE/LOAD WORD OPERATION
		if(instructions[line].find("sw ") != string::npos || instructions[line].find("lw ") != string::npos ){
			int i = 11;
			string offsetStr;
			int offset;
			//Get the integer value before '(' for the offset
			while(instructions[line][i] != '('){
				offsetStr += instructions[line][i++];
			}
			i += 2;
			//Get the base address
			if(instructions[line][i] == 't'){
				rs = (int)(instructions[line][i+1]) - (int)'0' + 10;
			}
			else if(instructions[line][i] == 's'){
				if(instructions[line][i+1] == 'p'){
					rs = 29;
				}
				else{
					rs = (int)(instructions[line][i+1]) - (int)'0' + 20;
				}
			}
			//Convert offset string to integer
			for(int i=0; i<offsetStr.length(); i++){
				offset += ((int)offsetStr[i] - (int)'0')*pow(10,offsetStr.length()-i-1);
			}
			
			//Output the relevant machine code for sw/lw
			if(instructions[line].find("sw ") != string::npos){
				MachineCode = (43<<26) + (rs<<21) + (rd<<16) + offset;
			}
			else{
				MachineCode = (35<<26) + (rs<<21) + (rd<<16) + offset;
			}
		}
		//ADD OPERATION
		else if(instructions[line].find("add ") != string::npos){
			MachineCode = 32 + (rd << 11) + (rt << 16) + (rs << 21);
		}
		//SUB OPERATION
		else if(instructions[line].find("sub ") != string::npos){
			MachineCode = 34 + (rd << 11) + (rt << 16) + (rs << 21);
		}
		//SHIFT-LESS-THAN OPERATION
		else if(instructions[line].find("slt ") != string::npos){
			MachineCode = 42 + (rd << 11) + (rt << 16) + (rs << 21);
		}
		//JUMP OPERATION
		else if(instructions[line].find("j ") != string::npos){
			int JumpDestLine;
			//Get the jump target branch label in the instruction
			string jump = instructions[line].substr(6, instructions[line].size());
			//Match that with all labels saved in the JumpDest array
			for(int i=0; i<(JumpDestLast + 1); i++){
				//When found, set the target line to the line number of the first
				//instruction of the branch
				if(jump == JumpDest[i]){
					JumpDestLine = JumpDestInt[i];
					break;
				}
			}
			//Get the value of PC + 4
			bitset<32> linebits((line+1) << 2);
			//Retain (PC+4)[31-28] only to be concanated with JumpDestLine
			for(int i=0; i<28; i++){
				linebits[i] = 0;
			}
			int lineUpper = (int)(linebits.to_ulong());
			int offset = JumpDestLine - lineUpper;
			MachineCode = (2 << 26) + offset;
		}
		//ADD/SLT IMMEDIATE OPERATION
		else if(instructions[line].find("addi ") != string::npos || instructions[line].find("slti ") != string::npos){
			string addValStr;
			int addVal = 0;
			//deals with negative values by storing in twos compliment form
			if(instructions[line][16] == '-'){
				for(int i=17; i<instructions[line].length(); i++){
					addValStr += instructions[line][i];
				}
				for(int i=0; i<addValStr.length(); i++){
					addVal += ((int)addValStr[i] - (int)'0')*pow(10,addValStr.length()-i-1);
				}
				addVal = (twos ^ addVal)+1;
			}
			else{
				for(int i=16; i<instructions[line].length(); i++){
					addValStr += instructions[line][i];
				}
				for(int i=0; i<addValStr.length(); i++){
					addVal += ((int)addValStr[i] - (int)'0')*pow(10,addValStr.length()-i-1);
				}
			}
			//Set up the respective machine code
			if(instructions[line].find("addi ") != string::npos){
				MachineCode = (8<<26) + (rs<<21) + (rd<<16) + addVal;
			}
			else{
				MachineCode = (10<<26) + (rs<<21) + (rd<<16) + addVal;
			}
		}
		//AND OPERATION
		else if(instructions[line].find("and ") != string::npos){
			MachineCode = (rs<<21) + (rt<<16) + (rd<<11) + 36;
		}
		//BRANCH OPERATIONS
		else if(instructions[line].find("beq ") != string::npos || instructions[line].find("bne ") != string::npos){
			int JumpDestLine;
			string jump = instructions[line].substr(16, instructions[line].size());
			//Find the branch label in JumpDest array and set target to the
			//line number of the first instruction of that branch
			for(int i=0; i<(JumpDestLast + 1); i++){
				if(jump == JumpDest[i]){
					JumpDestLine = JumpDestInt[i];
					break;
				}
			}
			int offset = JumpDestLine - line - 1;
			//Deal with negative offsets
			//If jumping to previous lines
			if(offset < 0){
				offset = -offset;
				offset = (twos xor offset) + 1;
			}
			
			//Set up the respective machine code
			if(instructions[line].find("beq ") != string::npos){
				MachineCode = (4<<26) + (rd<<21) + (rs<<16) + offset;
			}
			else{
				MachineCode = (5<<26) + (rd<<21) + (rs<<16) + offset;
			}
		}
		
		//SHIFT LEFT LOGICAL OPERATION
		else if(instructions[line].find("sll ") != string::npos){
			string shamtStr;
			int shamt = 0;
			//Get shift amount
			for(int i=16; i<instructions[line].length(); i++){
				shamtStr += instructions[line][i];
			}
			//Convert to integer
			for(int i=0; i<shamtStr.length(); i++){
				shamt += ((int)shamtStr[i] - (int)'0')*pow(10,shamtStr.length()-i-1);
			}
			MachineCode = (rs<<16) + (rd<<11) + (shamt<<6);
		}
		
		//Store the machine code for each instruction in MemoryText
		unsigned tempDeduct = 0;
		unsigned MemoryInverse[4];
		for(int i=0; i<4; i++){
			unsigned MachineCodeTemp = (MachineCode >>(32-8*(i+1))) - tempDeduct;
			tempDeduct = ((tempDeduct + MachineCodeTemp) << 8);
			MemoryInverse[i] = MachineCodeTemp;
		}
		for(int i=0; i<4; i++){
			MemoryText[line*4 + i] = MemoryInverse[3-i];
		}
	}
}

void IF(unsigned PC, unsigned* MemoryText, unsigned* IR, unsigned* PCnext){
	//store the hex values from memory temporarily
	unsigned IRTemp[4];
	for(int i=0; i<4; i++){
		IRTemp[i] = MemoryText[PC + i];
	}
	//convert into an array of 32 bit binary
	for(int i = 0; i<32; i++){
		IR[i] = IRTemp[(i)/8]%2;
		IRTemp[(i)/8]/=2;
	}
	//increment PC by 4 for the next instruction
	//PC not written to yet until the end of the cycle
	*PCnext = PC + 4;
}

void ID(unsigned* IR, unsigned* Register, unsigned *RegA, unsigned *RegB, unsigned *RegC, 
		unsigned* A, unsigned* B, unsigned *ALUout, unsigned* PCnext, unsigned* OpCode, unsigned* Function, 
		unsigned* ShiftAmount, unsigned* MemOffset, bool* ALUAccess, bool* MemAccess, bool* RegWrite, bool* StackAccess){
	//declare a temporary integer for calculating register addresses
	int RegAddress = 0;
	
	//Calc register address for A
	for(int i = 0; i<5; i++){
		RegAddress += IR[21+i]*pow(2, i);
	}
	*RegA = RegAddress;
	*A = Register[*RegA];
	//Calc register address for B
	RegAddress = 0;
	for(int i = 0; i<5; i++){
		RegAddress += IR[16+i]*pow(2, i);
	}
	*RegB = RegAddress;
	*B = Register[*RegB];
	
	//Calc register address for destination
	RegAddress = 0;
	for(int i = 0; i<5; i++){
		RegAddress += IR[11+i]*pow(2, i);
	}
	*RegC = RegAddress;
	
	//Calc branch address to be used later if it is a branch instruction
	//or offset for load/store if it is a lw or sw instruction
	RegAddress = 0;
	for(int i = 0; i<16; i++){
		RegAddress += IR[i]*pow(2,i);
	}
	//Set offset
	*MemOffset = RegAddress;
	//Set branch memory address
	if(RegAddress > pow(2, 15)){
		RegAddress = (twos xor RegAddress) + 1;
		RegAddress = -RegAddress;
	}
	*ALUout = *PCnext + RegAddress*4;
	
	//Get Op Code
	RegAddress = 0;
	for(int i = 0; i<6; i++){
		RegAddress += IR[26+i]*pow(2,i);
	}
	*OpCode = RegAddress;
	//Get Function for R type instructions
	RegAddress = 0;
	for(int i = 0; i<6; i++){
		RegAddress += IR[i]*pow(2,i);
	}
	*Function = RegAddress;
	
	//Get ShiftAmount for shift instructions
	RegAddress = 0;
	for(int i = 0; i<5; i++){
		RegAddress += IR[6+i]*pow(2,i);
	}
	*ShiftAmount = RegAddress;
	
	//Setup new PCnext value for Jump Instruction
	if(*OpCode == 2){
		//Get jump address for jump instructions
		RegAddress = 0;
		for(int i = 0; i<26; i++){
			RegAddress += IR[i]*pow(2,i);
		}
		unsigned JumpAddress = RegAddress;
		//get PC[31:28]
		bitset<32> PCbits(*PCnext);
		for(int i=0; i<28; i++){
			PCbits[i] = 0;
		}
		//PC = PC[31:28] | IR[26:0] << 2
		*PCnext = (int)(PCbits.to_ulong()) + JumpAddress*4;
	}
	
	//setup control signals
	else{
		*ALUAccess = true;
		if(*OpCode == 43 || *OpCode == 35){
			*MemAccess = true;
			if(*RegA == 29){
				*StackAccess = true;
			}
		}
		if(*OpCode == 0 || *OpCode == 35 || *OpCode == 8 || *OpCode == 10){
			*RegWrite = true;
		}
	}
}

void EX(unsigned* ALUout, unsigned RegA, unsigned RegB, unsigned* RegC, unsigned A, unsigned B, unsigned OpCode, unsigned Function, 
		unsigned ShiftAmount, unsigned MemOffset, unsigned* PCnext){
	//R Type
	if(OpCode == 0){
		//ADD
		if(Function == 32){
			*ALUout = A + B;
		}
		//SUB
		else if(Function == 34){
			*ALUout = A - B;
		}
		//AND
		else if(Function == 36){
			*ALUout = A & B;
		}
		//SLL
		else if(Function == 0){
			*ALUout = (B<<ShiftAmount);
		}
		//SLT
		else if(Function == 42){
			if(A < B){
				*ALUout = 1;
			}
			else{
				*ALUout = 0;
			}
		}
	}
	
	//ADDI
	else if(OpCode == 8){
		signed ADDIoffset;
		//if offset is negative
		if(MemOffset > pow(2,15)){
			//offset = -((2^16-1) xor offset) - 1
			ADDIoffset = -((twos^MemOffset) + 1);
		}
		else{
			ADDIoffset = MemOffset;
		}
		*ALUout = A + ADDIoffset;
		//Because WB writes to RegC
		//Destination of ADDI saved in RegB
		//Hence, set RegC = RegB for WB later
		*RegC = RegB;
	}
	
	//SLTI
	else if(OpCode == 10){
		signed CompareVal;
		//if offset is negative
		if(MemOffset > pow(2,15)){
			//offset = -((2^16-1) xor offset) - 1
			CompareVal = -((twos^MemOffset) + 1);
		}
		else{
			CompareVal = MemOffset;
		}
		if(A<CompareVal){
			*ALUout = 1;
		}
		else{
			*ALUout = 0;
		}
		//Because WB writes to RegC
		//Destination of SLTI saved in RegB
		//Hence, set RegC = RegB for WB later
		*RegC = RegB;
	}
	
	//load word or store word
	else if(OpCode == 35 or OpCode == 43){
		//Address for loading/storing is A + offset
		*ALUout = A + MemOffset;
	}
	
	//BEQ
	else if(OpCode == 4){
		//if A=B, branch to new memory address
		if(A == B){
			*PCnext = *ALUout;
		}
	}
	
	//BNE
	else if(OpCode == 5){
		//if A!=B, branch to new memory address
		if(A != B){
			*PCnext = *ALUout;
		}
	}
}

void MEM(unsigned *MemoryData, unsigned* MemoryStack, unsigned* ALUout, unsigned OpCode, unsigned B, unsigned* RegC, unsigned RegB, bool* StackAccess){
	//store word
	if(OpCode == 43){
		unsigned tempDeduct = 0;
		unsigned MemoryInverse[4];
		//split the value to be stored into 4 hex values
		for(int i=0; i<4; i++){
			unsigned BTemp = (B >>(32-8*(i+1))) - tempDeduct;
			tempDeduct = ((tempDeduct + BTemp) << 8);
			MemoryInverse[i] = BTemp;
		}
		//if the pointer is $sp, store in the stack
		if(*StackAccess){
			for(int i=0; i<4; i++){
				MemoryStack[*ALUout + i] = MemoryInverse[3-i];
			}
			*StackAccess = false;
		}
		//if not, store in data segment
		else{
			for(int i=0; i<4; i++){
				MemoryData[*ALUout + i] = MemoryInverse[3-i];
			}
		}
	}

	//load word
	else if(OpCode == 35){
		unsigned ALUoutTemp = 0;
		//If pointer is $sp, load from stack
		if(*StackAccess){
			for(int i=0; i<4; i++){
				ALUoutTemp += (MemoryStack[*ALUout + i] << 8*i);
			}
			*StackAccess = false;
		}
		//if not, load from data segment
		else{
			for(int i=0; i<4; i++){
				ALUoutTemp += (MemoryData[*ALUout + i] << 8*i);
			}
		}
		*ALUout = ALUoutTemp;
		//Set the value of RegC to RegB
		//Because WB writes to RegC, destination of lw saved in RegB
		*RegC = RegB;
	}
}

void WB(unsigned OpCode, unsigned ALUout, unsigned RegC, unsigned* Register){
	//For R type, LW, SLTI or ADDI instructions
	Register[RegC] = ALUout;
}