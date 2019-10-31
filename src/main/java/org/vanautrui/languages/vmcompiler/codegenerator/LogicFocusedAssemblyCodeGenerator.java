package org.vanautrui.languages.vmcompiler.codegenerator;

import org.vanautrui.languages.vmcompiler.AssemblyWriter;

import static org.vanautrui.languages.vmcompiler.codegenerator.AssemblyCodeGenerator.unique;
import static org.vanautrui.languages.vmcompiler.model.Register.*;

final class LogicFocusedAssemblyCodeGenerator {

  //https://gist.github.com/nikAizuddin/0e307cac142792dcdeba


  static void compile_eq(final AssemblyWriter a) {
    final String comment = "eq";

    final long unique = unique();
    final String labeltrue = ".eq_push"+unique;
    final String labelend = ".eq_end"+unique;

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

  static void compile_gt(final AssemblyWriter a) {

    final String comment = "gt";

    final long unique = unique();
    final String labeltrue = ".gt_push"+unique;
    final String labelend = ".gt_end"+unique;

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


  static void compile_and(final AssemblyWriter a) {
    final String comment = "and";

    a.pop(ebx, comment);
    a.pop(eax, comment);
    a.and(eax,ebx,comment);
    a.push(eax,comment);
  }

  static void compile_or(final AssemblyWriter a) {
    final String comment = "or";

    a.pop(ebx, comment);
    a.pop(eax, comment);
    a.or(eax,ebx,comment);
    a.push(eax,comment);
  }

  static void compile_leq(final AssemblyWriter a) {

    final String comment = "leq";

    final long unique = unique();
    final String labeltrue = ".leq_push"+unique;
    final String labelend = ".leq_end"+unique;

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

  static void compile_geq(final AssemblyWriter a) {

    final String comment = "geq";

    final long unique = unique();
    final String labeltrue = ".geq_push"+unique;
    final String labelend = ".geq_end"+unique;

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


  static void compile_lt(final AssemblyWriter a) {

    final String comment = "lt";

    final long unique = unique();
    final String labeltrue = ".lt_push"+unique;
    final String labelend = ".lt_end"+unique;

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

  private static final String BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS = "0100011100000000B";

  static void compile_f_lt(final AssemblyWriter a){
    //https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

    final String comment = "f_lt";
    final String BITMASK_LESS = "0000000100000000B";
    final String label_less = ".less"+unique();
    final String label_end = ".end"+unique();

    a.finit(comment);

    //our stack
    //st0
    //st1

    //we want to know if st0 < st1

    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);

    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);

    final String st0="st0";
    final String st1="st1";

    a.fcom(st0,st1,comment);
    a.fstsw(ax,comment); //ax is fpu status register

    a.and(eax,BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS,"take only the condition code flags");
    a.cmp(eax,BITMASK_LESS,comment);
    a.je(label_less,comment); //if it is less, jump to less

    a.push(0,comment); //false
    a.jmp(label_end,comment);

    a.label(label_less,comment);
    a.push(1,comment); //true

    a.label(label_end,comment);
  }

  static void compile_f_gt(final AssemblyWriter a){
    //https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

    final String comment = "f_gt";
    final String BITMASK_FLOAT_GREATER = "0000000000000000B";

    a.finit(comment);

    //our stack
    //st0
    //st1

    //we want to know if st0 > st1

    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);

    final String st0="st0";
    final String st1="st1";

    a.fcom(st0,st1,comment);

    a.fcom(st0,st1,comment);
    a.fstsw(ax,comment); //ax is fpu status register

    a.and(eax,BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS,"take only the condition code flags");
    a.cmp(eax,BITMASK_FLOAT_GREATER,comment);

    final String label_greater = ".greater"+unique();
    final String label_end = ".end"+unique();

    a.je(label_greater,comment);

    a.push(0,comment); //false
    a.jmp(label_end,comment);

    a.label(label_greater,comment);
    a.push(1,comment); //true

    a.label(label_end,comment);
  }

  static void compile_f_eq(final AssemblyWriter a){

    final String BITMASK_FLOAT_EQ = "0100000000000000B";

    //https://gist.github.com/nikAizuddin/0e307cac142792dcdeba

    final String comment = "f_eq";

    a.finit(comment);

    //our stack
    //st0
    //st1

    //we want to know if st0 == st1

    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);
    a.fld("dword "+"["+esp+"]",comment);
    a.pop(eax,comment);

    final String st0="st0";
    final String st1="st1";

    a.fcom(st0,st1,comment);
    a.fstsw(ax,comment); //ax is fpu status register

    a.and(eax,BITMASK_FLOATING_POINT_ONLY_CONDITION_FLAGS,"take only the condition code flags");
    a.cmp(eax,BITMASK_FLOAT_EQ,comment);

    final String label_eq = ".eq"+unique();
    final String label_end = ".end"+unique();

    a.je(label_eq,comment);
    a.push(0,comment); //false
    a.jmp(label_end,comment);

    a.label(label_eq,comment);
    a.push(1,comment); //true

    a.label(label_end,comment);
  }
}
