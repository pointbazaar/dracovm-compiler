package org.vanautrui.languages.vmcompiler.codegenerator;

import org.apache.commons.io.IOUtils;
import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.util.List;

import static org.junit.Assert.assertEquals;

public final class ArrayFocusedAssemblyCodeGeneratorTest {

    @Test
    public void test_compile_arrayread_arraystore()throws Exception{

        DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);
        a.iconst(3);
        a.call("Builtin","new");

        //return value is on stack
        a.swap();
        a.pop();
        //removed the argument, return value (pointer to array) still on stack
        a.dup();

        a.iconst(0); //store at index 0
        a.iconst(9); //store a 9
        a.arraystore();

        //value stored, pointer to array on stack
        a.iconst(0);

        a.arrayread();
        //value read is on stack (should be a 9)

        //return value (supposed array length on stack)
        a.call("Builtin","putdigit");

        //clear return value and argument
        a.pop();
        a.pop();

        a.iconst(0);
        a.exit();

        List<String> vmcodes=a.getDracoVMCodeInstructions();

        Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "arrayread_arraystore_testing");

        assertEquals(0,pr.exitValue());
        assertEquals("9", IOUtils.toString(pr.getInputStream()));
    }

    @Test
    public void test_compile_arraystore_len()throws Exception{

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);
        a.iconst(3);
        a.call("Builtin","new");

        //return value is on stack
        a.swap();
        a.pop();
        //removed the argument, return value (pointer to array) still on stack
        a.dup();

        a.iconst(0); //store at index 0
        a.iconst(9); //store a 9
        a.arraystore(); //if it is correctly implemented, it should store at internal index 1, as the length prefix is on internal index 0

        a.call("Builtin","len");

        a.swap();
        a.pop();

        //return value (supposed array length on stack)
        a.call("Builtin","putdigit");
        a.pop(); //pop return value
        a.pop(); //pop argument

        a.iconst(0);
        a.exit();

        List<String> vmcodes=a.getDracoVMCodeInstructions();

        Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "arraystore_len_testing");

        assertEquals(0,pr.exitValue());
        assertEquals("3", IOUtils.toString(pr.getInputStream()));
    }

    @Test
    public void test_len_is_ok_with_stack()throws Exception{

        //test that len does not put too much on the stack

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(1);
        a.iconst(3);
        a.call("Builtin","new");

        //return value (pointer to array) is on stack

        a.call("Builtin","len");
        a.pop(); //pop return value, argument still on stack

        a.call("Builtin","len");
        a.pop(); //pop return value, argument still on stack

        a.call("Builtin","len");
        a.pop(); //pop return value, argument still on stack

        a.pop(); //pop the array reference
        a.pop(); //pop the 3

        //earlier value (1) should be on the stack now
        a.call("Builtin","putdigit");
        a.pop(); //pop return value
        a.pop(); //pop argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "len_stack_stability_testing");

        assertEquals(0,pr.exitValue());
        assertEquals("1", IOUtils.toString(pr.getInputStream()));
    }

    @Test
    public void test_new_is_ok_with_stack()throws Exception{

        //test that new does not put too much on the stack

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(5);

        a.call("Builtin","new");
        //return value (pointer to array) is on stack

        a.pop(); //pop the array reference


        //earlier value (1) should be on the stack now
        a.call("Builtin","putdigit");
        a.pop(); //pop return value
        a.pop(); //pop argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "new_stack_stability_testing");

        assertEquals(0,pr.exitValue());
        assertEquals("5", IOUtils.toString(pr.getInputStream()));
    }
}
