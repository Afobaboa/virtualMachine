/**
 * @file
 * This header provides you an interface to assemble your file with 
 * virtual machine's code with instructions to machine code for this virtual machine.
 */

#ifndef ASSEMBLER_H
#define ASSEMBLER_H


//--------------------------------------------------------------------------------------------------


/**
 * Assemble code from file to machine code.
 * Your code file must have .asm extension like *name*.asm .
 * Machin code file will have .vm extension and the same name: *name*.vm .
 * 
 * @param fileName Name of file with code. Don't forget .asm extension!
 * 
 * @return true if assembling is complete,
 * @return false if there is an error. Error will be printed to terminal. 
 *         More detailed information about error you can find in logs/log.txt .
 */
bool Assemble(const char* fileName);


//--------------------------------------------------------------------------------------------------


#endif // ASSEMBLER_H