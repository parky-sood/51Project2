// Copyright 2024 blaise
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <assert.h>
#include <util.h>
#include "types.h"
#include "core.h"
#include "debug.h"

using namespace tinyrv;

GShare::GShare() {
  //--
}

GShare::~GShare() {
  //--
}

bool GShare::predict(pipeline_trace_t* trace) {
  // predict
  uint8_t bht_index = ((trace->PC) >> 2) ^ BHR;
  uint8_t btb_index = ((trace->PC) >> 2) & 0xFF;
  bool taken = (BHT[bht_index] >= 2) ? true : false;

  Word predict_addr = 0xFFFFFFFF;
  if(taken)
  {
    if (BTB[btb_index].TAG == ((trace->PC >> 10))) {
      predict_addr = (BTB[btb_index].target) << 2;
    }
  }
  else {
    predict_addr = trace->PC + 4;
  }
  

  bool actual = (trace->PC+4 != trace->nextPC) ? true : false;
  bool correct_predict = (predict_addr == trace->nextPC) ? true : false;

  // update
  BHR = (BHR << 1) | actual;
  if (actual) {
    if (BHT[bht_index] < 3)
      BHT[bht_index]++;
  }
  else {
    if (BHT[bht_index] > 0)
      BHT[bht_index]--;
  }
  if (actual) {
    BTB[btb_index].target = (trace->nextPC) >> 2;
    BTB[btb_index].TAG = (trace->PC) >> 10;
  }

  return correct_predict;
}

// std::cout << "DEBUG *** GShare: BHR=0x" << std::hex << static_cast<unsigned>(BHR) 
//           << ", PHT_index=" << std::hex << static_cast<unsigned>(bht_index)
//           << ", PHT_taken=" << (BHT[bht_index] > 1) 
//           << ", BTB_nextPC=0x" << std::hex << predict_addr 
//           << ": " << *trace << std::endl;
//   if (!taken) {
//     std::cout << "DEBUG *** GShare: predicted not-taken=" << correct_predict 
//           << ": " << *trace << std::endl;
//   }
//   else {
// std::cout << "DEBUG *** GShare: predicted taken=" << correct_predict 
//           << ": " << *trace << std::endl;
//   }