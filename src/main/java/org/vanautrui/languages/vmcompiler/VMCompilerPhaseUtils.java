package org.vanautrui.languages.vmcompiler;


public final class VMCompilerPhaseUtils {

    public static void printBeginPhase(final String phaseName, final boolean debug) {
        if(debug) {
            System.out.println(phaseName);
        }
    }

    public static void printEndPhase(final boolean success, final boolean debug) {
        if(debug) {
            System.out.println((success) ? "✓" : "⚠");
        }
    }
}
