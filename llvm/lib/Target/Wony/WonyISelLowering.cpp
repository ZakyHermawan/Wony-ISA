//===----------------------------------------------------------------------===//
//
// This file implements the interfaces that Wony uses to lower LLVM code
// into a selection DAG.
//
//===----------------------------------------------------------------------===//

#include "Wony.h"
#include "WonySubtarget.h"
#include "WonyISelLowering.h"
#include "WonyTargetMachine.h"
#include "WonyCallingConvention.h"

#include "llvm/CodeGen/MachineFrameInfo.h"

using namespace llvm;

#define DEBUG_TYPE "wony-lowering"

WonyTargetLowering::WonyTargetLowering(const TargetMachine &TM)
    : TargetLowering(TM) {}

FastISel *
WonyTargetLowering::createFastISel(FunctionLoweringInfo &funcInfo,
                                    const TargetLibraryInfo *libInfo) const {
  return Wony::createFastISel(funcInfo, libInfo);
}

SDValue WonyTargetLowering::LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                                                 bool IsVarArg,
                                                 const SmallVectorImpl<ISD::InputArg> &Ins,
                                                 const SDLoc &DL, SelectionDAG &DAG,
                                                 SmallVectorImpl<SDValue> &InVals) const {

  if (IsVarArg) {
    report_fatal_error("variadic functions, not yet implemented");
  }

  MachineFunction &MF = DAG.getMachineFunction();
  if (MF.getFunction().hasStructRetAttr()) {
    report_fatal_error("aggregate returns, not yet implemented");
  }

  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Wony_Common);

  for(size_t I = 0; I<ArgLocs.size(); ++I) {
    auto& VA = ArgLocs[I];
    SDValue ArgValue;

    if(VA.isRegLoc()) {
      if(VA.getLocInfo() != CCValAssign::Full) {
        report_fatal_error("partial type, not yet implemented");
      }

      EVT RegVT = VA.getValVT();
      TypeSize TySizeInBits = RegVT.getSizeInBits();

      const TargetRegisterClass *DstRC = nullptr;
      switch (TySizeInBits) {
      default:
        report_fatal_error("argument type, not yet implemented");
      case 16:
        DstRC = &Wony::GPR16RegClass;
        break;
      case 32:
        DstRC = &Wony::GPR32RegClass;
        break;
      }
      Register VReg = RegInfo.createVirtualRegister(DstRC);
      RegInfo.addLiveIn(VA.getLocReg(), VReg);
      ArgValue = DAG.getCopyFromReg(Chain, DL, VReg, RegVT);
    }
    else {
      assert(VA.isMemLoc() && "CCValAssign is neither reg nor mem");
      if(VA.getLocInfo() != CCValAssign::Full) {
        report_fatal_error("support only value directly in the stack");
      }
      unsigned ArgOffset = VA.getLocMemOffset();
      unsigned ArgSize = VA.getValVT().getSizeInBits() / 8;
      int FrameIdx = MFI.CreateFixedObject(ArgSize, ArgOffset, /*IsImmutable=*/true);

      // Create load nodes to retrieve arguments from the stack.
      SDValue FrameIdxNode =
          DAG.getFrameIndex(FrameIdx, getPointerTy(DAG.getDataLayout()));
      MachinePointerInfo PtrInfo =
          MachinePointerInfo::getFixedStack(MF, FrameIdx);

      // We support only full loads for now, so no extension whatsoever.
      ISD::LoadExtType ExtType = ISD::NON_EXTLOAD;
      MVT MemVT = VA.getValVT();

      ArgValue = DAG.getExtLoad(ExtType, DL, VA.getLocVT(), Chain, FrameIdxNode,
                                PtrInfo, MemVT);
    }
    InVals.push_back(ArgValue);
  }

  return Chain;
}
