//===-- RISCVMCObjectWriter.cpp - RISCV ELF writer --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/RISCVMCTargetDesc.h"
#include "MCTargetDesc/RISCVMCFixups.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"

using namespace llvm;

namespace {
class RISCVObjectWriter : public MCELFObjectTargetWriter {
public:
  RISCVObjectWriter(uint8_t OSABI);

  virtual ~RISCVObjectWriter();

protected:
  // Override MCELFObjectTargetWriter.
  unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                        bool IsPCRel) const override;
};
} // end anonymouse namespace

RISCVObjectWriter::RISCVObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*Is64Bit=*/true, OSABI, ELF::EM_S390,
                            /*HasRelocationAddend=*/ true) {}

RISCVObjectWriter::~RISCVObjectWriter() {
}

// Return the relocation type for an absolute value of MCFixupKind Kind.
static unsigned getAbsoluteReloc(unsigned Kind) {
  switch (Kind) {
  case FK_Data_1: return ELF::R_390_8;
  case FK_Data_2: return ELF::R_390_16;
  case FK_Data_4: return ELF::R_390_32;
  case FK_Data_8: return ELF::R_390_64;
  }
  llvm_unreachable("Unsupported absolute address");
}

//TODO: fix relocation types
// Return the relocation type for a PC-relative value of MCFixupKind Kind.
static unsigned getPCRelReloc(unsigned Kind) {
  switch (Kind) {
  case FK_Data_2:                return ELF::R_390_PC16;
  case FK_Data_4:                return ELF::R_390_PC32;
  case FK_Data_8:                return ELF::R_390_PC64;
  case RISCV::FK_390_PC16DBL:  return ELF::R_390_PC16DBL;
  case RISCV::FK_390_PC32DBL:  return ELF::R_390_PC32DBL;
  case RISCV::FK_390_PLT16DBL: return ELF::R_390_PLT16DBL;
  case RISCV::FK_390_PLT32DBL: return ELF::R_390_PLT32DBL;
  case RISCV::FK_390_PLT64DBL: return ELF::R_390_PLT64;
  }
  llvm_unreachable("Unsupported PC-relative address");
}

// Return the R_390_TLS_LE* relocation type for MCFixupKind Kind.
static unsigned getTLSLEReloc(unsigned Kind) {
  switch (Kind) {
  case FK_Data_4: return ELF::R_390_TLS_LE32;
  case FK_Data_8: return ELF::R_390_TLS_LE64;
  }
  llvm_unreachable("Unsupported absolute address");
}

// Return the PLT relocation counterpart of MCFixupKind Kind.
static unsigned getPLTReloc(unsigned Kind) {
  switch (Kind) {
  case RISCV::FK_390_PC16DBL: return ELF::R_390_PLT16DBL;
  case RISCV::FK_390_PC32DBL: return ELF::R_390_PLT32DBL;
  }
  llvm_unreachable("Unsupported absolute address");
}

unsigned RISCVObjectWriter::GetRelocType(const MCValue &Target,
                                         const MCFixup &Fixup,
                                         bool IsPCRel) const {
  MCSymbolRefExpr::VariantKind Modifier = (Target.isAbsolute() ?
                                           MCSymbolRefExpr::VK_None :
                                           Target.getSymA()->getKind());
  unsigned Kind = Fixup.getKind();
  switch (Modifier) {
  case MCSymbolRefExpr::VK_None:
    if (IsPCRel)
      return getPCRelReloc(Kind);
    return getAbsoluteReloc(Kind);

  case MCSymbolRefExpr::VK_NTPOFF:
    assert(!IsPCRel && "NTPOFF shouldn't be PC-relative");
    return getTLSLEReloc(Kind);

  case MCSymbolRefExpr::VK_GOT:
    if (IsPCRel && Kind == RISCV::FK_390_PC32DBL)
      return ELF::R_390_GOTENT;
    llvm_unreachable("Only PC-relative GOT accesses are supported for now");

  case MCSymbolRefExpr::VK_PLT:
    assert(IsPCRel && "@PLT shouldt be PC-relative");
    return getPLTReloc(Kind);

  default:
    llvm_unreachable("Modifier not supported");
  }
}

MCObjectWriter *llvm::createRISCVObjectWriter(raw_pwrite_stream &OS,
                                                uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new RISCVObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/false);
}
