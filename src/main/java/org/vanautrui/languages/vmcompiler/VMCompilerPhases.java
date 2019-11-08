package org.vanautrui.languages.vmcompiler;

import org.vanautrui.languages.vmcompiler.codegenerator.AssemblyCodeGenerator;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import java.util.stream.Collectors;

public final class VMCompilerPhases {

    

    public static final String SEGMENT_ARG = "ARG";
    public static final String SEGMENT_LOCAL = "LOCAL";

    public VMCompilerPhases() { }

    public synchronized Path compile_vm_codes_and_generate_executable(final List<String> draco_vm_codes, final String filename_without_extension) throws Exception{

        System.out.println("VM CODE COMPILATION");
        final List<String> asm_codes = AssemblyCodeGenerator.compileVMCode(draco_vm_codes);

        System.out.println("CALL NASM");
        return phase_generate_executable(asm_codes,filename_without_extension);
    }

    private synchronized Path phase_generate_executable(final List<String> asm_codes,final String filename_without_extension) throws Exception{

        final String asm_file_name = filename_without_extension+".asm";
        final Path asm_path = Paths.get(asm_file_name);
        final String asm_codes_string = asm_codes.stream().collect(Collectors.joining("\n"))+"\n";
        Files.write(
                asm_path,asm_codes_string.getBytes()
        );

        //so that the nasm call can be easily adapted with different flags
        final String nasm_command = "nasm -f elf -g -F stabs " + asm_file_name;
        System.out.println(nasm_command);

        final Process p = Runtime.getRuntime().exec(nasm_command);
        p.waitFor();

        if(p.exitValue()!=0){
            throw new Exception("nasm exit with nonzero exit code");
        }

        //so the ld call can be done seperately if someone wants to use different flags.
        final String ld_command = "ld -melf_i386 -s -o "+filename_without_extension+" "+filename_without_extension+".o";
        System.out.println(ld_command);

        final Process p2 =
                Runtime
                        .getRuntime()
                        .exec(ld_command);

        p2.waitFor();
        if(p2.exitValue() != 0){
            throw new Exception("ld exit with nonzero exit code");
        }

        Files.delete(Paths.get(filename_without_extension+".o"));

        return Paths.get(filename_without_extension);
    }

}
