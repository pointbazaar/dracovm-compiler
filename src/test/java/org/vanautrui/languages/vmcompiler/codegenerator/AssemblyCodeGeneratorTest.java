package org.vanautrui.languages.vmcompiler.codegenerator;

import org.apache.commons.io.IOUtils;
import org.junit.Test;
import org.vanautrui.languages.CodeGeneratorTestUtils;
import org.vanautrui.languages.DracoVMCodeWriter;

import java.util.List;

import static org.junit.Assert.assertEquals;

public class AssemblyCodeGeneratorTest {

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
  public void test_eq_false()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0,"");
    a.iconst(1);
    a.iconst(0);
    a.eq();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(0);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(45,pr.exitValue());
  }

  @Test
  public void test_eq_true()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0,"");
    a.iconst(11);
    a.iconst(11);
    a.eq();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(3,pr.exitValue());
  }

  @Test
  public void test_neq_true()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0,"");
    a.iconst(1);
    a.iconst(0);
    a.neq();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(0);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(0,pr.exitValue());
  }

  @Test
  public void test_neq_false()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0,"");
    a.iconst(11);
    a.iconst(11);
    a.neq();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "putchartesting");

    assertEquals(45,pr.exitValue());
  }

  @Test
  public void test_gt_true()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(12);
    a.iconst(11);
    a.gt();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "gttesting");

    assertEquals(3,pr.exitValue());
  }


  @Test
  public void test_gt_false()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(11);
    a.iconst(11);
    a.gt();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "gttestingfalse");

    assertEquals(45,pr.exitValue());
  }



  @Test
  public void test_lt_true()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(2);
    a.iconst(11);
    a.lt();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "lttestingtrue");

    assertEquals(3,pr.exitValue());
  }


  @Test
  public void test_lt_false()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(11);
    a.iconst(11);
    a.lt();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "lttesting");

    assertEquals(45,pr.exitValue());
  }

  @Test
  public void test_swap()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(2);
    a.iconst(11);
    a.swap();
    a.gt();
    a.if_goto("label1");
    a.iconst(45);
    a.exit();
    a.label("label1");
    a.iconst(3);
    a.exit();

    List<String> vmcodes = a.getDracoVMCodeInstructions();
    Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "swaptesting");

    assertEquals(3,pr.exitValue());
  }

  @Test
  public void test_and()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(1);
    a.iconst(1);

    a.and();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "andtesting");

    assertEquals(1,pr.exitValue());
  }

  @Test
  public void test_and2()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(1);
    a.iconst(0);

    a.and();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "andtesting2");

    assertEquals(0,pr.exitValue());
  }

  @Test
  public void test_or()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(0);
    a.iconst(0);

    a.or();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testor");

    assertEquals(0,pr.exitValue());
  }

  @Test
  public void test_or2()throws Exception{
    DracoVMCodeWriter a=new DracoVMCodeWriter();

    a.subroutine("Main","main",0,0);
    a.iconst(1);
    a.iconst(0);

    a.or();
    a.exit();

    final List<String> vmcodes = a.getDracoVMCodeInstructions();
    final Process pr = CodeGeneratorTestUtils.compile_and_run_vm_codes_for_testing(vmcodes, "testor2");

    assertEquals(1,pr.exitValue());
  }
}
