//===----------------------------------------------------------------------===//
//
// Describes the scheduling events modeled in the Wony target.
//
//===----------------------------------------------------------------------===//

// Widening signed multiply events.
def WriteWSMUL : SchedWrite;
def ReadWSMULArg0 : SchedRead;
def ReadWSMULArg1 : SchedRead;

// Widening unsigned multiply events.
def WriteWUMUL : SchedWrite;
def ReadWUMULArg0 : SchedRead;
def ReadWUMULArg1 : SchedRead;
