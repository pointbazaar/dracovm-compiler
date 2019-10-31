package org.vanautrui.languages.vmcompiler;

import org.vanautrui.languages.vmcompiler.model.Register;
import java.util.*;

import static org.vanautrui.languages.vmcompiler.model.Register.*;

public final class AssemblyWriter {

    //https://www.cs.virginia.edu/~evans/cs216/guides/x86.html

    private final List<String> instrs=new ArrayList<>();
    private boolean indented=true;

    public AssemblyWriter(){

    }

    public List<String> getAssemblyProgram(){
        return this.instrs;
    }

    private void any(final String cmd) {
        any(cmd,"");
    }

    public void any(final String cmd, final String comment){
        final String spacer="        ";
        //base
        String prefix="";
        if(indented){
            prefix=spacer;
        }
        String suffix="";
        if(!indented){
            suffix=spacer;
        }
        instrs.add(
                String.format(prefix+"%-20s "+suffix+";%-20s",cmd,comment)
        );
    }

    public void section(final String name,final String comment) {
        indented=false;
        any("section "+name,comment);
        indented=true;
    }
    public void section(final String name) {
        section(name,"");
    }

    public void global(final String s,final String comment) {
        indented=false;
        any("global "+s,"");
        indented=true;
    }
    public void global(final String s) {
        global(s,"");
    }

    public void mov(final Register dest, final int value, final String comment) {
        any("mov "+dest+","+value,comment);
    }
    public void mov(final Register dest,final float value) {
        mov(dest,value,"");
    }
    public void mov(final Register register, final float value, String comment) {
        any("mov "+register+","+value,comment);
    }
    public void mov(final Register dest,final int value) {
        any("mov "+dest+","+value);
    }
    public void mov(final Register dest, final Register src, final String comment) {
        any("mov "+dest+","+src,comment);
    }
    public void mov(final Register dest,final Register src) {
        mov(dest,src,"");
    }
    public void mov(final Register dest, final String src, final String comment) {
        any("mov "+dest+","+src,comment);
    }

    public void push(final Register register) {
        push(register,"");
    }
    public void push(final Register register,final String comment) {
        any("push "+register,comment);
    }
    public void push(final int value,final String comment) {
        any("push "+value,comment);
    }
    public void push(final float f, final String comment) {
        any("push "+f,comment);
    }


    public void pop(final Register register) {
        pop(register,"");
    }
    public void pop(final Register reg, final String comment) {
        any("pop "+reg,comment);
    }

    /** enables to call a subroutine at an address contained in a register
     * @param register the register containing the destination address
     */
    public void call(final Register register,final String comment) {
        any("call "+register,comment);
    }
    public void call(final String methodName,final String comment) {
        any("call "+methodName,comment);
    }
    public void call(final String methodName) {
        call(methodName,"");
    }

    public void add(final Register dest,final int val) {
        add(dest,val,"");
    }
    public void add(final Register dest,final int val,final String comment) {
        any("add "+dest+","+val,comment);
    }
    public void add(final Register dest,final Register reg2) {
        add(dest,reg2,"");
    }
    public void add(final Register dest, final Register reg2, final String comment) {
        any("add "+dest+","+reg2,comment);
    }
    public void sub(final Register dest,final Register reg2) {
        sub(dest,reg2,"");
    }
    public void sub(final Register dest, final Register reg2, final String comment) {
        any("sub "+dest+","+reg2,comment);
    }

    public void label(final String label,final String comment) {
        indented=false;
        any(label+":",comment);
        indented=true;
    }
    public void label(final String label) {
        label(label,"");
    }

    public void jmp(final String targetLabel,final String comment) {
        any("jmp "+targetLabel,comment);
    }
    public void jmp(final String targetLabel) {
        jmp(targetLabel,"");
    }


    /** i read that the 'mul' instruction only takes one operang,
     *  and computes eax=eax*other_register
     * @param other
     */
    public void mul_eax_with(final Register other) {
        mul_eax_with(other,"");
    }
    public void mul_eax_with(final Register other, final String comment) {
        any("mul "+other,comment);
    }

    public void call_kernel() {
        any("int 0x80","call kernel");
    }

    public void cmp(final Register reg1, final Register reg2) {
        cmp(reg1,reg2,"");
    }
    public void cmp(final Register reg1, final Register reg2,final String comment) {
        any("cmp "+reg1+","+reg2,comment);
    }
    public void cmp(final Register reg1, final String rightside, final String comment){
        any("cmp "+reg1+","+rightside,comment);
    }

    public void je(final String label) {
        je(label,"");
    }
    public void je(final String label,final String comment) {
        any("je "+label,comment);
    }


    public void xor(final Register reg1, final Register reg2, final String comment) {
        any("xor "+reg1+","+reg2,comment);
    }


    public void nop() {
        any("nop");
    }

    /**
     * Increments the value of a register
     * @param register register to increment
     */
    public void inc(final Register register,final String comment) {
        any("inc "+register,comment);
    }
    public void inc(final Register reg){
        inc(reg,"");
    }

    /**
     * Decrements the value of a register
     * @param register register to decrement
     */
    public void dec(final Register register){
        dec(register,"");
    }
    public void dec(final Register reg,final String comment){
        any("dec "+reg,comment);
    }

    //jump if comparison result was 'greater'
    public void jg(final String label) {
        jg(label,"");
    }
    public void jg(final String label,final String comment) {
        any("jg "+label,comment);
    }


    public void jge(final String label, final String comment) {
        any("jge "+label,comment);
    }


    //jump if comparison result was 'lesser'
    public void jl(final String label) {
        jl(label,"");
    }
    public void jl(final String label,final String comment) {
        any("jl "+label,comment);
    }


    public void jle(final String label, final String comment) {
        any("jle "+label,comment);
    }


    /** dereferences a pointer register,
     * which means to set the value of the register to the value in memory which it points to
     * @param register the register to be dereferenced
     */
    public void dereference(final Register register) {
        dereference(register,"");
    }

    public void dereference(final Register register, final String comment) {
        any("mov "+register+","+"["+register+"]",comment+"// dereference "+register);
    }

    public void store_second_into_memory_location_pointed_to_by_first(final Register first, final Register second) {
        store_second_into_memory_location_pointed_to_by_first(first,second,"");
    }


    public void store_second_into_memory_location_pointed_to_by_first(final Register first, final Register second, final String comment) {
        any("mov ["+first+"],"+second,comment);
    }

    public void idiv_eax_by(final Register register) {
        idiv_eax_by(register,"");
    }
    public void idiv_eax_by(final Register register, final String comment) {
        //using idiv instead of div, because idiv should also work for signed values

        any("idiv "+register,comment);
    }

    public void ret() {
        any("ret");
    }


    /** this subroutine is intended to be able to push a label on the stack, to facilitate functional programming
     * @param labelName the label to be pushed
     */
    public void push(final String labelName, final String comment) {
        any("push "+labelName,comment);
    }

    public void shl(final Register register, final Register lower_half_of_cx_probably,final String comment) {
        //usure if we only can shift by cl ?
        any("shl "+register+","+lower_half_of_cx_probably,comment);
    }

    public void shr(final Register register, final Register lower_half_of_cx_probably,final String comment) {
        //usure if we only can shift by cl ?
        any("shr "+register+","+lower_half_of_cx_probably,comment);
    }


    public void lea(final Register register, final String other) {
        //https://www.cs.virginia.edu/~evans/cs216/guides/x86.html
        any("lea "+register+","+other);
    }

    public void and(final Register reg1, final Register reg2, final String comment) {
        any("and "+reg1+","+reg2,comment);
    }
    public void and(final Register reg1, final String rightside, final String comment) {
        any("and "+reg1+","+rightside,comment);
    }

    public void or(final Register reg1, final Register reg2, final String comment) {
        any("or "+reg1+","+reg2,comment);
    }

    public void cdq(String comment) {
        any("cdq",comment);
    }

    public void fld(final String leftside, final String comment) {

        //https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4
        //<3 data sheets

        //https://stackoverflow.com/questions/37775144/how-to-load-the-value-stored-in-extended-registereax-into-st0-of-floating-poin

        any("fld "+leftside,comment);
    }

    public void faddp(String comment) {
        //add 2 floating point values on the stack and pop
        any("faddp",comment);
    }

    public void fstp(String comment) {
        //store floating point value and pop from fpu stack
        //TODO: remove the bloat here
        push(esi,comment);
        any("fstp dword ["+esp+"]",comment);
        pop(esi,comment);
    }

    public void fadd(final String comment) {
        any("fadd",comment);
    }

    public void fsub(final String comment){
        any("fsub",comment);
    }

    public void fmul(final String comment) {
        any("fmul",comment);
    }

    public void fcom(String float_register_left, String float_register_right, String comment) {
        //compare floating point numbers
        any("fcom "+float_register_left+","+float_register_right,comment);
    }

    public void finit(final String comment) {
        //https://gist.github.com/nikAizuddin/0e307cac142792dcdeba
        //resets fpu stacks to default
        any("finit ",comment);
    }
}
