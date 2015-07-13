//===-- RISCVFrameLowering.h - Frame lowering for RISCV -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef RISCVFRAMELOWERING_H
#define RISCVFRAMELOWERING_H

#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
class RISCVTargetMachine;
class RISCVSubtarget;

class RISCVFrameLowering : public TargetFrameLowering {
public:
  RISCVFrameLowering();

  bool hasFP(const MachineFunction &MF) const;

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction&, MachineBasicBlock&) const;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const;

  bool hasReservedCallFrame(const MachineFunction &MF) const;

  void processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
                                            RegScavenger *RS) const;
  unsigned ehDataReg(unsigned I) const;
};

} // End llvm namespace

#endif
