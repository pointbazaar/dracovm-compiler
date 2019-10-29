package org.vanautrui.languages.vmcompiler.codegenerator;

import org.apache.commons.io.IOUtils;
import org.junit.Assert;
import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

import static org.junit.Assert.*;

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

        //running this shows that the value seems to be increasing,
        //as we get successive alphabetic characters when we are in the ascii range
        System.out.println(IOUtils.toString(pr.getInputStream()));


        //toodo: we should really test it, but for that we probably have to have a builtin
        //subroutine in assembly to print an integer to stdout
    }

    @Test
    public void test_fopen()throws Exception{

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(2); //we need a string of length 2, 1 char for name, 1 char for nullbyte
        a.call("Builtin","new");

        //pointer to string should be on stack

        //get rid of argument
        a.swap();
        a.pop();

        a.dup();

        //store filename into string
        a.iconst(0); //index
        a.cconst('x'); //our filename
        a.arraystore();

        a.dup();

        //store the nullbyte, as it is a c-style system call
        a.iconst(1);
        a.iconst(0);
        a.arraystore();

        a.iconst(2); //access mode rw

        //subroutine arguments must be on stack in reverse order.
        a.swap();

        a.call("Builtin","fopen");

        a.swap();
        a.pop(); //clear argument

        a.swap();
        a.pop(); //clear argument

        //our file descriptor is the exit code.
        //if there was no error it should be =/= -1
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "time_testing");

        System.out.println(pr.exitValue());
        assertNotEquals(-1,pr.exitValue());

        Assert.fail();
        //this test is flawed. it should give a different file descriptor,
        //if we change the filename. but you can see from the exit code that it is not the case
    }

    @Test
    public void test_fopen_fputs()throws Exception{

        //TODO: think about it: why could this fail?
        // maybe the syscall is expecting our string in bytes, yet we are working with DWORDS
        // let us investigate why this test is not working

        final DracoVMCodeWriter a=new DracoVMCodeWriter();

        a.subroutine("Main","main",0,0);

        a.iconst(2); //we need a string of length 2, 1 char for name, 1 char for nullbyte
        a.call("Builtin","new");

        //pointer to string should be on stack

        //get rid of argument
        a.swap();
        a.pop();

        a.dup();

        //store filename into string
        a.iconst(0); //index
        a.cconst('c'); //our filename
        a.arraystore();

        a.dup();

        //store the nullbyte, as it is a c-style system call
        a.iconst(1);
        a.iconst(0);
        a.arraystore();

        a.iconst(2); //access mode rw

        //subroutine arguments must be on stack in reverse order.
        a.swap();

        a.call("Builtin","fopen");

        a.pop(); //clear return value
        a.pop(); //clear argument
        a.pop(); //clear argument

        a.iconst(0);
        a.exit();

        final List<String> vmcodes=a.getDracoVMCodeInstructions();

        final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "time_testing");

        assertEquals(0,pr.exitValue());

        //see if file exists
        final Path path = Paths.get("c");
        assertTrue(Files.exists(path));

        //https://stackoverflow.com/questions/1390592/check-if-file-is-already-open
        //maybe this whole test is doomed, because the process we are creating
        //is probably a child of this? what does that mean for files open?
        try {
            org.apache.commons.io.FileUtils.touch(path.toFile());
            Assert.fail();
        } catch (IOException e) {
            //pass, we could not touch the file as it is locked and has been opened by someone else
        }
    }
}
