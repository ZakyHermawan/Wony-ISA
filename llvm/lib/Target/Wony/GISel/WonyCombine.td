//===----------------------------------------------------------------------===//
//
// Declare GlobalISel combine rules and provide mechanisms to opt-out.
//
//===----------------------------------------------------------------------===//

include "llvm/Target/GlobalISel/Combine.td"

def registers_matchinfo: GIDefMatchData<"SmallVector<Register>">;

def insertvectorelt_to_build_vector : GICombineRule<
  (defs root:$root, registers_matchinfo:$matchinfo),
  (match (wip_match_opcode G_INSERT_VECTOR_ELT):$root,
         [{ return matchInsertVectorElt(*${root}, ${matchinfo}); }]),
  (apply [{ applyInsertVectorElt(*${root}, ${matchinfo}); }])>;

def WonyMandatoryPreLegalizerCombiner: GICombiner<
  "WonyMandatoryPreLegalizerCombinerImpl", [optnone_combines, insertvectorelt_to_build_vector]> {
}

// Turn G_UNMERGE_VALUES -> G_EXTRACT_VECTOR_ELT's
// This implementation comes from AArch64, this should just put it in the legalizer helper.
def vector_unmerge_lowering : GICombineRule <
  (defs root:$root),
  (match (wip_match_opcode G_UNMERGE_VALUES):$root,
          [{ return matchScalarizeVectorUnmerge(*${root}, MRI); }]),
  (apply [{ applyScalarizeVectorUnmerge(*${root}, MRI, B); }])
>;

def WonyMandatoryPostLegalizerCombiner : GICombiner<
  "WonyMandatoryPostLegalizerCombinerImpl", [vector_unmerge_lowering]> {
}
