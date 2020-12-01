add_llvm_library( LLVMEncodeFunctionName MODULE
        EncodeFunctionName.cpp

        PLUGIN_TOOL
        opt
        )