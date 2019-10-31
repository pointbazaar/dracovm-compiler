package org.vanautrui.languages.vmcompiler.codegenerator;

import org.apache.commons.io.IOUtils;
import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.util.List;

import static org.junit.Assert.assertEquals;

public final class AssemblyCodeGeneratorTest {

  @Test
  public void test_correct_return_code_is_generated()throws Exception{

    DracoVMCodeWriter a=new DracoVMCodeWriter();
    a.subroutine("Main","main",0,0,"");
    a.iconst(45);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(45,pr.exitValue());
    assertEquals("", IOUtils.toString(pr.getInputStream()));
  }

  @Test
  public void test_goto()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();
    a.subroutine("Main","main",0,0,"");
    a._goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(0);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(0,pr.exitValue());
    assertEquals("", IOUtils.toString(pr.getInputStream()));
  }

  @Test
  public void test_if_goto()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0,"");
    a.iconst(1);
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(0);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(0,pr.exitValue());
    assertEquals("", IOUtils.toString(pr.getInputStream()));
  }

  @Test
  public void test_iconst()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(-6);
    a.iconst(6);
    a.add();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "test_iconst");

    assertEquals(0,pr.exitValue());
  }
}
