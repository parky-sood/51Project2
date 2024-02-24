# M151M Project #2 : Out-of-order RISC-V CPU Simulator

**Description**:

In this project, you will design an out-of-order RISC-V processor using
C++. 

The ISA of this processor is the standard RV32-I base extension.

The provided source code contains an incomplete implementation of the CPU.
The processor implementation consists of two parts:
* The CPU emulator: decode and execute instructions to generate instruction trace.
* The CPU simulator: Use the instruction trace to perform cycle-level simulation.

You are responsible for completing the code of the scoreboard and ROB such that you can successfully execute the provided tests with the correct timing.

The project is divided into 2 parts:

## Part 1 : Scoreboard
In this part, you will complete the implementation of the processor's scoreboard and re-order buffer.
Refer to **TODO** entries inside scoreboard.cpp for where you may make your changes.

## Part 3 : Gshare predictor
In this part, you will complete the implementation of the processor's branch predictor.
It will consist of a GShare predictor containing an 8-bit BHR and 256-entry BHT, matched with a 256-entry BTB.
You should complete the implementation of the GShare class by implementing the predict() method.
Note that we are only interested in timing. 
Your GShare's predict() method should first determine the current predicted outcome, and then update the predictor.
also note that a successful prediction is a combination of branch direction and branch target hits.

## Testing your code
The provided Makefile contains a `test` command to execute all provided tests.

    $ make test

All tests are under the /tests/ folder.
You can execute an individual test by running:

    $ ./tinyrv -s tests/rv32ui-p-sub.hex

If a test succeeds, you will get "PASSED!" output message.

## Debugging your code
You need to build the project with DEBUG=```LEVEL``` where level varies from 0 to 5.
That will turn on the debug trace inside the code and show you what the processor is doing and some of its internal states.

    $ make clean
    $ DEBUG=3 make
    $ ./tinyrv -s tests/rv32ui-p-sub.hex

## What to submit
**A zip file of your source code. 
When done with your changes, execute ```make submit``` to generate the submission.zip. Do not use another method for creating the zip file.

    $ make submit

Please submit the **submission.zip** file.

## Build instructions
* run ```make``` at the root of the project to build the simulator

        $ make

* A linux development environment is needed to build the project
* We recommend using a Ubuntu 18.04 or above distribution
* This C++ project requires C++ 11 to compile and it should come installed on Ubuntu 18.04 or above

## Guidelines
* Do not modify the following files: Makefile, main.cpp
* Do not modify the following directories: common
* If needed it is ok to add new files to the code or modify other parts of the codebase.
* Do not remove an existing file from the project.
* Do not change the Makefile, you can always add a new file as a header. Make sure to include the new file under the **src/** folder.

## Grading
* Scoreboard timing is correct: 7 pts.
* GShare timing is correct: 8 pts.

***Please do not procrastinate.***  

## Understanding the codebase
The codebase contains the following important files:
- Makefile: your build makefile
- config.h: the processor configuration file
- debug.h: the application debugging layer
- main.cpp: implements the application's main() entry point where the command line is parsed and the processor class is instantiated. This is also where the simulation loop is executed.
- processor.cpp: implements the processor class which contains a single core.
- core.cpp: implements the CPU simulator pipeline.
- decode.cpp: implements the emulator's instruction decode
- execute.cpp: implements emulator's instruction execution
- instr.h: implements the emulator's decoded instruction class
- pipeline.h: implements the simulator pipeline state and pipeline latches
- type.h: implements processor data structures
- common/*: implements utility libraries

When the application execute, the ```Core::tick()``` function is invoked every cycle and that is where the 5 stages of the pipeline execute. The ```if_stage()``` invokes the emulator's ```Emulator::step()``` function to obtain the current pipeline trace to simulate. Internally, the emulator ```Emulator::step()``` function will invoke ```Emulator::decode()```, and then ```Emulator::execute()``` to construct the pipeline trace.

The default implementation of the simulator is to simply forward the instruction data (```pipeline_trace```) to next stage without any stall. The test program will PASSED, but the timing will be incorrect since this design ignores stalls caused by data and control hazards.

## Resources
* RISC-V ISA Specs: https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf
* Class materials.

## Q & A's

Q1: Which development environment can I use to work on the project.
A1: The project requires a Linux environment with GCC 4.8.1 minimum.
You could work on your PC and install Ubuntu via WSL 2.0 (https://learn.microsoft.com/en-us/windows/wsl/install).
You should also be able to use the UCLA SEASNET server to work on the project. 
We recommend using the Microsoft Visual Studio Code (VSCode) as IDE if possible. 
VSCode supports remote development via SSH (https://code.visualstudio.com/docs/remote/ssh) and best, you can also do interactive debugging of C++ code (https://code.visualstudio.com/docs/cpp/cpp-debug).

Q2: How do I use the ```dump``` files to debug my code?

A2: The program instructions are loaded to address 0x80000000, which is also the starting value of the program counter (PC).
You can open the dump file and search for "80000000 <_start>:" to see the first instruction that should be executed.
follows the debugging instructions above to verify the each instruction is executing correctly.
