add_llvm_library(LLVMPJT MODULE            # Name of the shared library that you will generate
       gather_data_pass.cpp                          # Your pass
       PLUGIN_TOOL
       opt
)