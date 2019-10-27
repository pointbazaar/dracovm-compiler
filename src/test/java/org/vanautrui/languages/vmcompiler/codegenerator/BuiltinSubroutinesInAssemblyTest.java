package org.vanautrui.languages.vmcompiler.codegenerator;

import org.apache.commons.io.IOUtils;
import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.util.List;

import static org.junit.Assert.assertEquals;

public final class BuiltinSubroutinesInAssemblyTest {

    @Test
    public void test_abs()throws Exception{

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(3);//arg
        a.call("Builtin","abs");
        //clear arg
        a.swap();
        a.pop();

        //return value on stack
        a.call("Builtin","putdigit");

        a.pop(); //clear return value
        a.pop(); //clear argument

        a.iconst(-3); //arg
        a.call("Builtin","abs");
        //clear arg
        a.swap();
        a.pop();

        //return value on stack
        a.call("Builtin","putdigit");

        a.pop(); //clear return value
        a.pop(); //clear argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "arraystore_testing");

        assertEquals(0,pr.exitValue());
        assertEquals("33", IOUtils.toString(pr.getInputStream()));
    }

    @Test
    public void test_time()throws Exception{

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.call("Builtin","time");

        //return value on stack
        a.call("Builtin","putdigit");

        a.pop(); //clear return value
        a.pop(); //clear argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "time_testing");

        assertEquals(0,pr.exitValue());
        //toodo: we should really test it, but for that we probably have to have a builtin
        //subroutine in assembly to print an integer to stdout
    }
}
