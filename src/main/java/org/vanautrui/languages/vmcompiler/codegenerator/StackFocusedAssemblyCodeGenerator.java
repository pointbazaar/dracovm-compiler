package org.vanautrui.languages.vmcompiler.codegenerator;

import org.vanautrui.languages.vmcompiler.AssemblyWriter;
import org.vanautrui.languages.vmcompiler.VMCompilerPhases;
import org.vanautrui.languages.vmcompiler.instructions.VMInstr;

import java.util.Optional;

import static org.vanautrui.languages.vmcompiler.VMCompilerPhases.SEGMENT_ARG;
import static org.vanautrui.languages.vmcompiler.VMCompilerPhases.SEGMENT_LOCAL;
import static org.vanautrui.languages.vmcompiler.model.Register.*;
import static org.vanautrui.languages.vmcompiler.model.Register.ebx;

final class StackFocusedAssemblyCodeGenerator {


  static void compile_dup(final VMInstr instr, final AssemblyWriter a) {
    //duplicates top of stack
    a.mov(eax,"["+esp+"]",instr.toString());
    a.push(eax,instr.toString());
  }

  /**
   * swaps the 2 topmost items on the stack
   * @param a
   */
  static void compile_swap(final String mycomment, final AssemblyWriter a) {
    String comment=" swap ";

    if(!mycomment.isEmpty()){
      comment=mycomment;
    }

    a.pop(eax,comment);
    a.pop(ebx,comment);

    a.push(eax,comment);
    a.push(ebx,comment);
  }

  private static void compile_push_arg(final int index, final AssemblyWriter a){
    final String comment="push ARG "+index;
    //arguments are on stack in reverse order
    //ebp is on the caller's return address, with the arguments above

    //eax = ebp + index + 2 * (4 (byte))
    //the + 2 * (4 (byte)) is because there is the saved ebp on stack also,
    //and the variables are indexed starting with 0
    a.mov(eax,2*byte_offset_32bit,comment); //4 bytes make up a 32 bit item on stack
    a.add(eax,ebp,comment);
    a.add(eax,index*byte_offset_32bit,comment); //4 bytes make up a 32 bit item on stack

    // a.dereference(eax) -> mov eax,[eax]
    a.dereference(eax,comment);

    //push eax
    a.push(eax,comment);
  }

  private static void compile_push_local(final int index, final AssemblyWriter a){
    final String comment="push LOCAL "+index;
    //locals are on stack in order
    //ebp is on the caller's return address, with the local variables below

    //eax = ebp - index - 1 //the +1 is because there is the saved ebp on stack also

    //eax=ebp
    a.mov(eax,ebp,comment);

    //eax -= 1
    a.mov(ebx,1*byte_offset_32bit,comment); //4 bytes make up a 32 bit item on stack
    a.sub(eax,ebx,comment);

    //eax -= index
    a.mov(ebx,index*4,comment); //4 bytes make up a 32 bit item on stack
    a.sub(eax,ebx,comment);

    // a.dereference(eax) -> mov eax,[eax]
    a.dereference(eax,comment);

    //push eax
    a.push(eax,comment);
  }

  static void compile_push(final String segment, final int index, final AssemblyWriter a) {

    switch (segment){
      case SEGMENT_ARG:
        compile_push_arg(index,a);
        break;
      case SEGMENT_LOCAL:
        compile_push_local(index,a);
        break;
      default:
        throw new RuntimeException("fatal");
    }
  }

  private static void compile_pop_arg(final int index, final AssemblyWriter a){
    final String comment = "pop ARG "+index;

    //arguments are on stack in reverse order
    //ebp is on the caller's return address, with the arguments above

    //eax = ebp + index + 2 //the +2 is because there is the saved ebp on stack also,
    //and the variables are indexed starting with 0
    a.mov(eax,2*byte_offset_32bit,comment); //4 bytes make up a 32 bit item on stack
    a.add(eax,ebp,comment);
    a.add(eax,index*byte_offset_32bit,comment);//4 bytes make up a 32 bit item on stack

    //get the value we want to pop
    a.pop(ebx,comment);

    //get that value into the address pointed to by eax
    a.store_second_into_memory_location_pointed_to_by_first(eax,ebx,comment);
  }

  private static void compile_pop_local(final int index, final AssemblyWriter a){
    final String comment = "pop LOCAL "+index;

    //locals are on stack in order
    //ebp is on the caller's return address, with the local variables below

    //eax = ebp - index - 1 //the +1 is because there is the saved ebp on stack also

    //eax=ebp
    a.mov(eax,ebp,comment);

    //eax -= 1
    a.mov(ebx,1 * byte_offset_32bit,comment); //4 bytes make up a 32 bit item on stack
    a.sub(eax,ebx,comment);

    //eax -= index
    a.mov(ebx,index*byte_offset_32bit,comment); //4 bytes make up a 32 bit item on stack
    a.sub(eax,ebx,comment);

    //get the value we want to pop
    a.pop(ebx,comment);

    a.store_second_into_memory_location_pointed_to_by_first(eax,ebx,comment);
  }

  /**
   * @param mysegment the Optional segment to pop into
   * @param a the AssemblyWriter we are using
   * @throws Exception if we are pop - ing from a nonexisting segment
   */
  static void compile_pop(final Optional<String> mysegment, final Optional<Integer> myindex, final AssemblyWriter a) throws Exception{

    if(!mysegment.isPresent()){
      a.pop(eax,"pop");
    }else {

      switch (mysegment.get()) {
        case SEGMENT_ARG:
          compile_pop_arg(myindex.get(),a);
          break;
        case SEGMENT_LOCAL:
          compile_pop_local(myindex.get(),a);
          break;
        default:
          throw new Exception("fatal");
      }
    }
  }
}
