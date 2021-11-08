# mips-simulator

#general comments on the simulator
"simulator.cpp" consists of a Multi-Cycle MIPS Processor that is also capable of translating assembly code into machine code before the instructions are fed into the processor

The translator is not written very efficiently (might be a bit messy) as the main objective of the project was to develop a MIPS processor
i.e., the translator is added to aid and simplify the usage of the processor

The simulator is not capable of processing the entire MIPS instruction set. Below is the instruction set that is implemented:
#add rd, rs, rt
#and rd, rs, rt
#slt rd, rs, rt
#sub rd, rs, rt
#sll rd, rt sa
#addi rt, rs, imm
#slti rt, rs, imm
#lw rt, offset(rs)
#sw rt, offset(rs)
#beq rs, rt, offset
#bne rs, rt, offset
#j target

The memory layout of the processor is as follows:
MemoryText[400]: Text Segment (Program Code)
MemoryData[400]: Data Segment (Variable Storage)
MemoryStack[200]: Stack Segment (Automatic Storage)


#output of simulator
The output that is printed when all instructions are ran is currently tailored for the instructions in "instructions.txt". Change appropriately to test the processor with other instructions

#writing assembly code
Ensure that any new assembly code written in a txt file follows the same format as those in "instructions.txt" or "test.txt"

Change the name of the file being read in the simulator


#about instructions.txt
instructions.txt contains the assembly code that squares the integers 1 to 99 and store them in the processor's memory unit. Check "squareFunct.cpp" for the implementation of this code in c++
