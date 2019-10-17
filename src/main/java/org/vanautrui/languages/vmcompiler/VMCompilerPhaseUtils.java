package org.vanautrui.languages.vmcompiler;

import org.fusesource.jansi.Ansi;

import static org.fusesource.jansi.Ansi.Color.GREEN;
import static org.fusesource.jansi.Ansi.Color.RED;
import static org.fusesource.jansi.Ansi.ansi;

public class VMCompilerPhaseUtils {
    public static void printBeginPhase(String phaseName,boolean printLongForm) throws InterruptedException {
        //TerminalUtil.print(String.format("%-18s ",phaseName),Ansi.Color.GREEN);
        Ansi ansi = Ansi.ansi();
        if(!printLongForm) {
            ansi.eraseLine(Ansi.Erase.BACKWARD);
        }

        ansi.fg(GREEN);
        ansi.a(phaseName);

        ansi.reset();
        System.out.print(ansi);
        System.out.flush();

        //rip this out, this is slowing down the compiler
        //Thread.sleep(100);
    }

    public static void printEndPhase(boolean success, boolean printLongForm) throws Exception{
        Ansi ansi1=Ansi.ansi();
        if(!printLongForm) {
            ansi1.cursorToColumn(0);
        }

        ansi1.cursorToColumn(18);
        if(success) {
            ansi1.fg(GREEN);
            ansi1.a("✓");
        }else{
            ansi1.fg(RED);
            ansi1.a("⚠");
        }
        ansi1.cursorToColumn(0);
        if(printLongForm){
            ansi1.newline();
        }
        if(!printLongForm) {
            ansi1.eraseLine(Ansi.Erase.FORWARD);
        }
        ansi1.reset();
        System.out.print(ansi1);
        System.out.flush();

        //rip this out, this is slowing down the compiler
        //Thread.sleep(100);
    }
}
