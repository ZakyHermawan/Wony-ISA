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

WonyTargetLowering::WonyTargetLowering(const TargetMachine &TM,
                                         const WonySubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
  // call addRegisterClass to register all legal types
  addRegisterClass(MVT::i16, &Wony::GPR16RegClass);
  addRegisterClass(MVT::i32, &Wony::GPR32RegClass);

  // Finalize the registration process and compute all the information that SDISel may need.
  // Tell the generic implementation that we are done with setting up our
  // register classes.
  computeRegisterProperties(Subtarget.getRegisterInfo());
}

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

  // Populate ArgLocs
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

SDValue
WonyTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {

  SmallVector<CCValAssign> RetValLocs;
  MachineFunction &MF = DAG.getMachineFunction();

  // Populate RetValLocs
  CCState CCInfo(CallConv, IsVarArg, MF, RetValLocs, *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_Wony_Common);

  SDValue Glue;

  // Vector to store all the operands needed for the target-specific return instruction node in SelectionDAG lowering
  SmallVector<SDValue> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (size_t i = 0, e = RetValLocs.size(); i != e; ++i) {
    CCValAssign &VA = RetValLocs[i];
    assert(VA.isRegLoc() && "stack return not yet implemented");
    assert(VA.getLocInfo() == CCValAssign::Full &&
           "extension/truncation of any sort, not yet implemented");

    // Create SDNode getCopyToReg,  
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Glue);

    // Guarantee that all emitted copies are stuck together,
    // avoiding something bad.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  // The return must have the last value of the chain.
  // Update it now.
  RetOps[0] = Chain;

  // Add the glue if we have it.
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  return DAG.getNode(WonyISD::RETURN_GLUE, DL, MVT::Other, RetOps);
}

const char *WonyTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((WonyISD::NodeType)Opcode) {
  case WonyISD::FIRST_NUMBER:
    break;
  case WonyISD::RETURN_GLUE:
    return "WonyISD::RETURN_GLUE";
  }
  return nullptr;
}
