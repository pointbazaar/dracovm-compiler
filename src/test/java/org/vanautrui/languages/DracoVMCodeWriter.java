package org.vanautrui.languages;

import org.apache.commons.lang3.StringUtils;
import org.vanautrui.languages.vmcompiler.codegenerator.SubroutineFocusedAssemblyCodeGenerator;

import java.util.ArrayList;
import java.util.List;

/**
 * this class provides an abstraction,
 * in order to write correct dracovm programs easily via
 * IntelliJ's autocomplete.
 * It provides subroutines for almost? every dracovm instruction
 */
public final class DracoVMCodeWriter {

  private List<String> instrs=new ArrayList<>();
  public List<String> getDracoVMCodeInstructions(){
    return this.instrs;
  }

  private static final int default_indent=4;
  private static final int label_indent=1;
  private static final int subroutine_indent=0;

  private int indent=default_indent;

  public static final String SEGMENT_ARG = "ARG";
  public static final String SEGMENT_LOCAL = "LOCAL";
  public static final String SEGMENT_STATIC = "STATIC";

  public DracoVMCodeWriter(){}

  /**
   * all access to this.instrs must flow through here because this subroutine
   * keeps track of correct indentation
   */
  private void any(String command,String comment){
    String myindent = StringUtils.repeat("  ", this.indent);

    String mycomment="";
    if(!comment.isEmpty()){
      mycomment="       //"+comment;
    }

    instrs.add(myindent+command+mycomment);
  }

  private void any(String command){
    any(command,"");
  }

  public void subroutine(String containing_class_name,String name,int nArgs,int nLocals){
   subroutine(containing_class_name,name,nArgs,nLocals,"");
  }

  public void subroutine(String containing_class_name,String name,int nArgs,int nLocals,String comment){
    indent=subroutine_indent;
    any("subroutine "+ containing_class_name+"_"+name +" "+nArgs+" args "+nLocals+" locals", SubroutineFocusedAssemblyCodeGenerator.compile_subroutine_description()+"//"+comment);
    indent=default_indent;
  }

  public void push(String segment, int index){
    any("push "+segment+" "+index);
  }

  public void pop(String segment, int index){
    any("pop "+segment+" "+index);
  }

  public void pop(){
    pop("");
  }
  public void pop(String comment){
    any("pop",comment);
  }

  public void label(String name){
    indent=label_indent;
    any("label "+name);
    indent=default_indent;
  }

  public void if_goto(String label){
    any("if-goto "+label);
  }

  public void _goto(String label){
    any("goto "+label);
  }

  public void neg(){
    any("neg");
  }

  public void dup() {
    any("dup");
  }


  public void iconst(int i,String comment){

    any("iconst "+i,comment);
  }
  public void iconst(int i) {
    iconst(i,"");
  }

  public void eq() {
    any("eq");
  }

  public void dec() {
    any("dec");
  }
  public void inc() {
    any("inc");
  }

  public void fconst(float value) {
    any("fconstn "+value);
  }

  public void call(String containing_class_name,String methodName) {
    any("call "+containing_class_name+"_"+methodName, SubroutineFocusedAssemblyCodeGenerator.compile_call_description());
  }

  public void add() {
    any("add");
  }

  public void sub() {
    any("sub");
  }

  public void div() {
    any("div");
  }

  public void mul(){
    any("mul");
  }

  public void exit() {
    any("exit");
  }

  public void _return() {
    any("return", SubroutineFocusedAssemblyCodeGenerator.compile_return_description());
  }

  public void cconst(char c) {
    any("cconst "+c);
  }

  public void gt() {
    any("gt");
  }

  public void lt() {
    any("lt");
  }

  /**
   * swaps the topmost 2 items on the stack
   */
  public void swap() {
    any("swap");
  }
  public void swap(String comment) {
    any("swap",comment);
  }


  public void arraystore() {
    any("arraystore");
  }

  public void arrayread() {
    /*
    arrayread
        //stack looks like:
        //|undefined
        //|array address
        //|array index <- esp
        ////after execution of this command, stack looks like
        //|undefined
        //|array[index] <-esp
        //meaning this vm command reads from an array, and places the value on the stack
     */
    any("arrayread");
  }


  public void not() {
    //boolean negation
    any("not");
  }

  /**
   * if the 2 topmost values on stack are not equal, this returs true
   * else this returns false
   */
  public void neq() {
    eq();
    not();
  }

  /**
   * modulo. if the stack is like [ a,b <-esp]
   * then after execution it would be:
   * [ (a modulo b)  <- esp ]
   */
  public void mod() {
    any("mod");
  }

  public void pushsubroutine(String subroutine_name) {
    any("pushsubroutine "+subroutine_name);
  }

  public void callfromstack() {
    any("callfromstack");
  }

  public void lshiftl() {
    any("lshiftl");
  }

  public void lshiftr() {
    any("lshiftr");
  }

  public void and() {
    any("and");
  }

  public void or() {
    any("or");
  }
}
