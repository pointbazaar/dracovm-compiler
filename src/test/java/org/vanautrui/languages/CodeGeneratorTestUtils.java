package org.vanautrui.languages;

import org.vanautrui.languages.vmcompiler.VMCompilerPhases;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

public class CodeGeneratorTestUtils {

    public static Path  generateFromVMCodeAndWriteExecutable(List<String> vmcodes,String filename)throws Exception{
        VMCompilerPhases phases = new VMCompilerPhases();
        List<String> asm_codes = phases.phase_vm_code_compilation(vmcodes,false);
        Path path = phases.phase_generate_executable(asm_codes,filename);

        return path;
    }


    public static Process compile_and_run_vmcodes_but_not_waitFor(List<String> vmcodes,String filename_without_extension) throws Exception{
        generateFromVMCodeAndWriteExecutable(vmcodes,filename_without_extension);
        Process pr = Runtime.getRuntime().exec("./"+filename_without_extension);
        return pr;
    }

    public static Process compile_and_run_vm_codes_for_testing(List<String> vmcodes,String filename_without_extension) throws Exception{

        Process pr = compile_and_run_vmcodes_but_not_waitFor(vmcodes,filename_without_extension);

        pr.waitFor();
        Files.delete(Paths.get(filename_without_extension));
        Path vmpath = Paths.get(filename_without_extension + ".dracovm");
        if(Files.exists(vmpath)) {
            Files.delete(vmpath);
        }
        Files.delete(Paths.get(filename_without_extension+".asm"));

        return pr;
    }

}
