package org.vanautrui.languages;

import org.vanautrui.languages.vmcompiler.VMCompilerPhases;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

public final class CodeGeneratorTestUtils {

    public static Path  generateFromVMCodeAndWriteExecutable(List<String> vmcodes,String filename)throws Exception{

        return (new VMCompilerPhases()).compile_vm_codes_and_generate_executable(vmcodes,filename);
    }


    public static Process compile_and_run_vmcodes_but_not_waitFor(List<String> vmcodes,String filename_without_extension) throws Exception{
        generateFromVMCodeAndWriteExecutable(vmcodes,filename_without_extension);
        return Runtime.getRuntime().exec("./"+filename_without_extension);
    }

    public static Process compile_and_run_vm_codes_for_testing(List<String> vmcodes,String filename_without_extension) throws Exception{

        final Process pr = compile_and_run_vmcodes_but_not_waitFor(vmcodes,filename_without_extension);

        pr.waitFor();
        Files.delete(Paths.get(filename_without_extension));
        final Path vmpath = Paths.get(filename_without_extension + ".dracovm");
        if(Files.exists(vmpath)) {
            Files.delete(vmpath);
        }
        Files.delete(Paths.get(filename_without_extension+".asm"));

        return pr;
    }

}
