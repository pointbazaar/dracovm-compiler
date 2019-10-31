package org.vanautrui.languages.vmcompiler.codegenerator;

import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.util.List;

import static org.junit.Assert.assertEquals;

public final class ArithmeticFocusedAssemblyCodeGeneratorTest {

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
