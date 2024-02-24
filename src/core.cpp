// Copyright 2024 Blaise Tine
// 
// Licensed under the Apache License;
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <iomanip>
#include <string.h>
#include <assert.h>
#include <util.h>
#include "types.h"
#include "core.h"
#include "debug.h"
#include "processor_impl.h"

using namespace tinyrv;

extern bool gshare_enabled;

Core::Core(const SimContext& ctx, uint32_t core_id, ProcessorImpl* processor)
    : SimObject(ctx, "core")
    , core_id_(core_id)
    , processor_(processor)
    , emulator_(this)
    , RAT_(NUM_REGS)
    , scoreboard_(this, NUM_RSS, ROB_SIZE)
{
  // create functional units
  FUs_[(int)FUType::ALU] = FunctionalUnit::Create(ALU_LATENCY);
  FUs_[(int)FUType::LSU] = FunctionalUnit::Create(LSU_LATENCY);
  FUs_[(int)FUType::CSR] = FunctionalUnit::Create(CSR_LATENCY);

  // create the ROB
  ROB_ = ReorderBuffer::Create(this, ROB_SIZE);

  this->reset();
}

Core::~Core() {}

void Core::reset() { 
  emulator_.clear();
  stalled_trace_ = nullptr;
  issue_stalls_ = 0;
  issued_instrs_ = 0;
  perf_stats_ = PerfStats();
}

void Core::tick() {
  this->commit();
  this->writeback();
  this->execute();
  this->issue();

  scoreboard_.dump();
  ROB_->dump();

  ++perf_stats_.cycles;
  DPN(2, std::flush);  
}

void Core::issue() {
  auto trace = stalled_trace_;
  if (issue_stalls_ != 0) {
    --issue_stalls_;
    DT(3, "*** pipeline-issue stalled!: " << *trace);
    return;
  }

  if (trace == nullptr) {
    trace = emulator_.step();
    stalled_trace_ = trace;
    if (gshare_enabled) {
      if (!gshare_.predict(trace)) {
        issue_stalls_ = 2;
        return;
      }
    }
  }

  if (!scoreboard_.issue(trace)) {
    DT(3, "*** pipeline-issue stalled!: " << *trace);
    return;
  }

  DT(3, "pipeline-issue: " << *trace);

  stalled_trace_ = nullptr;
  ++issued_instrs_;
}

void Core::execute() {   
  auto traces = scoreboard_.execute();
  for (auto trace : traces) {
    __unused (trace);
    DT(3, "pipeline-execute: " << *trace);
  }
}

void Core::writeback() {
  auto trace = scoreboard_.writeback();
  if (trace) {
    __unused (trace);
    DT(3, "pipeline-writeback: " << *trace);
  }
}

void Core::commit() {
  if (ROB_->Committed.empty())
    return;

  auto trace = ROB_->Committed.front();
  
  DT(3, "pipeline-commit: " << *trace);

  assert(perf_stats_.instrs <= issued_instrs_);
  ++perf_stats_.instrs;

  ROB_->Committed.pop();

  // delete the trace
  delete trace;
}

bool Core::check_exit(Word* exitcode, bool riscv_test) const {
  return emulator_.check_exit(exitcode, riscv_test);
}

bool Core::running() const {
  return (perf_stats_.instrs != issued_instrs_);
}

void Core::attach_ram(RAM* ram) {
  emulator_.attach_ram(ram);
}

void Core::showStats() {
  std::cout << std::dec << "PERF: instrs=" << perf_stats_.instrs << ", cycles=" << perf_stats_.cycles << std::endl;
}