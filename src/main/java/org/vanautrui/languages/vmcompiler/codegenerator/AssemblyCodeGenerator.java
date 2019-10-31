package org.vanautrui.languages.vmcompiler.codegenerator;

import org.vanautrui.languages.vmcompiler.AssemblyWriter;
import org.vanautrui.languages.vmcompiler.instructions.VMInstr;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.ThreadLocalRandom;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;

import static java.lang.Integer.parseInt;
import static java.util.stream.Collectors.groupingBy;
import static org.vanautrui.languages.vmcompiler.codegenerator.ArithmeticFocusedAssemblyCodeGenerator.*;
import static org.vanautrui.languages.vmcompiler.codegenerator.ArrayFocusedAssemblyCodeGenerator.compile_arrayread;
import static org.vanautrui.languages.vmcompiler.codegenerator.ArrayFocusedAssemblyCodeGenerator.compile_arraystore;
import static org.vanautrui.languages.vmcompiler.codegenerator.LogicFocusedAssemblyCodeGenerator.*;
import static org.vanautrui.languages.vmcompiler.codegenerator.StackFocusedAssemblyCodeGenerator.*;
import static org.vanautrui.languages.vmcompiler.codegenerator.SubroutineFocusedAssemblyCodeGenerator.*;
import static org.vanautrui.languages.vmcompiler.model.Register.*;

public final class AssemblyCodeGenerator {

    //https://en.wikipedia.org/wiki/X86_instruction_listings

    static long unique(){
        //returns 0 <= x <= Long.MAX_VALUE
        return ThreadLocalRandom.current().nextLong(0,Long.MAX_VALUE);
    }

    public static List<String> compileVMCode(final List<String> vmcode) throws Exception {
        //clean the vm code
        final List<String> clean_vm_codes = clean_vm_code(vmcode);

        final List<VMInstr> VMInstrs = parseVMInstrs(clean_vm_codes);

        //generate assembly

        return vm_codes_to_assembly(VMInstrs);
    }

    private static List<VMInstr> parseVMInstrs(final List<String> clean_vm_codes) {
        //maybe it is not beneficial for performance to start a new thread
        //to construct each VMInstr. as staring a thread has its own overhead.
        //maybe this solution for chunking could help:
        //https://stackoverflow.com/questions/27583623/is-there-an-elegant-way-to-process-a-stream-in-chunks

        AtomicInteger counter = new AtomicInteger();
        final int chunkSize = 1000;

        final List<VMInstr> vmInstrs = Collections.synchronizedList(new ArrayList<>());

        clean_vm_codes.stream()
                .collect(groupingBy(x->counter.getAndIncrement()/chunkSize))
                .values()
                .parallelStream().map(list->
                    list.stream().map(VMInstr::new).collect(Collectors.toList())
                ).forEach(vmInstrs::addAll);

        //return clean_vm_codes.parallelStream().map(VMInstr::new).collect(Collectors.toList());

        return vmInstrs;
    }

    private static void compile_iconst(final VMInstr instr, final AssemblyWriter a) {
        final int x = parseInt(instr.arg1.get());

        a.push(x, "iconst");
    }

    private static void compile_fconst(final VMInstr instr, final AssemblyWriter a) {

        final float f = Float.parseFloat(instr.arg1.get());
        final String comment = "fconst";

        a.any("mov "+eax+", __float32__("+f+")",comment);
        a.push(eax, comment);
    }

    private static void compile_vm_instr(final VMInstr instr, final AssemblyWriter a) throws Exception {

        switch (instr.cmd) {
            //stack related commands

            //constants
            case "iconst":
                compile_iconst(instr, a);
                break;
            case "fconst":
                compile_fconst(instr, a);
                break;
            case "cconst":
                compile_cconst(instr, a);
                break;


            case "pop":
                Optional<Integer> index = Optional.empty();
                if (instr.arg2.isPresent()) {
                    index = Optional.of(parseInt(instr.arg2.get()));
                }
                compile_pop(instr.arg1, index, a);
                break;
            case "push":
                compile_push(instr.arg1.get(), parseInt(instr.arg2.get()), a);
                break;
            case "dup":
                compile_dup(instr, a);
                break;
            case "swap":
                compile_swap("", a);
                break;

            //subroutine related commands
            case "subroutine":
                compile_subroutine(instr.arg1.get(), a);
                break;
            case "call":
                compile_call(instr, a);
                break;
            case "return":
                compile_return(a);
                break;
            case "exit":
                compile_exit(a);
                break;

            //function programming related
            case "pushsubroutine":
                compile_pushsubroutine(instr, a);
                break;
            case "callfromstack":
                compile_callfromstack(instr, a);
                break;

            //floating point arithmetic commands
            case "fadd":
                compile_fadd(a);
                break;
            case "fsub":
                compile_fsub(a);
                break;

            //TODO: prefix with 'i', so everyone knows it is about integers
            //integer arithmetic commands
            case "add":
                compile_add(instr, a);
                break;
            case "sub":
                compile_sub(instr, a);
                break;
            case "mul":
                compile_mul(instr, a);
                break;
            case "div":
                compile_div(instr, a);
                break;
            case "mod":
                compile_mod(instr, a);
                break;
            case "neg":
                compile_neg(instr, a);
                break;

            //TODO: prefix lt, gt, ... with 'i_' so we know that it is for integers
            //logic commands
            case "eq":
                compile_eq(a);
                break;
            case "f_eq":
                compile_f_eq(a);
                break;
            case "gt":
                compile_gt(a);
                break;
            case "f_gt":
                compile_f_gt(a);
                break;
            case "geq":
                compile_geq(a);
                break;
            case "lt":
                compile_lt(a);
                break;
            case "f_lt":
                compile_f_lt(a);
                break;
            case "leq":
                compile_leq(a);
                break;
            case "not":
                compile_not(a);
                break;
            case "and":
                compile_and(a);
                break;
            case "or":
                compile_or(a);
                break;

            //inc,dec
            case "inc":
                compile_inc(a);
                break;
            case "dec":
                compile_dec(a);
                break;

            //control flow
            case "goto":
                a.jmp(instr.arg1.get());
                break;
            case "if-goto":
                compile_if_goto(instr, a);
                break;
            case "label":
                a.label(instr.arg1.get());
                break;

            //array related
            case "arraystore":
                compile_arraystore(instr, a);
                break;
            case "arrayread":
                compile_arrayread(instr, a);
                break;

            //bit shifting
            case "lshiftl":
                compile_lshiftl(instr, a);
                break;
            case "lshiftr":
                compile_lshiftr(instr, a);
                break;

            //unhandled cases
            default:
                throw new Exception("unrecognized vm instr " + instr.cmd);
        }
    }


    private static void compile_lshiftr(final VMInstr instr, final AssemblyWriter a) {

        final String cmt = instr.toString();
        a.pop(ecx, cmt);
        a.pop(eax, cmt);
        a.shr(eax, cl, cmt);
        a.push(eax, cmt);
    }

    private static void compile_lshiftl(final VMInstr instr, final AssemblyWriter a) {

        final String cmt = instr.toString();
        a.pop(ecx, cmt);
        a.pop(eax, cmt);
        a.shl(eax, cl, cmt);
        a.push(eax, cmt);
    }


    private static void compile_pushsubroutine(final VMInstr instr, final AssemblyWriter a) {
        final String subrName = instr.arg1.get();
        a.push(subrName, instr.toString());
    }



    private static void compile_not(final AssemblyWriter a) {
        //logical not
        //if there is a 1 (true) on the stack, after this vm command, there must be a 0 (false) on the stack
        //if there is a 0 (false) on the stack, after this vm command, there must be a 1 (true) on the stack

        //https://www.tutorialspoint.com/assembly_programming/assembly_logical_instructions.htm

        final String comment = "not";

        a.mov(ebx, 1, comment);
        a.pop(eax, comment);
        a.xor(eax, ebx, comment);
        a.push(eax, comment);
    }


    private static void compile_dec(final AssemblyWriter a) {
        a.pop(eax, "dec");
        a.dec(eax, "dec");
        a.push(eax, "dec");
    }

    private static void compile_inc(final AssemblyWriter a) {
        final String comment = "inc";
        a.pop(eax, comment);
        a.inc(eax, comment);
        a.push(eax, comment);
    }



    private static List<String> vm_codes_to_assembly(final List<VMInstr> vmcodes) throws Exception {
        //receives only clean VM Codes

        final AssemblyWriter a = new AssemblyWriter();

        a.section(".text", "must be declared for linker (ld)");
        a.global("_start", "");
        a.label("_start", "tell linker entry point");

        a.nop(); //the 2 nops are recommended by my assembly book

        //at the start of the code, jump to main
        a.jmp("Main_main");


        for (final VMInstr instr : vmcodes) {
            compile_vm_instr(instr, a);
        }

        //add the builtin subroutines
        BuiltinSubroutinesInAssembly.compile_all_builtin_subroutines(a);

        a.nop(); //the 2 nops are recommended by my assembly book

        a.section(".data", "");
        //a.any("achar db '0'","");

        a.section(".bss", "");
        //a.any("buffer:	resb	64","");

        ////https://cs.lmu.edu/~ray/notes/nasmtutorial/
        ////https://stackoverflow.com/questions/8201613/printing-a-character-to-standard-output-in-assembly-x86
        return a.getAssemblyProgram();
    }

    private static void compile_exit(final AssemblyWriter a) {
        a.mov(eax, 1, "exit: sytem call number (sys_exit)");
        a.pop(ebx, "exit: pop exit status code from stack");
        a.call_kernel();
    }

    private static void compile_cconst(final VMInstr instr, final AssemblyWriter a) {

        final char c = instr.arg1.get().charAt(0);
        a.push(c, instr.toString());
    }


    private static void compile_if_goto(final VMInstr instr, final AssemblyWriter a) {

        final String comment = "if-goto";

        a.pop(eax, comment); //pops the condition into eax
        a.mov(ebx, 1, comment);  //ebx := true==1
        a.cmp(eax, ebx, comment);
        a.je(instr.arg1.get(), comment); //jumps, if eax==ebx
    }





    private static List<String> clean_vm_code(final List<String> vmcodes) {
        //remove comments and empty lines
        //and remove indentation

        final AtomicInteger counter = new AtomicInteger();
        final int chunkSize = 1000;

        final List<String> clean_vm_codes = Collections.synchronizedList(new ArrayList<>());

        vmcodes.stream()
                .collect(groupingBy(x->counter.getAndIncrement()/chunkSize))
                .values()
                .parallelStream()
                .map(list->
                    list
                        .stream()
                        .filter(instr->!instr.trim().isEmpty()) //filter out empty instructions
                        .map(instr->{
                            //map to instruction without comments

                            if(instr.contains("//")){
                                return instr.substring(0,instr.indexOf("//"));
                            }
                            return instr;
                        })
                        .map(String::trim)
                        .collect(Collectors.toList())
        ).forEach(clean_vm_codes::addAll);

        return clean_vm_codes;
    }
}
