package org.vanautrui.languages.vmcompiler.codegenerator;

import org.vanautrui.languages.vmcompiler.AssemblyWriter;
import org.vanautrui.languages.vmcompiler.instructions.VMInstr;

import static org.vanautrui.languages.vmcompiler.model.Register.*;
import static org.vanautrui.languages.vmcompiler.model.Register.eax;

final class ArithmeticFocusedAssemblyCodeGenerator {

  static void compile_div(final VMInstr instr, final AssemblyWriter a) {
    a.pop(ecx,instr.toString()); //pop the divisor
    a.pop(eax,instr.toString()); //pop the dividend

    a.xor(edx,edx,instr.toString()); //dividend high half=0
    //sign-extend eax in edx, in case eax is negative
    //https://www.aldeid.com/wiki/X86-assembly/Instructions/cdq
    a.cdq(instr.toString());

    a.idiv_eax_by(ecx,instr.toString());

    a.push(eax,instr.toString()); //push the quotient
  }

  static void compile_mul(final VMInstr instr, final AssemblyWriter a) {

    a.pop(ebx, instr.toString());
    a.pop(eax, instr.toString());

    a.mul_eax_with(ebx, instr.toString());
    a.push(eax, instr.toString());
  }

  static void compile_fmul(final AssemblyWriter a){
    //https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
    final String comment = "fmul";

    //load 2 arguments
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);

    a.fmul(comment);
    a.fstp(comment);
    a.push(eax,comment);
  }


  static void compile_sub(final VMInstr instr, final AssemblyWriter a) {
    a.pop(ebx, instr.toString());
    a.pop(eax, instr.toString());
    a.sub(eax, ebx, instr.toString());
    a.push(eax, instr.toString());
  }

  static void compile_fsub(final AssemblyWriter a){
    //https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
    final String comment = "fsub";

    a.finit(comment);

    //load 2 arguments from stack
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);

    a.fsub(comment);
    a.fstp(comment);
    a.push(eax,comment);
  }

  static void compile_add(final VMInstr instr, final AssemblyWriter a) {
    a.pop(eax, instr.toString());
    a.pop(ebx, instr.toString());
    a.add(eax, ebx, instr.toString());
    a.push(eax, instr.toString());
  }

  static void compile_fadd(final AssemblyWriter a){
    //https://stackoverflow.com/questions/11853133/adding-floating-point-double-numbers-in-assembly
    final String comment = "fadd";

    a.finit(comment);

    //load from stack
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(ebx,comment);

    a.fadd(comment);
    a.fstp(comment);
    a.push(eax,comment);
  }


  static void compile_mod(final VMInstr instr, final AssemblyWriter a) {
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

  static void compile_neg(final VMInstr instr, final AssemblyWriter a) {
    a.pop(eax, instr.toString());
    a.mov(ebx, -1, instr.toString());
    a.mul_eax_with(ebx, instr.toString());
    a.push(eax, instr.toString());
  }
}
