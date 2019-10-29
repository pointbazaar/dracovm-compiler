package org.vanautrui.languages.vmcompiler.codegenerator;

import org.vanautrui.languages.vmcompiler.AssemblyWriter;
import org.vanautrui.languages.vmcompiler.instructions.VMInstr;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

import static java.lang.Integer.parseInt;
import static org.vanautrui.languages.vmcompiler.codegenerator.ArrayFocusedAssemblyCodeGenerator.compile_arrayread;
import static org.vanautrui.languages.vmcompiler.codegenerator.ArrayFocusedAssemblyCodeGenerator.compile_arraystore;
import static org.vanautrui.languages.vmcompiler.codegenerator.StackFocusedAssemblyCodeGenerator.*;
import static org.vanautrui.languages.vmcompiler.codegenerator.SubroutineFocusedAssemblyCodeGenerator.*;
import static org.vanautrui.languages.vmcompiler.model.Register.*;

public final class AssemblyCodeGenerator {

    //https://en.wikipedia.org/wiki/X86_instruction_listings

    public static List<String> compileVMCode(final List<String> vmcode) throws Exception {
        //clean the vm code
        final List<String> clean_vm_codes = clean_vm_code(vmcode);

        final List<VMInstr> VMInstrs = parseVMInstrs(clean_vm_codes);

        //generate assembly

        return vm_codes_to_assembly(VMInstrs);
    }

    private static List<VMInstr> parseVMInstrs(final List<String> clean_vm_codes) {
        return clean_vm_codes.stream().map(VMInstr::new).collect(Collectors.toList());
    }

    private static void compile_iconst(final VMInstr instr, final AssemblyWriter a) {
        final int x = parseInt(instr.arg1.get());

        a.push(x, instr.toString());
    }

    private static void compile_fconst(final VMInstr instr, final AssemblyWriter a) {
        final float f = Float.parseFloat(instr.arg1.get());

        a.push(f, instr.toString());
    }

    private static void compile_vm_instr(final VMInstr instr, final AssemblyWriter a) throws Exception {

        switch (instr.cmd) {
            //stack related commands
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
                compile_exit(instr, a);
                break;

            //function programming related
            case "pushsubroutine":
                compile_pushsubroutine(instr, a);
                break;
            case "callfromstack":
                compile_callfromstack(instr, a);
                break;

            //arithmetic commands
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

            //logic commands
            case "eq":
                compile_eq(instr, a);
                break;
            case "gt":
                compile_gt(instr, a);
                break;
            case "geq":
                compile_geq(instr,a);
                break;
            case "lt":
                compile_lt(instr, a);
                break;
            case "leq":
                compile_leq(instr,a);
                break;
            case "not":
                compile_not(instr, a);
                break;
            case "and":
                compile_and(instr,a);
                break;
            case "or":
                compile_or(instr,a);
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

    private static void compile_and(final VMInstr instr, final AssemblyWriter a) {
        final String comment = instr.toString();

        a.pop(ebx, comment);
        a.pop(eax, comment);
        a.and(eax,ebx,comment);
        a.push(eax,comment);
    }

    private static void compile_or(final VMInstr instr, final AssemblyWriter a) {
        final String comment = instr.toString();

        a.pop(ebx, comment);
        a.pop(eax, comment);
        a.or(eax,ebx,comment);
        a.push(eax,comment);
    }

    private static void compile_leq(final VMInstr instr, final AssemblyWriter a) {

        final String comment = "leq";

        final String labeltrue = ".leq_push1";
        final String labelend = ".leq_end";

        a.pop(ebx, comment);
        a.pop(eax, comment);
        a.cmp(eax, ebx, comment);
        a.jle(labeltrue, comment);

        //push 0 (false)
        a.push(0, comment);
        a.jmp(labelend, comment);

        a.label(labeltrue, comment);
        //push 1 (true)
        a.push(1, comment);

        a.label(labelend, comment);
    }

    private static void compile_geq(final VMInstr instr, final AssemblyWriter a) {
        final String comment = "geq";
        final String labeltrue = ".geq_push1";
        final String labelend = ".geq_end";

        a.pop(ebx, comment);
        a.pop(eax, comment);
        a.cmp(eax, ebx, comment);
        a.jge(labeltrue, comment);

        //push 0 (false)
        a.push(0, comment);
        a.jmp(labelend, comment);

        a.label(labeltrue, comment);
        //push 1 (true)
        a.push(1, comment);

        a.label(labelend, comment);
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

    /**
     * compiles the 'mod' dracovm instruction to assembly code
     */
    private static void compile_mod(final VMInstr instr, final AssemblyWriter a) {
        //https://stackoverflow.com/questions/8021772/assembly-language-how-to-do-modulo
        final String comment = instr.toString();
        a.pop(ebx, comment);
        a.pop(eax, comment);


        a.xor(edx,edx,comment); //dividend high half = 0
        //sign-extend eax in edx, in case eax is negative
        //https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
        a.cdq(instr.toString());

        a.idiv_eax_by(ebx, comment);
        a.push(edx, comment);
    }

    private static void compile_not(final VMInstr instr, final AssemblyWriter a) {
        //logical not
        //if there is a 1 (true) on the stack, after this vm command, there must be a 0 (false) on the stack
        //if there is a 0 (false) on the stack, after this vm command, there must be a 1 (true) on the stack

        //https://www.tutorialspoint.com/assembly_programming/assembly_logical_instructions.htm

        final String comment = instr.toString();

        a.mov(ebx, 1, comment);
        a.pop(eax, comment);
        a.xor(eax, ebx, comment);
        a.push(eax, comment);
    }

    private static void compile_div(final VMInstr instr, final AssemblyWriter a) {
        a.pop(ecx,instr.toString()); //pop the divisor
        a.pop(eax,instr.toString()); //pop the dividend

        a.xor(edx,edx,instr.toString()); //dividend high half=0
        //sign-extend eax in edx, in case eax is negative
        //https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
        a.cdq(instr.toString());

        a.idiv_eax_by(ecx,instr.toString());

        a.push(eax,instr.toString()); //push the quotient
    }

    private static void compile_mul(final VMInstr instr, final AssemblyWriter a) {

        a.pop(ebx, instr.toString());
        a.pop(eax, instr.toString());

        a.mul_eax_with(ebx, instr.toString());
        a.push(eax, instr.toString());
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


    private static void compile_lt(final VMInstr instr, final AssemblyWriter a) {

        final String comment = instr.toString();

        final String labeltrue = ".lt_push1";
        final String labelend = ".lt_end";

        a.pop(ebx, comment);
        a.pop(eax, comment);
        a.cmp(eax, ebx, comment);
        a.jl(labeltrue, comment);

        //push 0 (false)
        a.push(0, comment);
        a.jmp(labelend, comment);

        a.label(labeltrue, comment);
        //push 1 (true)
        a.push(1, comment);

        a.label(labelend, comment);
    }

    private static void compile_gt(final VMInstr instr, final AssemblyWriter a) {
        final String comment = "gt";
        final String labeltrue = ".gt_push1";
        final String labelend = ".gt_end";

        a.pop(ebx, comment);
        a.pop(eax, comment);
        a.cmp(eax, ebx, comment);
        a.jg(labeltrue, comment);

        //push 0 (false)
        a.push(0, comment);
        a.jmp(labelend, comment);

        a.label(labeltrue, comment);
        //push 1 (true)
        a.push(1, comment);

        a.label(labelend, comment);
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

    private static void compile_exit(final VMInstr instr, final AssemblyWriter a) {
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

    private static void compile_eq(final VMInstr instr, final AssemblyWriter a) {
        final String comment = "eq";

        final String labeltrue = ".eq_push1";
        final String labelend = ".eq_end";

        a.pop(eax, comment);
        a.pop(ebx, comment);
        a.cmp(eax, ebx, comment);
        a.je(labeltrue, comment);

        //push 0 (false)
        a.push(0, comment);
        a.jmp(labelend, comment);

        a.label(labeltrue, comment);
        //push 1 (true)
        a.push(1, comment);

        a.label(labelend, comment);
    }

    private static void compile_neg(final VMInstr instr, final AssemblyWriter a) {
        a.pop(eax, instr.toString());
        a.mov(ebx, -1, instr.toString());
        a.mul_eax_with(ebx, instr.toString());
        a.push(eax, instr.toString());
    }

    private static void compile_sub(final VMInstr instr, final AssemblyWriter a) {
        a.pop(ebx, instr.toString());
        a.pop(eax, instr.toString());
        a.sub(eax, ebx, instr.toString());
        a.push(eax, instr.toString());
    }

    private static void compile_add(final VMInstr instr, final AssemblyWriter a) {
        a.pop(eax, instr.toString());
        a.pop(ebx, instr.toString());
        a.add(eax, ebx, instr.toString());
        a.push(eax, instr.toString());
    }


    private static List<String> clean_vm_code(final List<String> vmcodes) {
        //remove comments and empty lines
        //and remove indentation

        final List<String> result = new ArrayList<>();

        for (final String instr : vmcodes) {

            String instr_to_be_cleaned_of_comments = new String(instr);
            if (instr.contains("//")) {
                instr_to_be_cleaned_of_comments = instr_to_be_cleaned_of_comments.substring(0, instr.indexOf("//"));
            }
            instr_to_be_cleaned_of_comments = instr_to_be_cleaned_of_comments.trim();

            if (!instr_to_be_cleaned_of_comments.isEmpty()) {
                result.add(instr_to_be_cleaned_of_comments);
            }
        }

        return result;
    }
}
