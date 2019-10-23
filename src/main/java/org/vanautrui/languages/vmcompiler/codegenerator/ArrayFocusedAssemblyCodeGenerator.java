package org.vanautrui.languages.vmcompiler.codegenerator;

import org.vanautrui.languages.vmcompiler.AssemblyWriter;
import org.vanautrui.languages.vmcompiler.instructions.VMInstr;

import static org.vanautrui.languages.vmcompiler.model.Register.*;

public final class ArrayFocusedAssemblyCodeGenerator {

    public static void compile_arrayread(VMInstr instr, AssemblyWriter a) {
        /*
         arrayread
        //stack looks like:
        //|undefined
        //|array address
        //|array index <- esp
        ////after execution of this command, stack looks like
        //|undefined
        //|array[index] <-esp
        //meaning this vm command reads from an array, and places the value on the stack

         */
        a.pop(ebx, instr.toString()); //array index

        a.inc(ebx,instr.toString()); //as our arrays are length-prefixed, the index must be offset by one

        a.pop(eax, instr.toString()); //array address in memory
        a.add(eax, ebx, instr.toString()); //address of the value we want

        a.dereference(eax, instr.toString()); //eax = memory[eax] <=> mov eax,[eax]

        a.push(eax, instr.toString()); //push the value
    }

    /**
     * Documentation for the behavior, expected stack state before and after are to be found in
     * spec/dracovm-specification.txt
     */
    public static void compile_arraystore(VMInstr instr, AssemblyWriter a) {

        a.pop(ebx, instr.toString()); //value to store //-1

        a.pop(ecx, instr.toString()); //index //-1

        a.inc(ecx,instr.toString()); //index must be offset by 1, as our arrays are length-prefixed

        a.pop(eax, instr.toString()); //array_address //-1

        //address to store into = addray_address + index
        a.add(eax, ecx, instr.toString()); //0

        a.store_second_into_memory_location_pointed_to_by_first(eax, ebx, instr.toString()); //0
    }
}
