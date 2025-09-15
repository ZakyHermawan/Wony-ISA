//===----------------------------------------------------------------------===//
//
//  Describe the scheduling model of the Wony target.
//
//===----------------------------------------------------------------------===//

def WonyDefaultModel : SchedMachineModel {
  let IssueWidth = 1;
  let LoadLatency = 3;

  let CompleteModel = 0;
}

let SchedModel = WonyDefaultModel in {

let BufferSize = 1 in {
  def ALURes : ProcResource<1>;
  def MemRes : ProcResource<1>;
} // end BufferSize = 1.

let Latency = 3 in
def DefaultWriteLoad : SchedWriteRes<[MemRes]>;

let Latency = 2 in
def : WriteRes<WriteWSMUL, [ALURes]>;
def : ReadAdvance<ReadWSMULArg0, 0>;
def : ReadAdvance<ReadWSMULArg1, 1>;

def : WriteRes<WriteWUMUL, [ALURes]>;
def : ReadAdvance<ReadWUMULArg0, 0>;
def : ReadAdvance<ReadWUMULArg1, 0>;

// Setup the scheduling class for all loads, excluding
// load of immediate value.
def : InstRW<[DefaultWriteLoad], (instregex "^LD[^i]*$")>;

} // end SchedMachineModel = WonyDefaultModel.

