package org.vanautrui.languages.vmcompiler.codegenerator;

import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static org.junit.Assert.assertEquals;

public final class ArithmeticFocusedAssemblyCodeGeneratorTest {

  @Test
  public void test_fadd1()throws Exception{
    //final DracoVMCodeWriter a=new DracoVMCodeWriter();

    final List<String> vmcodes = Arrays.asList(
            "subroutine Main_main 0 args 0 locals",
            "fconst 3.0",
            "fconst 2.0",
            "fadd",
            "fconst 7.0",
            "f_lt",
            "if-goto exit0",
            "iconst 1",
            "exit",
            "label exit0",
            "iconst 0",
            "exit"
    );

    //it should exit with 0

    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testfadd1");

    assertEquals(0,pr.exitValue());
  }

  @Test
  public void test_fadd2()throws Exception{
    //final DracoVMCodeWriter a=new DracoVMCodeWriter();

    final List<String> vmcodes = Arrays.asList(
            "subroutine Main_main 0 args 0 locals",
            "fconst 3.0",
            "fconst 6.0",
            "fadd",
            "fconst 7.0",
            "f_lt",
            "if-goto exit0",
            "iconst 1",
            "exit",
            "label exit0",
            "iconst 0",
            "exit"
    );

    //it should exit with 0

    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testfadd2");

    assertEquals(1,pr.exitValue());
  }

  @Test
  public void test_fsub()throws Exception{
    //final DracoVMCodeWriter a=new DracoVMCodeWriter();

    final List<String> vmcodes = Arrays.asList(
            "subroutine Main_main 0 args 0 locals",
            "fconst 3.0",
            "fconst 2.0",
            "fsub",
            "fconst 7.0",
            "f_lt",
            "if-goto exit0",
            "iconst 1",
            "exit",
            "label exit0",
            "iconst 0",
            "exit"
    );

    //it should exit with 0

    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testfsub");

    assertEquals(0,pr.exitValue());
  }

  @Test
  public void test_fsub2()throws Exception{
    //final DracoVMCodeWriter a=new DracoVMCodeWriter();

    final List<String> vmcodes = Arrays.asList(
            "subroutine Main_main 0 args 0 locals",
            "fconst 30.0",
            "fconst 2.0",
            "fsub",
            "fconst 7.0",
            "f_lt",
            "if-goto exit0",
            "iconst 1",
            "exit",
            "label exit0",
            "iconst 0",
            "exit"
    );

    //it should exit with 0

    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testfsub2");

    assertEquals(1,pr.exitValue());
  }

  @Test
  public void test_add()throws Exception{
    final DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(3);
    a.iconst(2);

    a.add();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testadd");

    assertEquals(5,pr.exitValue());
  }

  @Test
  public void test_sub()throws Exception{
    final DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(3);
    a.iconst(2);

    a.sub();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testsub");

    assertEquals(1,pr.exitValue());
  }

  @Test
  public void test_mod()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(3);
    a.iconst(2);

    a.mod();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testmod");

    assertEquals(1,pr.exitValue());
  }

  @Test
  public void test_mod2()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(23);
    a.iconst(6);

    a.mod();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testmod2");

    assertEquals(5,pr.exitValue());
  }

  @Test
  public void test_div()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(6);
    a.iconst(2);

    a.div();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testdiv");

    assertEquals(3,pr.exitValue());
  }

  @Test
  public void test_div_positive_by_negative()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(200);
    a.iconst(-10);

    a.div();

    //-20 should be the result
    a.iconst(20);
    a.add();

    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "test_div_positive_by_negative");

    assertEquals(0,pr.exitValue());
  }

  @Test
  public void test_div_negative_by_positive()throws Exception{
    //https://stackoverflow.com/questions/46574893/divide-a-positive-number-by-a-negative-number-in-assembly

    //maybe the problem is that -300 is not sign-extended
    // edx:eax is the dividend. so if edx == 0b0... then maybe there is a problem?

    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(-300);
    a.iconst(10);

    a.div();

    //-30 should be the result
    a.iconst(30);
    a.add();

    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "test_negative_by_positive");

    assertEquals(0,pr.exitValue());
  }
}
