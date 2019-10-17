package org.vanautrui.languages.vmcompiler;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.io.IOUtils;
import org.fusesource.jansi.Ansi;
import org.vanautrui.languages.vmcompiler.codegenerator.AssemblyCodeGenerator;

import java.io.PrintStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import java.util.stream.Collectors;

import static java.lang.System.out;
import static org.fusesource.jansi.Ansi.ansi;
import static org.vanautrui.languages.vmcompiler.VMCompilerPhaseUtils.printBeginPhase;
import static org.vanautrui.languages.vmcompiler.VMCompilerPhaseUtils.printEndPhase;

public class VMCompilerPhases {

    //Assembly debugging:
    //(they are missing a stack display, but it be ok)
    //https://carlosrafaelgn.com.br/Asm86/

    //general having fun and trying different programming languages:
    //https://tio.run/#
    //https://www.tutorialspoint.com/codingground.htm

    //https://asmtutor.com/
    //https://asmtutor.com/#lesson1

    //https://www.youtube.com/watch?v=ubXXmQzzNGo

    private static final String FLAG_DEBUG="debug";
    private static final String FLAG_TIMED="timed";

    private final boolean debug;
    private final boolean timed;
    private final boolean printLong;

    public VMCompilerPhases(CommandLine cmd){
        this.debug=cmd.hasOption(FLAG_DEBUG);
        this.timed=cmd.hasOption(FLAG_TIMED);
        this.printLong=debug||timed;
    }

    public VMCompilerPhases() {
        //for testing of compiler phases
        this.debug=false;
        this.timed=false;
        this.printLong=false;
    }

    /**
     * @param asm_codes the assembly codes to be assembled and linked
     *
     * @throws Exception an exception is thrown if nasm or ld exit nonzero
     */
    public Path phase_generate_executable(List<String> asm_codes,String filename_without_extension) throws Exception{

        final String asm_file_name = filename_without_extension+".asm";
        final Path asm_path = Paths.get(asm_file_name);
        final String asm_codes_string = asm_codes.stream().collect(Collectors.joining("\n"))+"\n";
        Files.write(
                asm_path,asm_codes_string.getBytes()
        );

        final Process p = Runtime.getRuntime().exec("nasm -f elf -g -F stabs " + asm_file_name);
        p.waitFor();

        if(p.exitValue()!=0){
            final String output = IOUtils.toString(p.getInputStream());
            System.err.println(output);
            throw new Exception("nasm exit with nonzero exit code");
        }

        final Process p2 =
                Runtime
                        .getRuntime()
                        .exec("ld -melf_i386 -s -o "+filename_without_extension+" "+filename_without_extension+".o");
        p2.waitFor();
        if(p2.exitValue() != 0){
            throw new Exception("ld exit with nonzero exit code");
        }

        Files.delete(Paths.get(filename_without_extension+".o"));

        return Paths.get(filename_without_extension);
    }

    public List<String> phase_vm_code_compilation(List<String> draco_vm_codes,boolean debug) throws Exception{
        printBeginPhase("VM CODE COMPILATION",printLong);
        final List<String> assembly_codes = AssemblyCodeGenerator.compileVMCode(draco_vm_codes);
        if(debug){
            assembly_codes.stream().forEach(System.out::println);
        }
        //$ nasm -f elf hello.asm  # this will produce hello.o ELF object file
        //$ ld -s -o hello hello.o # this will produce hello executable

        if(debug){
            out.println("call nasm");
        }
        printEndPhase(true,printLong);
        return assembly_codes;
    }

    private static final String phase_clean_cache_dir=System.getProperty("user.home")+"/.dragoncache/clean/";
}
